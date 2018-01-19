#include "cache.h"
#include "system.h"
#include <stdlib.h>
#include "string.h"

#define  DEFAULT_TOTAL_SIZE	1024*1024*100 -1024

//任意大小的内存结点，管理用户内存的申请和回收
typedef struct MNode
{
	unsigned int buffersize;
	struct MNode* next;
} MNode;

int chooseCache(unsigned int size)
{
	unsigned int i,buffersize;
	for (i = 0; i < SIZETYPE_COUNT; ++i)
	{
		buffersize = 1 << (i + 3);
		if (size <= buffersize)
		{
			return i;
		}
	}

	return -1;
}


TxMemPool*txCreateMemPool(unsigned int totalSize)
{
	TxMemPool * pool = (TxMemPool*)txSystemAlloc(sizeof(TxMemPool) + totalSize);
	if(pool)
	{
		memset(pool,0,sizeof(TxMemPool));
		pool->totalSize = totalSize;
	}
	return pool;

}
void txReleaseMemPool(TxMemPool**pp)
{
	TxMemPool * pool;
	if (!pp)
	{
		return;
	}
	pool = *pp;
	if (pool)
	{
		txSystemFree(pool);
	}
	*pp = NULL;


}

void* private_malloc(TxMemPool*pool, unsigned int size)
{
	void *p = NULL;
	if (pool->freePos + size <= pool->totalSize)
	{
		p = (char*)pool + sizeof(TxMemPool) + pool->freePos;
		pool->freePos += size;
	}
	else
	{
		return NULL;
	}
	return p;
}

MNode* private_createNode(TxMemPool*pool,unsigned int buf_size)
{
	MNode* node = (MNode*)private_malloc(pool,sizeof(MNode) + buf_size);
	if (node)
	{
		node->buffersize = buf_size;
		node->next = NULL;
	}
	else
	{
		return NULL;
	}
	return node;
}

void* txPoolAlloc(TxMemPool*pool,unsigned int size)
{
	void *p = NULL;
	int i, buffer_size;
	MNode* node = NULL;
	
	i = chooseCache(size);
	buffer_size = 1 << (i + 3);
	node = (MNode*)pool->lists[i];

	if (node)
	{
		pool->lists[i] = node->next;
		node->next = NULL;
	}
	else
	{
		node = private_createNode(pool,buffer_size);
	}

	if (node)
	{
		p = (char*)node + sizeof(MNode);
	}
	return p;

}
void txPoolFree(TxMemPool*pool,void *ptr)
{
	int i;
	MNode* node = (MNode*)((char*)ptr - sizeof(MNode));

	i = chooseCache(node->buffersize);
	if (i >= 0)
	{
		node->next = (MNode*)pool->lists[i];
		pool->lists[i]= node;
	}

}

TxMemPool* g_pool = NULL;
TXMUTEXHANDLE g_cacheMutex;

void cache_init(void)
{
	g_cacheMutex = txCreateMutex((const TXCHAR*)"g_cacheMutex");
	txLock(g_cacheMutex);
	g_pool = txCreateMemPool(DEFAULT_TOTAL_SIZE);
	txUnLock(g_cacheMutex);
}

void *txCacheAlloc(unsigned int size)
{
	void* p = NULL;
	if (g_pool == NULL)
	{
		cache_init();
	}
	txLock(g_cacheMutex);
	if (g_pool)
	{
		p = txPoolAlloc(g_pool,size);
	}
	txUnLock(g_cacheMutex);

	return p;

}
void txCacheFree(void *ptr)
{
	txLock(g_cacheMutex);
	if (g_pool)
	{
		txPoolFree(g_pool,ptr);
	}
	txUnLock(g_cacheMutex);

}

void txCleanCache()
{
	txLock(g_cacheMutex);
	txReleaseMemPool(&g_pool);
	txUnLock(g_cacheMutex);
	txCloseMutex(g_cacheMutex);
}

