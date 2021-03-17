/* ============================================================ */
/* File: HEAP.C							*/
/*								*/
/* Copyright (C) 2001, Daniel W. Lewis and Prentice-Hall	*/
/*								*/
/* Purpose: Library routines for managing heaps.		*/
/* See file LIBEPC.H for function descriptions.			*/
/*								*/
/* Designed for use with the DJGPP port of the GNU C/C++	*/
/* protected mode 386 compiler.					*/
/*								*/
/* Modification History:					*/
/*								*/
/* ============================================================ */

#include "libepc.h"

extern BYTE8 heap_frst		__asm__("heap_frst")	;

/* Set the following defines to 1 if these public functions are desired: */
#define	LARGEST_FREE	0
#define	TOTAL_FREE	0

#define	BEGIN_CRITICAL	PUSHF; CLI
#define	END_CRITICAL	POPF

#define SMALLEST_BLOCK	32

typedef void REGION ;

typedef struct TAG
	{
	char		type ;
	unsigned	size ;
	} __attribute__((packed)) TAG ;

typedef struct FREE
	{
	REGION *	prev ;
	REGION *	next ;
	} __attribute__((packed)) FREE ;

#define	PREV(base)	((FREE *) (base))->prev
#define	NEXT(base)	((FREE *) (base))->next

PRIVATE REGION *	free_list = NULL ;

PRIVATE TAG *		Btm_Tag(REGION *) ;
PRIVATE TAG *		Top_Tag(REGION *) ;
PRIVATE REGION *	Prev_Region(REGION *) ;
PRIVATE REGION *	Next_Region(REGION *) ;
PRIVATE REGION *	Next_Free(REGION *) ;
PRIVATE REGION *	Allocate(REGION *, unsigned) ;
PRIVATE void		Ins_Free(REGION *, REGION *) ;
PRIVATE void		Del_Free(REGION *) ;
PRIVATE void		Combine(REGION *, REGION *) ;
PRIVATE void		Init_Tags(REGION *, unsigned, char) ;
PRIVATE void		Init_Heap(void) ;

/* -------------------------------------------------------------------- */
/* Public functions							*/ 
/* -------------------------------------------------------------------- */
void *malloc(long unsigned int bytes)
	{
	REGION *newblock, *end ;

	Init_Heap() ;

	if (!free_list) return NULL ;

	if (bytes < SMALLEST_BLOCK) bytes = SMALLEST_BLOCK ;

	end = free_list ;
	newblock = NULL ;

	BEGIN_CRITICAL ;
	do  /* First-Fit */
  		{
		if (Btm_Tag(free_list)->size >= bytes)
			{
			newblock = Allocate(free_list, bytes) ;
			}
		else free_list = Next_Free(free_list) ;
		} while (!newblock && free_list != end) ;
	END_CRITICAL ;

	return newblock ;
	}

void free(REGION *newhole)
	{
	register TAG *btm = Btm_Tag(newhole) ;
	register TAG *top = Top_Tag(newhole) ;

	if (btm->type != 'A' || top->size != btm->size) return ;

	BEGIN_CRITICAL ;

	btm->type = 'F';
	top->type = 'F' ;

	if (!free_list)
		{
		free_list = newhole ;
		NEXT(newhole) = newhole ;
		PREV(newhole) = newhole ;
		}
	else
		{
		BOOL changed = FALSE ;
		REGION *adjacent ;

		adjacent = Next_Region(newhole) ;
		if (adjacent != NULL && Btm_Tag(adjacent)->type == 'F')
		     	{
			Combine(newhole, adjacent) ;
			Ins_Free(newhole, adjacent) ;
			Del_Free(adjacent) ;
			changed = TRUE ;
			}

		adjacent = Prev_Region(newhole) ;
		if (adjacent != NULL && Btm_Tag(adjacent)->type == 'F')
		     	{
			Combine(adjacent, newhole) ;
			if (changed) Del_Free(newhole) ;
			changed = TRUE ;
			}

		if (!changed) Ins_Free(newhole, free_list) ;
		}

	END_CRITICAL ;
	}

#if LARGEST_FREE==1
unsigned Largest_Free(void)
	{
	unsigned largest ;
	REGION *end ;

	Init_Heap() ;

	if (!free_list) return 0 ;

	largest = 0 ;
	end = free_list ;

	BEGIN_CRITICAL ;
	do
  		{
		unsigned size = Btm_Tag(free_list)->size ;
		if (size > largest) largest = size ;
		free_list = Next_Free(free_list) ;
		} while (free_list != end) ;
	END_CRITICAL ;

	return largest ;
	}
#endif

#if	TOTAL_FREE==1
unsigned Total_Free(void)
	{
	unsigned total ;
	REGION *end ;

	Init_Heap() ;

	if (!free_list) return 0 ;

	total = 0 ;
	end = free_list ;

	BEGIN_CRITICAL ;
	do
  		{
		total += Btm_Tag(free_list)->size ;
		free_list = Next_Free(free_list) ;
		} while (free_list != end) ;
	END_CRITICAL ;

	return total ;
	}
#endif

/* -------------------------------------------------------------------- */
/* Private functions							*/ 
/* -------------------------------------------------------------------- */
PRIVATE TAG *Btm_Tag(REGION *base)
	{
	return (TAG *) ((BYTE8 *) base - sizeof(TAG)) ;
	}

PRIVATE TAG *Top_Tag(REGION *base)
	{
	return (TAG *) ((BYTE8 *) base + Btm_Tag(base)->size) ;
	}

PRIVATE void Init_Tags(REGION *r, unsigned bytes, char type)
	{
	register TAG *tag ;

	tag = Btm_Tag(r) ;
	tag->type = type ;
	tag->size = bytes ;

	tag = Top_Tag(r) ;
	tag->type = type ;
	tag->size = bytes ;
	}

PRIVATE REGION *Prev_Region(REGION *base)
	{
	register TAG *prev = Btm_Tag(base) - 1 ;
	if (prev->type == 'B') return NULL ;
	return (REGION *) ((BYTE8 *) prev - prev->size) ;
  	}

PRIVATE REGION *Next_Region(REGION *base)
	{
	register TAG *next = Top_Tag(base) + 1 ;
	if (next->type == 'T') return NULL ;
	return (REGION *) ((BYTE8 *) next + sizeof(TAG)) ;
	}

PRIVATE REGION *Next_Free(REGION *base)
	{
	return base ? NEXT(base) : NULL ;
	}

PRIVATE REGION *Allocate(REGION *hole, unsigned bytes)
	{
	unsigned hole_size = Btm_Tag(hole)->size ;

	if ((hole_size - bytes) <= SMALLEST_BLOCK)
		{
		Init_Tags(hole, hole_size, 'A') ;
		Del_Free(hole) ;
		return hole ;
		}

	free_list = hole + bytes + 2 * sizeof(TAG) ;
	hole_size -= bytes + 2 * sizeof(TAG) ;
	Init_Tags(free_list, hole_size, 'F') ;
	Ins_Free(free_list, hole) ;

	Init_Tags(hole, bytes, 'A') ;
	Del_Free(hole) ;
	return hole ;
	}

PRIVATE void Combine(REGION *a, REGION *b)
	{
	unsigned size = Btm_Tag(a)->size + Btm_Tag(b)->size + 2*sizeof(TAG) ;
	Btm_Tag(a)->size = size ;
	Top_Tag(a)->size = size ;
	}

PRIVATE void Ins_Free(REGION *this, REGION *prev)
	{
	register REGION *next = NEXT(prev) ;

	PREV(this) = prev ;
	NEXT(this) = next ;

	PREV(next) = this ;
	NEXT(prev) = this ;
	}

PRIVATE void Del_Free(REGION *this)
	{
	REGION *prev = PREV(this) ;
	REGION *next = NEXT(this) ;

	if (prev == next && prev == this)
		{
		free_list = NULL ; /* Only one in the list */
		}
	else
		{
		if (free_list == this) free_list = prev ;
		NEXT(prev) = next ;
		PREV(next) = prev ;
		}

	NEXT(this) = PREV(this) = NULL ;
	}

PRIVATE void Init_Heap(void)
	{
#	define	EXT_MEM_START	((BYTE8 *) 0x100000)
#	define	RSVD_MEM_START	((BYTE8 *) 0x0A0000)
#	define	RSVD_MEM_SIZE	(EXT_MEM_START - RSVD_MEM_START)
	static BOOL initialized = FALSE ;
	BYTE8 *top = (BYTE8 *) LastMemoryAddress() + 1 - sizeof(TAG) ;
	BYTE8 *btm = &heap_frst + sizeof(TAG) ;
	unsigned bytes ;
	REGION *r ;

	if (initialized) return ;

	free_list = NULL ;

	/* Sanity check - Is there anything left to use? */
	bytes = SMALLEST_BLOCK + 2 * sizeof(TAG) ;
	if (RSVD_MEM_START <= btm + bytes) btm = EXT_MEM_START + sizeof(TAG) ;
	if (top < btm + bytes) return ;

	/* Mark top and bottom of entire heap */
	((TAG *) btm - 1)->type = 'B' ;
	((TAG *) top)->type = 'T' ;

	/* Create initial free block (ignore reserved memory) */
	r = (REGION *) (btm + sizeof(TAG)) ;
	Init_Tags(r, (top - btm) - 2 * sizeof(TAG), 'A') ;

	/* Cut a hole for reserved memory if necessary */
	if (btm < RSVD_MEM_START && top >= EXT_MEM_START)
		{
		bytes = (RSVD_MEM_START - (BYTE8 *) r) - 2 * sizeof(TAG) ;
		Init_Tags(r, bytes, 'A') ;
		Init_Tags(RSVD_MEM_START, RSVD_MEM_SIZE, 'A') ;
		r = (REGION *) (EXT_MEM_START + 2 * sizeof(TAG)) ;
		bytes = (top - (BYTE8 *) r) - sizeof(TAG) ;
		Init_Tags(r, bytes, 'A') ;
		free(r) ;
		}

	free(btm + sizeof(TAG)) ;

	initialized = TRUE ;
	}

