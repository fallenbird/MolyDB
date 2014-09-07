
#ifndef _JK_ALLOCATOR_I7TBE9C_H_
#define _JK_ALLOCATOR_I7TBE9C_H_

#include "JK_Lock.h"

#define ALLOCATION_QUANTUM 4 // Changes will require changes in the structs below

typedef struct 
{
	unsigned int size;
	JK_Lock lock;
} SegmentHeader;

typedef struct {
	unsigned int size;                 // physical size of block
	unsigned int allocated;            // allocated size; 0 means free
	void*  next;
} BlockHeader;



#define SPLIT_LIMIT        16			// Create no block smaller than this
static __inline unsigned int ALLOCSIZE(unsigned int size)
{
	return (size + (ALLOCATION_QUANTUM - 1)) & ~(ALLOCATION_QUANTUM - 1);
} 


typedef struct 
{
	void* block;				// block in pool
	unsigned int size;			// size of block requested by user
	unsigned int allocated;		// 0 if free, # of bytes in use if allocated
} MyHeapInfo, *PMyHeapInfo;


class JK_ALLOCATOR
{
public:
	JK_ALLOCATOR()
	{
	}

	~JK_ALLOCATOR()
	{

	}

	void* CreateMemPool(unsigned int size)
	{
		if(size == 0)
		{
			return NULL; 
		}

		void* mem = malloc( size );
		if(mem == NULL)
		{
			return NULL;
		}

		SegmentHeader * seghdr = (SegmentHeader*)mem;
		seghdr->size = size; // size specified for creation

		BlockHeader * blockhdr = (BlockHeader*)&seghdr[1];
		blockhdr->size = size - (sizeof(BlockHeader) + sizeof(SegmentHeader));
		blockhdr->allocated = 0; // none of this block is allocated
		blockhdr->next = NULL;
		return mem;
	}


	void*	AllocFromPool( void* pool, unsigned int size)
	{
		if(size == 0)
		{
			return NULL; // zero allocation returns NULL
		}
		if( NULL == pool )
		{
			return NULL;
		}

		unsigned int AllocSize = ALLOCSIZE(size);
		SegmentHeader * seghdr = (SegmentHeader *)pool;
		BlockHeader * blockhdr = (BlockHeader *)&seghdr[1];

		void* result = NULL;

		&seghdr->lock.Lock();
		while( true )
		{ 
			//scan for first fit 
			if(blockhdr == NULL)
			{
				break;
			}
			if(blockhdr->allocated != 0)
			{ 
				// allocated, skip it
				blockhdr = (BlockHeader *)blockhdr->next;
				continue;
			} 

			// It is not allocated, is it big enough?
			// Note that the size of a free block is already rounded up to
			// the allocation quantum

			if(blockhdr->size >= AllocSize)
			{ 

				unsigned int delta = blockhdr->size - AllocSize;
				//
				// if delta is 0, we have an exact fit
				if(delta == 0)
				{
					// perfect match
					result = (void*)&blockhdr[1];
					blockhdr->allocated = size;
					break;
				}

				// if delta is too small to hold a header + SPLIT_LIMIT bytes of useful space, do not split
				if(delta < sizeof(BlockHeader) + SPLIT_LIMIT)
				{ 
					result = (void*)&blockhdr[1];
					blockhdr->allocated = size;
					break;
				} 


				BlockHeader * newhdr = (BlockHeader *) (((BYTE *)blockhdr) + AllocSize + sizeof(BlockHeader));
				newhdr->size = delta - sizeof(BlockHeader);
				newhdr->allocated = 0;  // no space is allocated in this new block
				newhdr->next = blockhdr->next;
				blockhdr->next = newhdr;
				blockhdr->allocated = size;
				blockhdr->size = AllocSize;
				result = (void*)&blockhdr[1];

				break;
			}
			else
			{ 
				// Skip this block if it is too small
				blockhdr = (BlockHeader *)blockhdr->next;
			}
		} 

		&seghdr->lock.Unlock();
		return result;
	}



	void   FreeMemPool( void* pool, void* mem )
	{
		if(NULL == pool )
		{
			return;
		}
		if( NULL == mem )
		{
			return;
		}
		SegmentHeader * seghdr = (SegmentHeader *)pool;
		
		&seghdr->lock.Lock();
		BlockHeader * blockhdr = (BlockHeader *) ((BYTE*)mem - sizeof(BlockHeader));
		blockhdr->allocated = 0;
		&seghdr->lock.Unlock();
	} 



	void   CoalesceMemPool(void* pool)
	{
		if(NULL == pool)
		{
			return;
		}
		SegmentHeader * seghdr = (SegmentHeader *)pool;

		&seghdr->lock.Lock();
		BlockHeader * blockhdr = (BlockHeader *)&seghdr[1];
		while(TRUE)
		{ 
			// scan pool
			if(blockhdr == NULL)
			{
				break;
			}
			if(blockhdr->next == NULL)
			{
				break;
			}

			BlockHeader * nexthdr = (BlockHeader *)blockhdr->next;

			if(blockhdr->allocated != 0)
			{
				// allocatd, skip it
				blockhdr = nexthdr;
				continue;
			} 
			// It is a free header.  Coalesce it with the next block
			// if the next block is free
			if(nexthdr->allocated != 0)
			{ 
				// next allocated, skip it 
				blockhdr = (BlockHeader *)nexthdr->next;
				continue;
			}

			// Coalesce the two
			blockhdr->size = blockhdr->size + nexthdr->size + sizeof(BlockHeader);
			blockhdr->next = nexthdr->next;
		} 
		&seghdr->lock.Unlock();
	}




	void   DestroyMemPool( void* pool)
	{
		if(NULL == pool)
		{
			return;
		}
		SegmentHeader * seghdr = (SegmentHeader *)pool;
		free(pool);
		//::VirtualFree(pool, seghdr->size, MEM_RELEASE);
	}


	unsigned int GetPoolSize(void* pool)
	{
		SegmentHeader * seghdr = (SegmentHeader *)pool;
		return seghdr->size;
	} 

private:
	bool	m_bThread;


	BOOL GetMyBlockInfo( void* pool, void* addr, PMyHeapInfo info)
	{
		if( NULL == pool )
		{
			return FALSE;
		}
		if(addr == NULL)
		{
			return FALSE;
		}

		SegmentHeader * seghdr = (SegmentHeader *)pool;

		&seghdr->lock.Lock();
		BlockHeader * blockhdr = (BlockHeader *)((LPBYTE)addr - sizeof(BlockHeader));
		info->block = addr;
		info->size = blockhdr->size;
		info->allocated = blockhdr->allocated;
		&seghdr->lock.Unlock();

		return TRUE;
	} // GetMyBlockInfo
	



	bool WalkMyHeap( void* pool, bool (CDECL * fn)(PMyHeapInfo info, LPARAM p), LPARAM userdata)
	{
		if( NULL == pool )
		{
			return FALSE;
		}

		SegmentHeader * seghdr = (SegmentHeader *)pool;

		&seghdr->lock.Lock();
		BlockHeader * blockhdr;
		blockhdr = (BlockHeader *)&seghdr[1];
		while(TRUE)
		{ /* scan blocks */
			MyHeapInfo info;

			if(blockhdr == NULL)
			{
				break;
			}
			info.block = (void*)&blockhdr[1];
			info.allocated = blockhdr->allocated;
			info.size = blockhdr->size;
			if(!fn(&info, userdata))
			{
				break;
			}

			blockhdr = (BlockHeader *)blockhdr->next;
		}
		&seghdr->lock.Unlock();
		return TRUE;
	}


};




#endif
