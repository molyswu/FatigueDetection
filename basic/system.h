#pragma once
#ifdef __cplusplus
extern "C" {
#endif

//#define AMBA_MODE
//#define ANDROID_MODE

#include "stdio.h"
#if defined WIN32 || defined _WIN32 || defined WINCE
#include <process.h>
#include "Windows.h"
#include <time.h> 
#elif defined ANDROID_MODE
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#else
#include "AmbaDataType.h"
#include <applib.h>
#include "AmbaSysTimer.h"
#endif

#ifdef _UNICODE
	typedef unsigned short TXCHAR;
#else
	typedef char TXCHAR;
#endif 
#define INFINITE            0xFFFFFFFF  // Infinite timeout


#if defined WIN32 || defined _WIN32 || defined WINCE
	//typedef void *TXENENTHANDLER
	typedef void *TXMUTEXHANDLE;
	typedef void *TXHANDLE;

#elif defined ANDROID_MODE
	//typedef event_flag *TXENENTHANDLER
	typedef pthread_mutex_t	*TXMUTEXHANDLE;
	typedef void *TXHANDLE;

#else
	//typedef UINT32 TXENENTHANDLER
	//AMBA_KAL_EVENT_FLAG_t event_Flag1;
	//UINT32 LaneEventData = 0;
	//UINT32 trackingEventData = 0;
	//UINT32 carEventData = 0;
	//UINT32 faceEventData = 0;

	typedef AMBA_KAL_MUTEX_t TXMUTEXHANDLE;	
	TXMUTEXHANDLE mutex;

#endif


#ifdef AMBA_MODE
#define CARTIME 0
#define TRACKINGTIME 0
#define LANETIME 0
#define FACETIME 0
#define PRINT 0
#define AMBA_MODE
#define DEBUG 0
#endif

//memory
void* txSystemAlloc(unsigned int size);
void txSystemFree(void*ptr);

//return current time in millisecond
double txGetTickCount();


// the following functions are not implemented well enough now in android
#ifndef ANDROID_MODE
//event
TXHANDLE txCreateEvent(const TXCHAR* lpName);
int txSetEvent(void* hEvent);
int txResetEvent(TXHANDLE hEvent);
int txCloseEvent(TXHANDLE hEvent);
#endif

//mutex
TXMUTEXHANDLE txCreateMutex(const TXCHAR* lpName);
void txLock(TXMUTEXHANDLE hMutex);
void txUnLock(TXMUTEXHANDLE hMutex);
int txCloseMutex(TXMUTEXHANDLE hMutex);



//thread
#if defined WIN32 || defined _WIN32 || defined WINCE
//priority:取值范围[-2,2],0表示normal,数字越大，优先级越高
TXHANDLE txBeginThread(unsigned (__stdcall * _StartAddress) (void *),void*arglist,int priority, int auto_start);
int txResumeThread(TXHANDLE hThread);
int txTerminateThread(TXHANDLE hThread);
int txCloseHandle(TXHANDLE hObject);

int txWaitForSingleObject(TXHANDLE hobj,long dwMilliseconds);
void txSleep(int ms);
#endif


#ifndef ANDROID_MODE

#define MAX_MSG_COUNT 256

//自定义消息队列
typedef struct _TX_ADAS_MSG
{
	int id;
	int para;
}TX_ADAS_MSG;

//消息队列，线程安全
typedef struct _TX_ADAS_MSG_QUEUE
{
	int count;	//消息个数
	int head;	//队头
	int tail;	//队尾
	TX_ADAS_MSG msg[MAX_MSG_COUNT];

	TXHANDLE hMutex;
	TXHANDLE hEvent;

}TX_ADAS_MSG_QUEUE;

//初始化消息队列
void txMsgQueueInit(TX_ADAS_MSG_QUEUE*q);
//释放
void txMsgQueueRelease(TX_ADAS_MSG_QUEUE*q);
//发送消息
BOOL txSendMsg(TX_ADAS_MSG_QUEUE*q,TX_ADAS_MSG msg);
//接收消息，没有消息的话会阻塞
BOOL txReceiveMsg(TX_ADAS_MSG_QUEUE*q,TX_ADAS_MSG* msg);

//播放声音
void txPlaySound(int id);

//设置灯光状态,state:0关闭，1常亮，2闪烁
void txSetLight(int id, int state);

#endif

#ifdef __cplusplus
}
#endif
