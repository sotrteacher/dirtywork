/*----------------------------------------------------------------------------
 *-------------------------      M a R T E   O S      ------------------------
 *----------------------------------------------------------------------------
 *                                                             V2.0 2017-02-22
 *
 *             'H a r d w a r e   I n t e r r u p t   C o n t r o l'
 *
 *                                      H
 *
 * File 'intr.h'                                                      By MAR.
 *
 * Basic hardware interrupts management for C applications. Based
 * on the POSIX draft for "Interrupt Control API" (P1003.2X/D1.0,
 * February 2001).
 *
 * ----------------------------------------------------------------------
 *  Copyright (C) 2000-2017, Universidad de Cantabria, SPAIN
 *
 *  MaRTE OS web page: http://marte.unican.es
 *  Contact Addresses: Mario Aldea Rivas          aldeam@unican.es
 *                     Michael Gonzalez Harbour      mgh@unican.es
 *
 * MaRTE OS  is free software; you can  redistribute it and/or  modify it
 * under the terms of the GNU General Public License  as published by the
 * Free Software Foundation;  either  version 2, or (at  your option) any
 * later version.
 *
 * MaRTE OS  is distributed  in the  hope  that  it will be   useful, but
 * WITHOUT  ANY  WARRANTY;     without  even the   implied   warranty  of
 * MERCHANTABILITY  or  FITNESS FOR A  PARTICULAR PURPOSE.    See the GNU
 * General Public License for more details.
 *
 * You should have received  a  copy of  the  GNU General Public  License
 * distributed with MaRTE  OS;  see file COPYING.   If not,  write to the
 * Free Software  Foundation,  59 Temple Place  -  Suite 330,  Boston, MA
 * 02111-1307, USA.
 *
 * As a  special exception, if you  link this  unit  with other  files to
 * produce an   executable,   this unit  does  not  by  itself cause  the
 * resulting executable to be covered by the  GNU General Public License.
 * This exception does  not however invalidate  any other reasons why the
 * executable file might be covered by the GNU Public License.
 *
 *---------------------------------------------------------------------------*/

#ifndef	__INTR_H__
#define	__INTR_H__ 1
#include <time.h>

/* ***************************** *
 * Source of hardware interrupts *
 * ***************************** */
typedef int intr_t;

/*  PC standard hardware interrupts */
#define TIMER_HWINTERRUPT        0 // Master PIC
#define KEYBOARD_HWINTERRUPT     1 // Master PIC
#define CTLR2_HWINTERRUPT        2 // Master PIC
#define SERIAL2_HWINTERRUPT      3 // Master PIC
#define SERIAL1_HWINTERRUPT      4 // Master PIC
#define PARALLEL2_HWINTERRUPT    5 // Master PIC
#define DISKETTE_HWINTERRUPT     6 // Master PIC
#define PARALLEL1_HWINTERRUPT    7 // Master PIC
#define RTC_HWINTERRUPT          8 // Slave PIC
#define SOFT_HWINTERRUPT         9 // Slave PIC
#define RESERVED1_HWINTERRUPT   10 // Slave PIC
#define RESERVED2_HWINTERRUPT   11 // Slave PIC
#define RESERVED3_HWINTERRUPT   12 // Slave PIC
#define COPROCESSOR_HWINTERRUPT 13 // Slave PIC
#define FIXED_DISK_HWINTERRUPT  14 // Slave PIC
#define RESERVED4_HWINTERRUPT   15 // Slave PIC

/* ******************************* *
 * Interrupt handlers return codes *
 * ******************************* */
#define POSIX_INTR_HANDLED_NOTIFY         _MARTE_POSIX_INTR_HANDLED_NOTIFY
// The interrupt handler handled this interrupt, and the thread that
// registered the handler should be notified that the interrupt
// occurred.
#define POSIX_INTR_HANDLED_DO_NOT_NOTIFY _MARTE_POSIX_INTR_HANDLED_DO_NOT_NOTIFY
// The interrupt handler handled this interrupt, but the thread that
// registered the handler should NOT be notified that the interrupt
// occurred.
#define POSIX_INTR_NOT_HANDLED            _MARTE_POSIX_INTR_NOT_HANDLED
// The interrupt handler did not handle this interrupt; if there are
// other handlers connected to this interrupt, then the next handler
// should be called.


/* ************************************************************ *
 * Associate a user-written interrupt handler with an interrupt *
 * ************************************************************ */
int posix_intr_associate (intr_t intr,
			  int (*intr_handler) (void * area, intr_t intr),
			  volatile void * area,
			  size_t areasize);

int posix_intr_disassociate (intr_t intr,
			     int (*intr_handler) (void * area, intr_t intr));


/* ************************** *
 * Lock and unlock interrupts *
 * ************************** */
int posix_intr_lock (intr_t intr);
int posix_intr_unlock (intr_t intr);


/* **************************** *
 * Await interrupt notification *
 * **************************** */
int posix_intr_timedwait (int flags,
			  const struct timespec *abs_timeout,
			  intr_t *intr,
			  int (**intr_handler) (void * area, intr_t intr) );

#endif	/* __INTR_H__ */
