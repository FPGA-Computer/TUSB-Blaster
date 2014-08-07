#include "main.h"

const struct usb_device_descriptor dev_descript = 
{
   sizeof(dev_descript),				//    BYTE		bLength;
   USB_DT_DEVICE,					//    BYTE		bDescriptorType;
   0x0110,						//    WORD	bcdUsb;
   USB_CLASS_PER_INTERFACE,				//    BYTE    	bDeviceClass;
   0,							//    BYTE		bDeviceSubClass;
   0,							//    BYTE		bDeviceProtocol;
   EP0_SIZE,						//    BYTE		bMaxPacketSize0;
   VID,							//    WORD	wIdVendor;
   PID,							//    WORD	wIdProduct;
   VERSION,						//    WORD	wBcdDevice;
   1,							//    BYTE		wManufacturer;
   2,							//    BYTE		wProduct;
   3,							//    BYTE		wSerialNumber;
   1,							//    BYTE		bNumConfigurations;
 };

// The following has to be contiguous
// ----------------------------------------------
const struct usb_config_descriptor config_descript =
  /* Configuration */
{
    sizeof(config_descript),				// BYTE    bLength;
    USB_DT_CONFIG,					// BYTE    bDescriptorType;
    sizeof(config_descript)+				// WORD	wTotalLength;
    sizeof(struct usb_interface_descriptor)+
    sizeof(struct usb_endpoint_descriptor)*2,    
    1,							// BYTE	bNumInterfaces;
    1,							// BYTE	bConfigurationValue;
    0,							// BYTE	bConfiguration;
    USB_ATTR, //CFG_DESC_ATTR_BUS_POWERED,		// BYTE	bAttributes;
    MAX_POWER						// BYTE    bMaxPower; (*2mA)
   };

const struct usb_interface_descriptor if0_descript =   
{										
     sizeof(struct usb_interface_descriptor),		// BYTE    bLength;
     USB_DT_INTERFACE, 					// BYTE    bDescriptorType;
     0,							// BYTE    bInterfaceNumber;
     0,							// BYTE    bAlternateSetting;
     2,							// BYTE    bNumEndpoints;
      USB_CLASS_VENDOR_SPEC,				// BYTE    bInterfaceClass;
      USB_CLASS_VENDOR_SPEC,				// BYTE    bInterfaceSubClass;
      USB_CLASS_VENDOR_SPEC,				// BYTE    bInterfaceProtocol;
     0							// BYTE    bInterface;
 };

const struct usb_endpoint_descriptor if0_ep1_in_descript =	// EP1 In :Bulk 64
{										
      sizeof(struct usb_endpoint_descriptor),		// BYTE	bLength;
       USB_DT_ENDPOINT,					// BYTE	bDescriptorType;
       1 | USB_DIR_IN,					// BYTE	bEndpointAddress;
       USB_ENDPOINT_XFER_BULK,				// BYTE	bAttributes;
       EP1_SIZE,					// WORD	wMaxPacketSize;
       0						// BYTE	bInterval;	
  };
      
const struct usb_endpoint_descriptor if0_ep2_out_descript =	// EP2 Out :Bulk 64
{										
       sizeof(struct usb_endpoint_descriptor),		// BYTE	bLength;
       USB_DT_ENDPOINT,					// BYTE	bDescriptorType;
       2 | USB_DIR_OUT,					// BYTE	bEndpointAddress;
       USB_ENDPOINT_XFER_BULK,				// BYTE	bAttributes;
       EP2_SIZE,					// WORD	wMaxPacketSize;
       0						// BYTE	bInterval;	
  };
  
// ----------------------------------------------
const unsigned int LangTable[]=
{ (sizeof(LangTable)&0xff) | (USB_DT_STRING<<8), 
   0x0409 /* English */ 
  };
 
const unsigned int ManufacturerDscr[] = 
{ (sizeof(ManufacturerDscr)&0xff) | (USB_DT_STRING<<8),
   'A','l','t','e','r','a'
 };

const unsigned int ProductDscr[] = 
{ (sizeof(ProductDscr)&0xff) | (USB_DT_STRING<<8),
   'U','S','B','-','B','l','a','s','t','e','r'
 };

const unsigned int SerialNumDscr[] = 
{ (sizeof(SerialNumDscr)&0xff) | (USB_DT_STRING<<8),
   '0','0','0','0','0','0','0','0'
 };

const static unsigned char EndDescr[] = { 0 };
// ----------------------------------------------

void usb_handle_descriptor_request()
{ const char *usb_desc;
   unsigned char index = usb_setup_buffer.wValue >>8;

  if(index == USB_DT_DEVICE) 
    usb_write_ep0_buffer((unsigned char *)&dev_descript,sizeof(dev_descript));
  else if  (index == USB_DT_CONFIG)
    usb_write_ep0_buffer((unsigned char *)&config_descript,config_descript.wTotalLength);
  else if  (index == USB_DT_STRING)
  {  for(index=0, usb_desc = (unsigned char *) LangTable; usb_desc[0]; index++, usb_desc +=  usb_desc[0])
         if ((usb_setup_buffer.wValue &0xff)==index)
         { usb_write_ep0_buffer(usb_desc, usb_desc[0]);
            break;
	  }
    }  
 }

/* The End */
