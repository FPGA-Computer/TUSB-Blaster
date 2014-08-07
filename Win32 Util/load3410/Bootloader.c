#include "bootloader.h"
#include "usb.h"
#include "bootcode.h"
#include <windows.h>
#include <WinNT.h>
#include <stdio.h>

struct usb_device *findboard(int VendorID, int Device_ID, int DeviceClass)
{
  struct usb_bus *busses;
  struct usb_bus *bus;
  struct usb_device *dev;

  /* Find the device */
  usb_init();
  usb_find_busses();
  usb_find_devices();
  busses = usb_get_busses();

  for (bus=busses; bus; bus=bus->next)
    for (dev=bus->devices; dev; dev=dev->next)
      if ((dev->descriptor.idVendor == VendorID) &&
          (dev->descriptor.idProduct == Device_ID) && 
          (dev->descriptor.bDeviceClass == DeviceClass))
        return(dev);

   return(NULL);
 }

int bootusb(struct usb_device *dev, char *file)
{ FILE *BootFile;
  usb_dev_handle *Dev_Handle;
  char BootImage[BOOT_SIZE];
  unsigned int filesize, i;
  int Error=1;

  BootFile = fopen(file,"rb");
    
  if(BootFile)
  {
    filesize=fread(&BootImage[HEADER_SIZE],1,BOOT_SIZE-HEADER_SIZE,BootFile);
    printf("Downloaded %d bytes\n",filesize);   

    if(filesize && !ferror(BootFile))
    { Dev_Handle=usb_open(dev);

      if(Dev_Handle && !usb_claim_interface(Dev_Handle, 0))
      { 

        // Generate header
        BootImage[0] = filesize & 0xff;
        BootImage[1] = (filesize >>8);
        BootImage[2] = 0;

	// Generate checksum
        for(i=0;i<filesize;i++)
          BootImage[2] += BootImage[i+HEADER_SIZE];

        filesize += HEADER_SIZE;
        Error=0;

        for(i=0; i<filesize; i+= PACKET_SIZE)
        {
          if (usb_bulk_write(Dev_Handle,1, &BootImage[i],(filesize-i>PACKET_SIZE)?PACKET_SIZE:filesize-i,USB_TIMEOUT) <0)
          { printf("Error downloading firmware at offset = %d\n",i);
	    Error=1;
	    break;
	   }
	 }

        usb_release_interface(Dev_Handle,0);
       }
      usb_close(Dev_Handle);
     }

    fclose(BootFile);
   };
  return(Error);
 }

int main(int argc, char **argv)
{ struct usb_device *Device;

  if(argc!=2)
  { printf("TUSB3410 bootloader v0.0\nUsage: %s <file>\n",argv[0]);
    exit(1);
   }

  Device=findboard(TI_Vendor_ID,TI_TUSB3410_ID,TI_Unconfig_Class);

  if (Device)
    bootusb(Device,argv[1]);
  else
    printf("No unconfigured TUSB3410 found.\n");
 }

/* The End */
