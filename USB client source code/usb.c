#include "main.h"

uword status=0;

// keep track of data transfer across interrupts

ubyte EP0_Status;
uword IEP0_PacketRemaining, OEP0_PacketRemaining;
ubyte *IEP0_Buffer, *OEP0_Buffer;

void USB_Init(void)
{ 
  EA  = 0;
  
  // disconnect from USB, firmware handles USB reset
  USBCTL = 0x00;

  // set i2c speed
  // i2cSetBusSpeed(I2C_400KHZ);
  
  // Enable the USB-specific Interrupts; SETUP, RESET and STPOW
  USBMSK = STPOW | SETUP | RSTR;
  
  // Enable global and USB interrupt
  EA  = 1;
  EX0 = 1;

  USBCTL = CONT;
}

void USB_Reset(void)
{
  FUNADR = 0x00;      // no device address
// this register needs to be
// cleared before enumeration

  USBSTA = 0xff;        // Clear all interrupts
  
  IEP0_PacketRemaining = OEP0_PacketRemaining = 0;
  IEP0_Buffer = OEP0_Buffer = NULL;
  EP0_Status = EP_IDLE;
  
// enable endpoint 0 interrupt
  IEPCNFG_0 = USBIE | UBME;
  OEPCNFG_0 = USBIE | UBME;
//  IEPBCNT_0 =  0;
//  OEPBCNT_0 = 0;

// enable all necessary endpoints here…
  USB_Handle_Reset();
 }

void usb_ack_ep0_in(void)
{
  /* ACK the status stage */
  OEPBCNT_0 = 0;
}

void usb_ack_ep0_out(void)
{
  /* Send a response to a dataless request */
  OEPCNFG_0 |= STALL;
  IEPBCNT_0 = 0;
 }
 
void EP0_Stall(void)
{
    IEPCNFG_0 |= STALL | UBME;
    OEPCNFG_0 |= STALL | UBME;
}

void EP0_SendNextPacket(void)
{ ubyte PacketSize, i;

  if(IEP0_PacketRemaining)
  {
    if(IEP0_PacketRemaining > EP0_MAXSIZE)
    { PacketSize = EP0_MAXSIZE;
       IEP0_PacketRemaining -= EP0_MAXSIZE;
//       EP0_Status = EP_DATA_IN;
     }
    else
    { PacketSize = IEP0_PacketRemaining;
    
       if(PacketSize < EP0_MAXSIZE)			// Last packet
          EP0_Status = EP_IDLE;
       else
	  EP0_Status |= EP_ZEROPACK;			// need a zero packet
	  
      IEP0_PacketRemaining = 0;
     }

    for(i=0;i<PacketSize;i++)
      usb_ep0in_buffer[i] = *IEP0_Buffer++;

    IEPBCNT_0 = PacketSize;
   }
  else
  { 
    if(EP0_Status &  EP_ZEROPACK)
      IEPBCNT_0 = 0;			// Send a zero sized one when we have no more to send     
 
    EP0_Status = EP_IDLE;
    USBCTL &= ~SIR;			// Done Servicing Setup Interrupt
   } 
 }

void EP0_GetNextPacket(void)
{ ubyte PacketSize, i;

  PacketSize = OEPBCNT_0 & IEPBCNT_0_SIZEMASK;

  if(OEP0_PacketRemaining && (OEP0_PacketRemaining >= PacketSize))
  { 
    for(i=0;i<PacketSize;i++)
    { 
      *OEP0_Buffer++ = usb_ep0out_buffer[i];
     }
    IEP0_PacketRemaining -= PacketSize;
      
    // Still expecting more packets
    if(IEP0_PacketRemaining)
    {   
      EP0_Status = EP_DATA_IN;
      OEPBCNT_0 = 0;
      return;              
     }
   }
  
  EP0_Stall();
  EP0_Status = EP_IDLE;
 }

void usb_write_ep0_buffer(ubyte *Buffer, uword Size)
{
  if(Size)
  {   
    IEP0_Buffer = Buffer;
    EP0_Status = EP_DATA_IN; 
       
    if(Size > usb_setup_buffer.wLength)				// Too large - Truncate to host buffer size
      IEP0_PacketRemaining = usb_setup_buffer.wLength;
    else
    { if (Size < usb_setup_buffer.wLength)
         EP0_Status = EP_DATA_IN | EP_ZEROPACK;		// Host wanted more
       
       IEP0_PacketRemaining = Size;
     }
     
    OEPBCNT_0  = 0;    							// abort out?
    EP0_SendNextPacket();
   }
  else
    EP0_Status = EP_IDLE;   
 }

void USB_Handle_Setup(void)
{ 
  IEPBCNT_0 = NAK;
  OEPBCNT_0 = NAK;
  USBSTA = SETUP;
      
  USBCTL |= SIR;                                // Servicing Setup Interrupt

  EP0_Status = EP_IDLE;
  
  if (usb_setup_buffer.bRequestType & USB_DIR_IN)
    USBCTL |= DIR;
  else
    USBCTL &= ~DIR;

 /* Look at the type field to start finding a request handler */
  switch(usb_setup_buffer.bRequestType & USB_TYPE_MASK)
  { case USB_TYPE_STANDARD:

      switch (usb_setup_buffer.bRequest)
      {
        case USB_REQ_GET_STATUS:
          usb_write_ep0_buffer((ubyte *)&status,sizeof(status));
          break;

        case USB_REQ_SET_ADDRESS:
          /* Set the device address and send a 0-length response */
          FUNADR = usb_setup_buffer.wValue;
          usb_ack_ep0_out();
          USBCTL &= ~SIR;                       // Done Servicing Setup Interrupt
          break;

        case USB_REQ_GET_DESCRIPTOR:
          /* Send back one of our descriptors from the table */
          usb_handle_descriptor_request();	  
          break;
          
        case USB_REQ_SET_CONFIGURATION:
        case USB_REQ_SET_INTERFACE:

          usb_ack_ep0_out();				// We only have one configuration/interface, so nothing to do
          USBCTL &= ~SIR;                       	// Done Servicing Setup Interrupt
          break;
	  
	default:
           EP0_Stall();
	   USBCTL &= ~SIR; 
        }
      break;
     
    case USB_TYPE_VENDOR:
      USB_Handle_Vendor_Request();
      USBCTL &= ~SIR;       
      break;
   }
 }

void USB_Interrupt(void) __interrupt 0
{ 
  EA = 0;
  
  switch(VECINT)
  {
    case USB_IRQ_IEND_0:

      OEPBCNT_0 = 0;

      if(EP0_Status & EP_DATA_IN)
        EP0_SendNextPacket();
      else
        IEPCNFG_0 |= STALL;
      break;
      
    case USB_IRQ_OEND_0:
      IEPBCNT_0 = 0;

      if(EP0_Status & EP_DATA_OUT)
        EP0_GetNextPacket();
      else
        OEPCNFG_0 |= STALL;                
      break;
      
    case USB_IRQ_SETUP:         // Setup transaction
      USB_Handle_Setup();
      break;
    
    case USB_IRQ_STPOW:         // Setup overwrite
      USB_Handle_Setup();
      USBSTA = STPOW;
      break;
    
    case USB_IRQ_RSTR:          // Function reset request
      USB_Reset();
      USBSTA = RSTR;
      break;
   }
  VECINT=0;
  EA = 1;
 }  

