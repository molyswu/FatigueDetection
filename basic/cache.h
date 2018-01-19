#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#define SIZETYPE_COUNT  30

//����ڴ�
typedef struct TxMemPool
{
	unsigned int totalSize;
	unsigned int freePos;
	void* lists[SIZETYPE_COUNT];
}TxMemPool;

//���ڴ������/�ͷ��ڴ�
TxMemPool*txCreateMemPool(unsigned int totalSize);
void txReleaseMemPool(TxMemPool**);
void* txPoolAlloc(TxMemPool*pool,unsigned int size);
void txPoolFree(TxMemPool*pool,void *ptr);

//��ȫ���ڴ�ط����ͷ��ڴ�
void *txCacheAlloc(unsigned int size);
void txCacheFree(void *ptr);
void txCleanCache(void);

#ifdef __cplusplus
}
#endif
