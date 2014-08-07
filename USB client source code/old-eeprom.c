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

#include "main.h"

static unsigned char ee_ptr;
static unsigned short ee_cksum;

void eeprom_append(unsigned char nb)
{
  unsigned char pree_ptr = ee_ptr & ~1;
  if(pree_ptr != ee_ptr)
  {
    ee_cksum = ee_cksum ^((unsigned short)nb << 8);
    ee_cksum = ee_cksum ^ eeprom[pree_ptr];
    ee_cksum = (ee_cksum << 1) | (ee_cksum >> 15);
  };
  eeprom[ee_ptr++] = nb;
}

void eeprom_init(void)
{ char j, sofs;
   char *ptr;

  ee_ptr = 0;
  ee_cksum = 0xAAAA;

  eeprom_append(0x00);
  eeprom_append(0x00);

  // unsigned short idVendor;
  // unsigned short idProduct;
  // unsigned short bcdDevice;

  for(j=0, ptr = (char*) &(dev_descript.idVendor);j<6;j++) 
     eeprom_append(ptr[j]);
  
//  unsigned char  bmAttributes;
//  unsigned char  MaxPower;
    
  for(j=0, ptr = (char *) & (config_descript.bmAttributes);j<2;j++) 
     eeprom_append(ptr[j]);

  eeprom_append(0x1C);
  eeprom_append(0x00);

// WORD	bcdUsb;
  for(j=0,ptr=(char *) & dev_descript.bcdDevice  ;j<2;j++) 
     eeprom_append(ptr[j]);
     
  sofs = 0x80 + ee_ptr + 6;
  eeprom_append(sofs);
  
  // sizeof(ManufacturerDscr)&0xff
  eeprom_append(((char *) ManufacturerDscr)[0]);
  sofs += ((char *) ManufacturerDscr)[0];
  eeprom_append(sofs);
  
  // sizeof(ProductDscr)&0xff
  eeprom_append(((char*)ProductDscr)[0]);
  sofs += ((char*)ProductDscr)[0];
  eeprom_append(sofs);
  
  // sizeof(SerialNumDscr)&0xff
  eeprom_append(((char*)SerialNumDscr)[0]);
 
  // unsigned int ManufacturerDscr[] 
  for(j=0,ptr=(char*) ManufacturerDscr;j<ptr[0];j++) 
     eeprom_append(ptr[j]);

  // unsigned int ProductDscr[]     
  for(j=0, ptr=(char*) ProductDscr;j<ptr[0];j++) 
     eeprom_append(ptr[j]);

  // unsigned int SerialNumDscr[]     
  for(j=0,ptr=(char*) SerialNumDscr;j<ptr[0];j++)
    eeprom_append(ptr[j]);
    
//    BYTE		wManufacturer;
//    BYTE		wProduct;
//    BYTE		wSerialNumber;
//    BYTE		bNumConfigurations;
    
  for(j=0,ptr = (char *)&dev_descript.iManufacturer;j<4;j++) 
     eeprom_append(ptr[j]);
     
  while(ee_ptr < 126) eeprom_append(0);
  eeprom[126] = ee_cksum&0xFF;
  eeprom[127] = (ee_cksum>>8)&0xFF;
 }
