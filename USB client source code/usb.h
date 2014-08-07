/*
 * usb_driver.h - A relatively easy to use C interface to the USB
 *                hardware on the TUSB3210/TUSB3410 chips. This module
 *                implements many standard commands defined in the USB
 *                spec, and allows vendor-specific commands to be
 *                defined externally.
 *
 * Copyright (C) 2004-2005 Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */
#ifndef __USB_DRIVER_H
#define __USB_DRIVER_H

#include "usb def.h"
#include "usb drv.h"

/* Main entry points */
void usb_init();
void usb_poll();

/* USB idle handler */
extern void (*usb_idle_handler)(void);

/* Application-defined */
void USB_Handle_Vendor_Request(void);


/* Setup and polling of DMA transfers to/from EP1 through EP3.
 * The 'ep' parameter must be an EDB_* constant from tusb.h
 * specifying an endpoint index.
 *
 * Buffer addresses must be in external RAM, aligned on an
 * 8-byte boundary.
 *
 * usb_dma_status returns 0 if nothing has happened, nonzero
 * if a transfer has completed. For OUT endpoints, this will
 * always be the number of bytes received.
 *
 * usb_dma_setup needs to be called to start or continue a transfer.
 * usb_dma_unstall should only be called before the first usb_dma_setup
 * normally.
 */
void usb_dma_setup(ubyte ep,  xubyte *buffer, ubyte buffer_size);
void usb_dma_stall(ubyte ep);
void usb_dma_unstall(ubyte ep);
int  usb_dma_status(ubyte ep);

#endif /* __USB_DRIVER_H */

/* The End */
