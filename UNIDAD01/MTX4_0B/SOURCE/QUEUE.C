/* ============================================================ */
/* File: QUEUE.C						*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Purpose: Library routines for managing queues.		*/
/* See file LIBEPC.H for function descriptions.			*/
/*								*/
/* Designed for use with the DJGPP port of the GNU C/C++	*/
/* protected mode 386 compiler.					*/
/*								*/
/* Modification History:					*/
/*								*/
/* ============================================================ */

#include "libepc.h"

QUEUE *QueueCreate(int item_size, int max_items)
	{
	QUEUE *q ;

	q = (QUEUE *) malloc(sizeof(QUEUE) + item_size * max_items) ;
	if (!q) return NULL ;

	q->item_size	= item_size ;
	q->max_items	= max_items ;
	q->item_count	= 0 ;
	q->nq_index	= 0 ;
	q->dq_index	= 0 ;

	return q ;
	}

BOOL QueueRemove(QUEUE *q, void *item)
	{
	BOOL empty ;

	PUSHF ;
	CLI ;
	empty = q->item_count <= 0 ;
	if (!empty)
		{
		void register *src = q->bfr + q->item_size * q->dq_index++ ;
		memcpy(item, src, q->item_size) ;
		if (q->dq_index == q->max_items) q->dq_index = 0 ;
		q->item_count-- ;
		}
	POPF ;
	return !empty ;
	}

BOOL QueueInsert(QUEUE *q, void *item)
	{
	BOOL full ;

	PUSHF ;
	CLI ;
	full = (q->item_count >= q->max_items) ;
	if (!full)
		{
		void register *dst = q->bfr + q->item_size * q->nq_index++ ;
		memcpy(dst, item, q->item_size) ;
		if (q->nq_index == q->max_items) q->nq_index = 0 ;
		q->item_count++ ;
		}
	POPF ;
	return !full ;
	}

