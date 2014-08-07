/*-----------------------------------------------------------------------------
 * FTDI EEPROM emulation
 *-----------------------------------------------------------------------------
 * Copyright (C) 2007 Kolja Waschk, ixo.de
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
   modified by K. C. Lee based on http://sa89a.net/mp.cgi/ele/ub.htm
*/

#include "main.h"

void eeprom_init(void)
{ char j, sofs, i;
  char *ptr;
  unsigned short checksum;

  eeprom[0]= 0x00;
  eeprom[1]= 0x00;
  eeprom[2]= LO(dev_descript.idVendor);
  eeprom[3]= HI(dev_descript.idVendor);
  eeprom[4]= LO(dev_descript.idProduct);
  eeprom[5]= HI(dev_descript.idProduct);
  eeprom[6]= LO(dev_descript.bcdDevice);
  eeprom[7]= HI(dev_descript.bcdDevice);
  eeprom[8]= config_descript.bmAttributes;
  eeprom[9] = config_descript.MaxPower;
  eeprom[10] = 0x1C;
  eeprom[11] = 0x00;
  eeprom[12] = LO(dev_descript.bcdUSB);
  eeprom[13] = HI(dev_descript.bcdUSB);
  eeprom[14] = sofs = 0x86+14;
  
  sofs += (eeprom[15] = LO(ManufacturerDscr[0]));
  eeprom[16]= sofs;
  
  sofs += (eeprom[17]=ProductDscr[0]);
  eeprom[18]= sofs;
  
  eeprom[19]= LO(SerialNumDscr[0]);
  i=20;

  for(j=0, ptr = (char*) &(ManufacturerDscr);j<LO(ManufacturerDscr[0]);j++) 
    eeprom[i++]=ptr[j];

  for(j=0, ptr = (char*) &(ProductDscr);j<LO(ProductDscr[0]);j++) 
    eeprom[i++]=ptr[j];

  for(j=0, ptr = (char*) &(SerialNumDscr);j<LO(SerialNumDscr[0]);j++) 
    eeprom[i++]=ptr[j];

  for(j=0, ptr = (char*) & dev_descript.iManufacturer;j<4;j++)
    eeprom[i++]=ptr[j]; 

  while(i<126)
    eeprom[i++]=0;

  checksum = 0xAAAA;

  for(i=0;i<63;i++)
  { checksum^=((word*)eeprom)[i];
   
    if(checksum&0x8000)
      checksum=(checksum<<1)|1;
    else 
      checksum=(checksum<<1);
   }
    
   ((word*)eeprom)[63]=checksum;
 }
