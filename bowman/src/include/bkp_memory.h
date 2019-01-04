#ifndef __BKP_MEMORY_H_
#define __BKP_MEMORY_H_

/*********************************************************************
 * Defines 
*********************************************************************/

/*********************************************************************
 * Type def & enum
*********************************************************************/

/*********************************************************************
 * Macro 
*********************************************************************/

/*********************************************************************
 * Struct 
*********************************************************************/
struct mheader 
{
	size_t size;
	int npool;
	struct mheader * next;

}mheader;

//____________________________________________________________________
typedef struct BKP_MemoryPool 
{
	struct mheader * head;
	size_t size;
	size_t count;
	size_t N;
	int nalloc;
}BKP_MemoryPool;

//____________________________________________________________________
typedef struct BKP_MemoryPools
{
	BKP_MemoryPool **pools;

	ssize_t bytes;
	int tickets_realloc;
	int pools_count;
	int pools_gap;
	int max_realloc;

}BKP_MemoryPools;

//____________________________________________________________________

/*********************************************************************
 * Global 
*********************************************************************/

BKP_MemoryPools BKPmPools;

/*********************************************************************
 * Struct 
*********************************************************************/

/*********************************************************************
 * Functions 
*********************************************************************/

void bkp_memory_setDefaultTicketsAllocs(int n);
void bkp_memory_setDefaultPoolsGap(int n);
void bkp_memory_setDefaultPoolsNumber(int n);

int bkp_memory_getDefaultTicketsAllocs(void);
int bkp_memory_getDefaultPoolsGap(void);
int bkp_memory_getDefaultPoolsNumber(void);

int bkp_memory_init(int max_realloc); /* max_rellact=-1 for infinite */
void bkp_memory_destroyPools(void);
void bkp_memory_clearPools(void);
void bkp_memory_clearPool(BKP_MemoryPool *mp);
void * bkp_memory_getTicket(ssize_t size);
void  bkp_memory_releaseTicket(void * ticket);
void print_pool(void * ticket);

ssize_t bkp_memory_using(void);
ssize_t bkp_memory_usage(void);
ssize_t bkp_memory_free(void);
ssize_t bkp_memory_allocated(void);

#endif

