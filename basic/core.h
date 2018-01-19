#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define USE_MEMPOOL

#ifndef NULL
#define NULL 0
#endif

//宏定义布尔类型
#ifndef BOOL 
typedef int                 BOOL;
#define TRUE 1
#define FALSE 0
#endif
#ifndef UINT
typedef unsigned int        UINT;
#endif

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

typedef struct TxPoint
{
	int x;
	int y;
} TxPoint;
TxPoint txPoint(int x, int y);

typedef struct TxPoint2f
{
	float x;
	float y;
} TxPoint2f;
TxPoint2f txPoint2f(float x, float y);


typedef struct TxPoint3f
{
	float x;
	float y;
	float z;
} TxPoint3f;
TxPoint3f txPoint3f(float x, float y, float z);


typedef struct TxSize
{
	int width;
	int height;
} TxSize;

typedef struct TxRect
{
	int x;
	int y;
	int width;
	int height;
} TxRect;

typedef struct TxRectf
{
	float x;
	float y;
	float width;
	float height;
} TxRectf;

typedef struct TxVec2f
{
	float data[2];
} TxVec2f;

//memory
#ifdef USE_MEMPOOL
#include "cache.h"
#define txAlloc(s)	txCacheAlloc(s)
#define txFree(ptr)	txCacheFree(ptr)
#else
#include "system.h"
#define txAlloc(s)	txSystemAlloc(s)
#define txFree(ptr)	txSystemFree(ptr)
#endif



#ifdef __cplusplus
}
#endif


