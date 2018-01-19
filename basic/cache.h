#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#define SIZETYPE_COUNT  30

//大块内存
typedef struct TxMemPool
{
	unsigned int totalSize;
	unsigned int freePos;
	void* lists[SIZETYPE_COUNT];
}TxMemPool;

//从内存池申请/释放内存
TxMemPool*txCreateMemPool(unsigned int totalSize);
void txReleaseMemPool(TxMemPool**);
void* txPoolAlloc(TxMemPool*pool,unsigned int size);
void txPoolFree(TxMemPool*pool,void *ptr);

//从全局内存池分配释放内存
void *txCacheAlloc(unsigned int size);
void txCacheFree(void *ptr);
void txCleanCache(void);

#ifdef __cplusplus
}
#endif
