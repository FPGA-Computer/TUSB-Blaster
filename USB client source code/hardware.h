/* JTAG TCK, AS/PS DCLK */
#define TDO		P3_0
#define TMS		P3_1
#define TDI		P3_3
#define TCK		P3_4

#define _TDO		_P3_0
#define _TMS		_P3_1
#define _TDI		_P3_3
#define _TCK		_P3_4

#define SWCLK		P3_4
#define SWDIO		P3_1

#define TRST		MCR_DTR
#define SRST		MCR_RTS

#define CPU_OSC	48000000L

void ProgIO_Init(void);
void ProgIO_Poll(void);
// These aren't called anywhere in usbjtag.c, but I plan to do so...
void ProgIO_Enable(void);
void ProgIO_Disable(void);
void ProgIO_Deinit(void);

  /* Set state of output pins:
   *
   * d.0 => TCK
   * d.1 => TMS
   * d.2 => nCE (only #ifdef HAVE_AS_MODE)
   * d.3 => nCS (only #ifdef HAVE_AS_MODE)
   * d.4 => TDI
   * d.5 => LED / Output Enable
   */ 

#define bm_TCK	0x01
#define bm_TMS	0x02
#define bm_TDI	0x10

#define bm_TRST	0x04
#define bm_SRST	0x08
   
void ProgIO_Set_State(unsigned char d);
unsigned char ProgIO_Set_Get_State(unsigned char d);
void ProgIO_ShiftOut(unsigned char c);
unsigned char ProgIO_ShiftInOut_JTAG(unsigned char c);

#define  ProgIO_ShiftInOut(x) ProgIO_ShiftInOut_JTAG(x)

void  Multibyte_ProgIO(word m, xubyte *pointer1);

