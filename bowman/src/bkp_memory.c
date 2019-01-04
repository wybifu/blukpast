#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "include/macro.h"
#include "include/bkp_logger.h"
#include "include/bkp_memory.h"

/*Copyright*/
/*
 * AUTHOR: lilington
 * addr	: lilington@yahoo.fr
 *
 */

/**************************************************************************
*	Defines & Maro
**************************************************************************/

/**************************************************************************
*	Structs, Enum, Unio and Typesdef	
**************************************************************************/

/**************************************************************************
*	Globals	
**************************************************************************/

static int stc_tickets_alloc_count = 128;
static int stc_pools_gap 			= 64;
static int stc_pools_count 		= 1024;

BKP_MemoryPools * BKPmPool = NULL;

/***************************************************************************
*	Prototypes 	
**************************************************************************/

static int _growPool(int index);

/**************************************************************************
*	Main	
**************************************************************************/

int *x;
//------------- Main fucntion ---------------
/**************************************************************************
*	Implementations	
**************************************************************************/

/*--------------------------------------------------------------------------------*/
void bkp_memory_setDefaultTicketsAllocs(int n) 	{ stc_tickets_alloc_count = n;}
void bkp_memory_setDefaultPoolsGap(int n) 		{ stc_pools_gap = n;}
void bkp_memory_setDefaultPoolsNumber(int n) 	{ stc_pools_count = n;}

int bkp_memory_getDefaultTicketsAllocs() 	{return stc_tickets_alloc_count;}
int bkp_memory_getDefaultPoolsGap() 		{return stc_pools_gap ;}
int bkp_memory_getDefaultPoolsNumber() 	{return stc_pools_count ;}

/*--------------------------------------------------------------------------------*/
int bkp_memory_init(int max_realloc)
{
	BKPmPool = (BKP_MemoryPools *)malloc(sizeof(BKP_MemoryPools));

	if(BKPmPool == NULL)
	{
		bkp_logger_write(BKP_LOGGER_ERROR,"Memory Malloc Error in %s : %d\n",__func__,__LINE__);
		return -1;
	}

	BKPmPool->tickets_realloc 	= stc_tickets_alloc_count;
	BKPmPool->pools_count		= stc_pools_count;
	BKPmPool->pools_gap			= stc_pools_gap;
	BKPmPool->max_realloc		= max_realloc;
	BKPmPool->bytes = sizeof(BKP_MemoryPools);

	BKPmPool->pools = (BKP_MemoryPool **)malloc(sizeof(BKP_MemoryPool *) * BKPmPool->pools_count);

	BKPmPool->bytes += sizeof(BKP_MemoryPool *) * BKPmPool->pools_count;

	if(BKPmPool->pools == NULL)
	{
		bkp_logger_write(BKP_LOGGER_ERROR,"Memory Malloc Error in %s : %d\n",__func__,__LINE__);
		return -1;
	}

	for(int i = 0; i < BKPmPool->pools_count; ++i)
	{
		BKPmPool->pools[i] 	= (BKP_MemoryPool *)malloc(sizeof(BKP_MemoryPool));
		if(BKPmPool->pools[i] == NULL)
		{
			bkp_logger_write(BKP_LOGGER_ERROR,"Memory Malloc Error in %s : %d\n",__func__,__LINE__);
			return -1;
		}
		BKPmPool->pools[i]->head	= NULL;
		BKPmPool->pools[i]->count	= 0; 
		BKPmPool->pools[i]->size	= (i+1) * BKPmPool->pools_gap; 
		BKPmPool->pools[i]->N		= 0; 
		BKPmPool->pools[i]->nalloc	= 0; 

		BKPmPool->bytes += sizeof(BKP_MemoryPool);
	}

	return 0;
}

/*--------------------------------------------------------------------------------*/
void bkp_memory_destroyPools()
{
	int i;

	bkp_memory_clearPools();

	for(i = 0; i < BKPmPool->pools_count; ++i)
	{
		BKPmPool->bytes -= sizeof(BKP_MemoryPool);
		free(BKPmPool->pools[i]);
	}

	BKPmPool->bytes -= sizeof(BKP_MemoryPool *) * BKPmPool->pools_count;
	BKPmPool->bytes -= sizeof(BKP_MemoryPools);
	free(BKPmPool->pools);
	free(BKPmPool);
		
	return;
}

/*--------------------------------------------------------------------------------*/
void bkp_memory_clearPools()
{
	for(int i = 0; i < BKPmPool->pools_count; ++i)
	{
		if(BKPmPool->pools[i]->count < BKPmPool->pools[i]->N)
			bkp_logger_write(BKP_LOGGER_WARNING,"Ticket for Memory pool are not all released, **MEMORY LEAK** P(%d) s:%ld\n",
					i,BKPmPool->pools[i]->size);
		bkp_memory_clearPool(BKPmPool->pools[i]);
	}
}

/*--------------------------------------------------------------------------------*/
void bkp_memory_clearPool(BKP_MemoryPool *mp)
{
	struct mheader* p = mp->head;

	while(p != NULL)
	{
		struct mheader * q = p;
		p = p->next;
		free(q);
	}
	mp->head = NULL;
	mp->count = 0;
	mp->N	  = 0; 

	return;
}

/*--------------------------------------------------------------------------------*/
void * bkp_memory_getTicket(ssize_t msize)
{
	ssize_t size = msize + sizeof(struct mheader);
	int index = size  / BKPmPool->pools_gap;
	struct mheader * pt = NULL;

	//fprintf(stderr,"%ld in pool index %d \n", size, index);
	if(BKPmPool->pools[index]->head == NULL)
	{
		//fprintf(stderr,"from pool %d of size : %d from object : %ld -> pool empty\n",index, (index + 1) * BKPmPool->pools_gap, size);
		if(0 == _growPool(index))
			return NULL;

		pt = BKPmPool->pools[index]->head;
		if(pt != NULL)
		{
			BKPmPool->pools[index]->count -= 1;
			BKPmPool->pools[index]->head = pt->next;
			pt +=1;
		}
	}
	else
	{
		pt = BKPmPool->pools[index]->head;
		BKPmPool->pools[index]->count -= 1;
		BKPmPool->pools[index]->head = pt->next;
		pt +=1;
	}

	return (void *)pt;
}

/*--------------------------------------------------------------------------------*/
void  bkp_memory_releaseTicket(void * ticket)
{
	struct mheader* p = (struct mheader*) ticket;
	p -= 1;

	int index = p->npool;
	//fprintf(stderr,"size to release : %ld, from pool :%d\n",p->size,p->npool);

	p->next = BKPmPool->pools[index]->head;
	BKPmPool->pools[index]->head = p;
	BKPmPool->pools[index]->count += 1;
	return;
}

/*--------------------------------------------------------------------------------*/
void print_pool(void * ticket)
{
	struct mheader* p = (struct mheader*) ticket;
	p -= 1;
	int i = p->npool;
	p = BKPmPool->pools[i]->head;
	fprintf(stdout,"-----------Pool %d----------\n\tcount:%ld\n\ts_elt:%ld\n\tmax:%ld\n",
			i,BKPmPool->pools[i]->count, BKPmPool->pools[i]->size, BKPmPool->pools[i]->N);
	fprintf(stdout,"[%p]",p);
	if(p != NULL)
	{
		do
		{
			p = p->next;
			fprintf(stdout,"->[%p]",p);
		}
		while(p != NULL);
	}
	fprintf(stdout,"\n--------------------------------------\n\n");

	return;
}

/*--------------------------------------------------------------------------------*/
ssize_t bkp_memory_using(void)
{
	return BKPmPool->bytes;
}
/*--------------------------------------------------------------------------------*/
ssize_t bkp_memory_usage(void)
{
    ssize_t s = 0;
    for(int i = 0; i < BKPmPool->pools_count; ++i)
        s += (BKPmPool->pools[i]->N - BKPmPool->pools[i]->count) * BKPmPool->pools[i]->size;

    return s;
}

/*--------------------------------------------------------------------------------*/
ssize_t bkp_memory_free(void)
{
    ssize_t s = 0;
    for(int i = 0; i < BKPmPool->pools_count; ++i)
        s += BKPmPool->pools[i]->count * BKPmPool->pools[i]->size;

    return s;
}

/*--------------------------------------------------------------------------------*/
ssize_t bkp_memory_allocated(void)
{
    ssize_t s = 0;
    for(int i = 0; i < BKPmPool->pools_count; ++i)
        s += BKPmPool->pools[i]->N * BKPmPool->pools[i]->size;

    return s;
}
/*--------------------------------------------------------------------------------*/


/******************** static functions ********************************/

/*--------------------------------------------------------------------------------*/
static int _growPool(int index)
{
	size_t size = (index + 1) * BKPmPool->pools_gap;
	BKP_MemoryPool * pool = BKPmPool->pools[index];
	
	if(BKPmPool->max_realloc > 0 && pool->nalloc >= BKPmPool->max_realloc)
	{
		bkp_logger_write(BKP_LOGGER_INFO,"Max refilling pool reached\n");
		return 0;
	}


	int i;
	for(i = 0; i < BKPmPool->tickets_realloc; ++i)
	{
		struct mheader * pt = (struct mheader *)malloc(size);
		if(pt == NULL)
		{
			bkp_logger_write(BKP_LOGGER_INFO,"Memory Malloc Error");
			return 0;
//			BKP_DIE_L("Memory Malloc Error",-1);
		}

		pt->size	= size;
		pt->npool = index;
		pt->next = pool->head;
		pool->head 	= pt;
		pool->count	+= 1;
	}

	pool->N		+= BKPmPool->tickets_realloc;
	++pool->nalloc ;

	BKPmPool->bytes += size * BKPmPool->tickets_realloc;

	return i;
}

/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------*/

