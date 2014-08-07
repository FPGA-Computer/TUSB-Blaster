#include "tusb.h"

/* supplement to tusb.h */

/* Memory mapped USB control registers */
/*Output 1 Endpoint Descriptor Block*/
__xdata __at 0xFF08 unsigned char OEPCNF_1;		/*Output endpoint 1 configuration*/
__xdata __at 0xFF09 unsigned char OEPBBAX_1;		/*Output endpoint 1 X buffer base address*/
__xdata __at 0xFF0A unsigned char OEPBCTX_1;		/*Output endpoint 1 X byte count*/
__xdata __at 0xFF0D unsigned char OEPBBAY_1;		/*Output endpoint 1 Y buffer base address*/
__xdata __at 0xFF0E unsigned char OEPBCTY_1;		/*Output endpoint 1 Y byte count*/
__xdata __at 0xFF0F unsigned char OEPSIZXY_1;		/*Output endpoint 1 X/Y buffer size*/

/*Output 2 Endpoint Descriptor Block*/
__xdata __at 0xFF10 unsigned char OEPCNF_2;		/*Output endpoint 2 configuration*/
__xdata __at 0xFF11 unsigned char OEPBBAX_2;		/*Output endpoint 2 X buffer base address*/
__xdata __at 0xFF12 unsigned char OEPBCTX_2;		/*Output endpoint 2 X byte count*/
__xdata __at 0xFF15 unsigned char OEPBBAY_2;		/*Output endpoint 2 Y buffer base address*/
__xdata __at 0xFF16 unsigned char OEPBCTY_2;		/*Output endpoint 2 Y byte count*/
__xdata __at 0xFF17 unsigned char OEPSIZXY_2;		/*Output endpoint 2 X/Y buffer size*/

/*Output 3 Endpoint Descriptor Block*/
__xdata __at 0xFF18 unsigned char OEPCNF_3;		/*Output endpoint 2 configuration*/
__xdata __at 0xFF19 unsigned char OEPBBAX_3;		/*Output endpoint 2 X buffer base address*/
__xdata __at 0xFF1A unsigned char OEPBCTX_3;		/*Output endpoint 2 X byte count*/
__xdata __at 0xFF1D unsigned char OEPBBAY_3;		/*Output endpoint 2 Y buffer base address*/
__xdata __at 0xFF1E unsigned char OEPBCTY_3;		/*Output endpoint 2 Y byte count*/
__xdata __at 0xFF1F unsigned char OEPSIZXY_3;		/*Output endpoint 2 X/Y buffer size*/

/*Input 1 Endpoint Descriptor Block*/
__xdata __at 0xFF48 unsigned char IEPCNF_1;		/*Output endpoint 1 configuration*/
__xdata __at 0xFF49 unsigned char IEPBBAX_1;		/*Output endpoint 1 X buffer base address*/
__xdata __at 0xFF4A unsigned char IEPBCTX_1;		/*Output endpoint 1 X byte count*/
__xdata __at 0xFF4D unsigned char IEPBBAY_1;		/*Output endpoint 1 Y buffer base address*/
__xdata __at 0xFF4E unsigned char IEPBCTY_1;		/*Output endpoint 1 Y byte count*/
__xdata __at 0xFF4F unsigned char IEPSIZXY_1;		/*Output endpoint 1 X/Y buffer size*/

/*Input 2 Endpoint Descriptor Block*/
__xdata __at 0xFF50 unsigned char IEPCNF_2;		/*Output endpoint 2 configuration*/
__xdata __at 0xFF51 unsigned char IEPBBAX_2;		/*Output endpoint 2 X buffer base address*/
__xdata __at 0xFF52 unsigned char IEPBCTX_2;		/*Output endpoint 2 X byte count*/
__xdata __at 0xFF55 unsigned char IEPBBAY_2;		/*Output endpoint 2 Y buffer base address*/
__xdata __at 0xFF56 unsigned char IEPBCTY_2;		/*Output endpoint 2 Y byte count*/
__xdata __at 0xFF57 unsigned char IEPSIZXY_2;		/*Output endpoint 2 X/Y buffer size*/

/*Input 3 Endpoint Descriptor Block*/
__xdata __at 0xFF58 unsigned char IEPCNF_3;		/*Output endpoint 3 configuration*/
__xdata __at 0xFF59 unsigned char IEPBBAX_3;		/*Output endpoint 3 X buffer base address*/
__xdata __at 0xFF5A unsigned char IEPBCTX_3;		/*Output endpoint 3 X byte count*/
__xdata __at 0xFF5D unsigned char IEPBBAY_3;		/*Output endpoint 3 Y buffer base address*/
__xdata __at 0xFF5E unsigned char IEPBCTY_3;		/*Output endpoint 3 Y byte count*/
__xdata __at 0xFF5F unsigned char IEPSIZXY_3;		/*Output endpoint 3 X/Y buffer size*/

/* Interrupt numbers: address = (number * 8) + 3 */
#define EX0_VECTOR		0       /* 0x03 external interrupt 0 */
#define ET0_VECTOR	1       /* 0x0b timer 0 */
#define EX1_VECTOR		2       /* 0x13 external interrupt 1 */
#define ET1_VECTOR	3       /* 0x1b timer 1 */
#define ES_VECTOR		4       /* 0x23 serial port 0 */

// TUSB INT0 Interrupts - KCL

#define USB_IRQ_OEND_1  	0x12
#define USB_IRQ_OEND_2  	0x14
#define USB_IRQ_OEND_3  	0x16
#define USB_IRQ_IEND_1  	0x22
#define USB_IRQ_IEND_2  	0x24
#define USB_IRQ_IEND_3  	0x26
#define USB_IRQ_STPOW   	0x30
#define USB_IRQ_SETUP   	0x32
#define USB_IRQ_RESR    		0x38
#define USB_IRQ_SUSR    		0x3a
#define USB_IRQ_RSTR    		0x3c
#define USB_IRQ_WAKEUP	0x3e
#define I2C_IRQ_TXE     		0x40
#define I2C_IRQ_RXF     		0x42
#define USB_IRQ_IEND_0  	0x44
#define USB_IRQ_OEND_0  	0x46
#define UART_STAUS		0x50
#define UART_MODEM		0x52
#define UART_RXF			0x60
#define UART_TXE			0x62
#define DMA1				0x80
#define DMA3				0x84

#define IEPBCNT_0_SIZEMASK   0x0f

// Timer 2
 __sfr __at 0xc8 T2CON;
 __sfr __at 0xc9	T2MOD;
 __sfr __at 0xca	RCAP2L;
 __sfr __at 0xcb	RCAP2H;
 __sfr __at 0xcc	TL2;
 __sfr __at 0xcd	TH2;

 __sbit __at 0xcf TF2;
 __sbit __at 0xce EXF2;
 __sbit __at 0xcd RCLK;
 __sbit __at 0xcc TCLK;
 __sbit __at 0xcb EXEN2;
 __sbit __at 0xca TR2;
 __sbit __at 0xc9 C_T2;
 __sbit __at 0xc8 CP_RL2;
 
 