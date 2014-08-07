TUSB_DIR = .\\

SDCC = "C:\Program Files\SDCC\bin\sdcc"
PACKIHX = "C:\Program Files\SDCC\bin\packihx"
MAKEBIN = makebin2

CFLAGS = 
LDFLAGS	= --xram-loc 0xF800 --xram-size 200

OBJS = usb.rel uart.rel

%.rel : %.c
	$(SDCC) -o $@ -c $< $(CFLAGS)

tusb_blaster : tusb_blaster.c USB_descript.rel eeprom.rel hardware.rel $(OBJS)
	$(SDCC) $(CFLAGS) $(LDFLAGS) $^
	$(PACKIHX) $@.ihx >$@.hex
	$(MAKEBIN) $@.hex $@.bin
        
all: tusb_blaster
	
clean:	
	del *.hex *.ihx *.lst *.map *.rel *.rst *.sym *.mem *.omf *.lk *.cdb
	

