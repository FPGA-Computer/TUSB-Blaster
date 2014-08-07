/*-----------------------------------------------------------------------------
 * Code that turns a Cypress FX2 USB Controller into an USB JTAG adapter
 *-----------------------------------------------------------------------------
 * Copyright (C) 2005..2007 Kolja Waschk, ixo.de
 *-----------------------------------------------------------------------------
 * Check hardware.h/.c if it matches your hardware configuration (e.g. pinout).
 * Changes regarding USB identification should be made in product.inc!
 *-----------------------------------------------------------------------------
 * This code is part of usbjtag. usbjtag is free software; you can redistribute
 * it and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version. usbjtag is distributed in the hope
 * that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.  You should have received a
 * copy of the GNU General Public License along with this program in the file
 * COPYING; if not, write to the Free Software Foundation, Inc., 51 Franklin
 * St, Fifth Floor, Boston, MA  02110-1301  USA
 *-----------------------------------------------------------------------------
 */

/* 
  code heavily modified by K. C. Lee for TI TUSB3410
  
*/

#include <string.h>
#include "main.h"

static BOOL Running;
static BOOL WriteOnly;

static byte ClockBytes;
static word Pending;

static WORD FirstDataInOutBuffer;
static WORD FirstFreeInOutBuffer;

void main()
{
  WDCSR = 0x2a;		// Disable watchdog!!  needed for TUSB3410
  
  uart_init();

  /* This connects to the USB bus and initializes EP0 */
  USB_Init();
  
  // eeprom emulator
  eeprom_init();
  usb_jtag_init();  

  while(1)
  {  
    usb_jtag_activity();
    }
}

void USB_Handle_Class_Request()
{  EP0_Stall();
 }
 
void USB_Handle_Vendor_Request()
{ ubyte Buffer[2];

  if (usb_setup_buffer.bRequestType & USB_ENDPOINT_DIR_MASK)		// Dir == In
  {
     if(usb_setup_buffer.bRequest == 0x90)							// FTDI eeprom emulation
     {
        BYTE addr = (wIndexL<<1) & 0x7F;
        Buffer[0] = eeprom[addr];
        Buffer[1] = eeprom[addr+1];
       }
     else
     {
       // dummy data
        Buffer[0] = 0x36;
        Buffer[1] = 0x83;
       }
      usb_write_ep0_buffer(Buffer, 2);
    }  
  else														// Dir == Out
  {  
      if(bRequest == USB_REQ_GET_STATUS)
        Running = 1;
	
      usb_ack_ep0_out();
    }
 }

void USB_Handle_Reset(void)
{
  // Input 1 In Endpoint Descriptor Block*/
  IEPCNF_1 = UBME;                      					// Input endpoint 1 configuration
  IEPBBAX_1 = BUF_ADDR(USB_EP1_IBUF);   			// Input endpoint 1 X buffer base address
  IEPSIZXY_1 = sizeof(ep1_in_x);    					// Input endpoint 1 X/Y buffer size 
  IEPBCTX_1 = NAK;                        					// Input endpoint 1 X byte count
  //IEPBBAY_1;                          					// Output endpoint 1 Y buffer base address
  //IEPBBTY_1;                          					// Output endpoint 1 Y byte count

// Input 2 Out Endpoint Descriptor Block
  OEPCNF_2 = UBME;                      					// Output endpoint 2 configuration
  OEPBBAX_2 = BUF_ADDR(USB_EP2_OBUF);			// Output endpoint 2 X buffer base address
  //OEPBBAY_2;                          					// Output endpoint 2 Y buffer base address
  //OEPBBTY_2;                          					// Output endpoint 2 Y byte count
  OEPSIZXY_2 = sizeof(ep2_out_x);    				// Output endpoint 2 X/Y buffer size
  OEPBCTX_2 = 0;                        					// Output endpoint 2 X byte count
 }

//-----------------------------------------------------------------------------

void usb_jtag_init(void)              // Called once at startup
{ word tmp;

   Running = FALSE;
   ClockBytes = 0;
   Pending = 0;
   WriteOnly = TRUE;

   ProgIO_Init();
   ProgIO_Enable();

   // Make Timer2 reload at 100 Hz to trigger Keepalive packets

   tmp = 65536 - ( CPU_OSC / 12 / 100 );
   RCAP2H = tmp >> 8;
   RCAP2L = tmp & 0xFF;
   T2CON = 0x04; 				// Auto-reload mode using internal clock, no baud clock. 
}

void OutputByte(BYTE d)
{
   OutBuffer[FirstFreeInOutBuffer++] = d;
   if(FirstFreeInOutBuffer >= OUTBUFFER_LEN) FirstFreeInOutBuffer = 0;
   Pending++;
}

//-----------------------------------------------------------------------------
// usb_jtag_activity does most of the work. It now happens to behave just like
// the combination of FT245BM and Altera-programmed EPM7064 CPLD in Altera's
// USB-Blaster. The CPLD knows two major modes: Bit banging mode and Byte
// shift mode. It starts in Bit banging mode. While bytes are received
// from the host on EP2OUT, each byte B of them is processed as follows:
//
// Please note: nCE, nCS, LED pins and DATAOUT actually aren't supported here.
// Support for these would be required for AS/PS mode and isn't too complicated,
// but I haven't had the time yet.
//
// Bit banging mode:
// 
//   1. Remember bit 6 (0x40) in B as the "Read bit".
//
//   2. If bit 7 (0x40) is set, switch to Byte shift mode for the coming
//      X bytes ( X := B & 0x3F ), and don't do anything else now.
//
//    3. Otherwise, set the JTAG signals as follows:
//        TCK/DCLK high if bit 0 was set (0x01), otherwise low
//        TMS/nCONFIG high if bit 1 was set (0x02), otherwise low
//        nCE high if bit 2 was set (0x04), otherwise low
//        nCS high if bit 3 was set (0x08), otherwise low
//        TDI/ASDI/DATA0 high if bit 4 was set (0x10), otherwise low
//        Output Enable/LED active if bit 5 was set (0x20), otherwise low
//
//    4. If "Read bit" (0x40) was set, record the state of TDO(CONF_DONE) and
//        DATAOUT(nSTATUS) pins and put it as a byte ((DATAOUT<<1)|TDO) in the
//        output FIFO _to_ the host (the code here reads TDO only and assumes
//        DATAOUT=1)
//
// Byte shift mode:
//
//   1. Load shift register with byte from host
//
//   2. Do 8 times (i.e. for each bit of the byte; implemented in shift.a51)
//      2a) if nCS=1, set carry bit from TDO, else set carry bit from DATAOUT
//      2b) Rotate shift register through carry bit
//      2c) TDI := Carry bit
//      2d) Raise TCK, then lower TCK.
//
//   3. If "Read bit" was set when switching into byte shift mode,
//      record the shift register content and put it into the FIFO
//      _to_ the host.
//
// Some more (minor) things to consider to emulate the FT245BM:
//
//   a) The FT245BM seems to transmit just packets of no more than 64 bytes
//      (which perfectly matches the USB spec). Each packet starts with
//      two non-data bytes (I use 0x31,0x60 here). A USB sniffer on Windows
//      might show a number of packets to you as if it was a large transfer
//      because of the way that Windows understands it: it _is_ a large
//      transfer until terminated with an USB packet smaller than 64 byte.
//
//   b) The Windows driver expects to get some data packets (with at least
//      the two leading bytes 0x31,0x60) immediately after "resetting" the
//      FT chip and then in regular intervals. Otherwise a blue screen may
//      appear... In the code below, I make sure that every 10ms there is
//      some packet.
//
//   c) Vendor specific commands to configure the FT245 are mostly ignored
//      in my code. Only those for reading the EEPROM are processed. See
//      DR_GetStatus and DR_VendorCmd below for my implementation.
//
//   All other TD_ and DR_ functions remain as provided with CY3681.
//
//-----------------------------------------------------------------------------

void usb_jtag_activity(void) // Called repeatedly while the device is idle
{ xubyte *pointer1, *pointer2;

   if(!Running) return;

   ProgIO_Poll();
   
   // IN1 Buffer is empty
   if ((IEPBCTX_1 &  NAK))					// (!(EP1INCS & bmEPBUSY)) 	
   {
      if(Pending > 0)
      {
         BYTE o, n;

         pointer2 = (xubyte*) ep1_in_x;			// AUTOPTRH2 = MSB( EP1INBUF );
									// AUTOPTRL2 = LSB( EP1INBUF );
	 *(pointer2++ ) = 0x31;				// XAUTODAT2 = 0x31;
	 *(pointer2++ ) = 0x60;				// XAUTODAT2 = 0x60;
       
         if(Pending > 0x3E) 
	 { n = 0x3E; 
	    Pending -= n;
           } 
	 else 
	 { n = Pending; 
	    Pending = 0;
           };
	   
         o = n;
         pointer1 = (xubyte*)  &(OutBuffer[FirstDataInOutBuffer]); 	// APTR1H = MSB( &(OutBuffer[FirstDataInOutBuffer]) );
												// APTR1L = LSB( &(OutBuffer[FirstDataInOutBuffer]) );	 
         while(n--)
         {
	     *(pointer2++ ) = *(pointer1++);					// XAUTODAT2 = XAUTODAT1;

            if(++FirstDataInOutBuffer >= OUTBUFFER_LEN)
            {
               FirstDataInOutBuffer = 0;
	       pointer1 = (xubyte*)  OutBuffer;					// APTR1H = MSB( OutBuffer );
												// APTR1L = LSB( OutBuffer );
              }
           }
	   
         // SYNCDELAY;
         // EP1INBC = 2 + o;
         // TF2 = 1; // Make sure there will be a short transfer soon
	 
	 IEPBCTX_1 = 2 + o;	 	 
        }
      else if(TF2)			// Poll timer 2 interrupt flag
      {
         ep1_in_x[0] =  0x31;		// EP1INBUF[0] = 0x31;
         ep1_in_x[1] =  0x60; 		// EP1INBUF[1] = 0x60;

	 IEPBCTX_1 = 2;			//  EP1INBC = 2;
         TF2 = 0;
        }
    }

   // Out 2 Buffer has packet
   
   if (	( OEPBCTX_2 &	NAK ) && 			// (!(EP2468STAT & bmEP2EMPTY) && (Pending < OUTBUFFER_LEN-0x3F))
        (Pending < OUTBUFFER_LEN-0x3F))
   {
      //BYTE i, n = EP2BCL; // bugfix by Sune Mai (Oct 2008, https://sourceforge.net/projects/urjtag/forums/forum/682993/topic/2312452)
      
      word i, n = OEPBCTX_2 & 0x7f;  			// WORD i, n = EP2BCL|EP2BCH<<8;

      pointer1 = (xubyte *) ep2_out_x;			// APTR1H = MSB( EP2FIFOBUF );
									// APTR1L = LSB( EP2FIFOBUF );
      for(i=0;i<n;)
      {
         if(ClockBytes > 0)
         {
            //BYTE m; // bugfix by Sune Mai (Oct 2008, https://sourceforge.net/projects/urjtag/forums/forum/682993/topic/2312452)
            WORD m;

            m = n-i;
            if(ClockBytes < m) m = ClockBytes;
            ClockBytes -= m;
            i += m;

            /* Shift out 8 bits from d */
         
            if(WriteOnly)
            { 
	       Multibyte_ProgIO(m,pointer1) ;									// while(m--) ProgIO_ShiftOut(*(++));
               pointer1+= m;
             }	     
            else /* Shift in 8 bits at the other end  */
            {
               while(m--) 
	           OutputByte(ProgIO_ShiftInOut(*(pointer1++)));
             }
         }
        else
        {
	    BYTE d = *(pointer1++);										// BYTE d = XAUTODAT1;
            WriteOnly = (d & bmBIT6) ? FALSE : TRUE;

            if(d & bmBIT7)
            {
               /* Prepare byte transfer, do nothing else yet */
               ClockBytes = d & 0x3F;
              }
            else
            {
               if(WriteOnly)
                   ProgIO_Set_State(d);
               else
                   OutputByte(ProgIO_Set_Get_State(d));
              }
            i++;
          }
       }

      SYNCDELAY;
      OEPBCTX_2 = 0;				// EP2BCL = 0x80; // Re-arm endpoint 2
   }
}


/* The End */
