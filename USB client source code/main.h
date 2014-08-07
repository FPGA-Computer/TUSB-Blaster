#include <stdio.h>
#include <stddef.h>

typedef unsigned char 		ubyte;
typedef unsigned int 			uword;
typedef char 				byte;
typedef int 				word;

typedef __xdata unsigned char 	xubyte;

typedef __bit BOOL;
#define FALSE 0
#define TRUE  1

#include "tusb3410.h"
#include "usb.h"
#include "uart.h"
#include "hardware.h"

// Altera byte blaster
//#define VID			0x16C0  
//#define PID			0x06AD
//#define VERSION 		0x0004

#define LO(X)			((X)&0xff)
#define HI(X)			((X)>>8)
#define VID			0x09fb  
#define PID			0x6001
#define VERSION 		0x0400

#define USB_VER		0x0110

#define MAX_POWER 	50			// need 2*50 mA max
#define USB_ATTR         	0x80           	// Bus powered, not self-powered, no remote wakeup

#define EP0_SIZE		8
#define EP1_SIZE		64
#define EP2_SIZE		64
#define EEPROM_SIZE	128
#define OUTBUFFER_LEN	0x100

#define USB_EP1_IBUF	(0xfef0-EP1_SIZE)            					// 0xFEB0 - 0xFEEF
#define USB_EP2_OBUF	(USB_EP1_IBUF-EP2_SIZE)					// 0xFE70 - 0xFEAF
#define EEPROM_BUF	(USB_EP2_OBUF-EEPROM_SIZE)				// 0xFDF0 - 0xFE6F
#define OUTPUT_BUF	(EEPROM_BUF-OUTBUFFER_LEN)			// 0xFBF0 - 0xFDEF (512 bytes)
														// 0xFCF0 - 0xFDEF (256)	
volatile  xubyte __at USB_EP1_IBUF ep1_in_x[EP1_SIZE];
volatile  xubyte __at USB_EP2_OBUF ep2_out_x[EP2_SIZE];

// FTDI Discriptor EEPROM Emulation 
xubyte __at EEPROM_BUF eeprom[EEPROM_SIZE];

// Output buffer
xubyte __at OUTPUT_BUF OutBuffer[OUTBUFFER_LEN];

// added for USB JTAG 
extern const struct usb_device_descriptor dev_descript;
extern const struct usb_config_descriptor config_descript;
extern const unsigned int ManufacturerDscr[];
extern const unsigned int ProductDscr[] ;
extern const unsigned int SerialNumDscr[];
void usb_wait_for_ep0_in();

#define BYTE ubyte
#define WORD uword

// alias into struct usb_ctrlrequest usb_setup_buffer
volatile  __xdata __at 0xFF00 unsigned char bRequestType;
volatile  __xdata __at 0xFF01 unsigned char bRequest;
volatile  __xdata __at 0xFF02 unsigned char wValueL;
volatile  __xdata __at 0xFF03 unsigned char wValueH;
volatile  __xdata __at 0xFF04 unsigned char wIndexL;
volatile  __xdata __at 0xFF05 unsigned char wIndexH;
volatile  __xdata __at 0xFF06 unsigned char wLengthL;
volatile  __xdata __at 0xFF07 unsigned char wLengthH;

#ifndef Old_Descriptor
void usb_handle_descriptor_request();
#endif

void USB_Handle_Vendor_Request(void);
void USB_Handle_Class_Request(void);
void USB_Handle_Reset(void);

void eeprom_init();

void usb_jtag_init(void);
void usb_jtag_activity(void);
void OutputByte(BYTE d);

#define SYNCDELAY

#define bmBIT6		0x40
#define bmBIT7		0x80
