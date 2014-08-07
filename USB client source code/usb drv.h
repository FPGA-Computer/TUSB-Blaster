#ifndef __USB_DRV_H
#define __USB_DRV_H

// USB Endpoint status
#define EP_IDLE			0x00
#define EP_DATA_IN			0x01
#define EP_DATA_OUT		0x02
#define EP_OVERSIZE		0x04
#define EP_ZEROPACK		0x08
#define EP_IN_DONE			0x10
#define EP_OUT_DONE		0x20

// USB Buffer Address
#define BUF_ADDR(X)     (((X) >> 3) & 0xFF)

// keep track of data transfer across interrupts
extern uword status;
extern ubyte EP0_Status;
extern uword IEP0_PacketRemaining, OEP0_PacketRemaining;
extern ubyte *IEP0_Buffer, *OEP0_Buffer;

#define EP0_MAXSIZE	8

/* The setup and EP0 buffers are in fixed locations in RAM */
volatile  __xdata __at 0xff00 struct usb_ctrlrequest usb_setup_buffer;
volatile  __xdata __at 0xfef8 ubyte usb_ep0in_buffer[EP0_MAXSIZE];
volatile  __xdata __at 0xfef0 ubyte usb_ep0out_buffer[EP0_MAXSIZE];

void USB_Init(void);
extern void USB_Interrupt(void) __interrupt 0;
void usb_write_ep0_buffer(ubyte *Buffer, uword Size);
void usb_Read_ep0_buffer(ubyte *Buffer, uword Size);
void EP0_Stall(void);
void usb_ack_ep0_out(void);
void usb_ack_ep0_in(void);

struct usb_descriptor_entry
{
  ubyte type;
  ubyte index;
  void  *buffer;
  int   length;
  uword language;               /* 0 for "don't care" */
 };

extern const struct usb_descriptor_entry usb_descriptors[];

#endif /* __USB_DRV_H */