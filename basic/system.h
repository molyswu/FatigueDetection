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
//priority:ȡֵ��Χ[-2,2],0��ʾnormal,����Խ�����ȼ�Խ��
TXHANDLE txBeginThread(unsigned (__stdcall * _StartAddress) (void *),void*arglist,int priority, int auto_start);
int txResumeThread(TXHANDLE hThread);
int txTerminateThread(TXHANDLE hThread);
int txCloseHandle(TXHANDLE hObject);

int txWaitForSingleObject(TXHANDLE hobj,long dwMilliseconds);
void txSleep(int ms);
#endif


#ifndef ANDROID_MODE

#define MAX_MSG_COUNT 256

//�Զ�����Ϣ����
typedef struct _TX_ADAS_MSG
{
	int id;
	int para;
}TX_ADAS_MSG;

//��Ϣ���У��̰߳�ȫ
typedef struct _TX_ADAS_MSG_QUEUE
{
	int count;	//��Ϣ����
	int head;	//��ͷ
	int tail;	//��β
	TX_ADAS_MSG msg[MAX_MSG_COUNT];

	TXHANDLE hMutex;
	TXHANDLE hEvent;

}TX_ADAS_MSG_QUEUE;

//��ʼ����Ϣ����
void txMsgQueueInit(TX_ADAS_MSG_QUEUE*q);
//�ͷ�
void txMsgQueueRelease(TX_ADAS_MSG_QUEUE*q);
//������Ϣ
BOOL txSendMsg(TX_ADAS_MSG_QUEUE*q,TX_ADAS_MSG msg);
//������Ϣ��û����Ϣ�Ļ�������
BOOL txReceiveMsg(TX_ADAS_MSG_QUEUE*q,TX_ADAS_MSG* msg);

//��������
void txPlaySound(int id);

//���õƹ�״̬,state:0�رգ�1������2��˸
void txSetLight(int id, int state);

#endif

#ifdef __cplusplus
}
#endif
