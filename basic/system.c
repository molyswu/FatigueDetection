#include "system.h"

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

//memory
void* txSystemAlloc(unsigned int size)
{
	return malloc(size);
}
void txSystemFree(void* ptr)
{
	if(ptr)
	{
		free(ptr);
	}
}

//return current time in millisecond
double txGetTickCount()
{
	double t;
#if defined WIN32 || defined _WIN32 || defined WINCE
	LARGE_INTEGER counter;
	LARGE_INTEGER freq;
	QueryPerformanceCounter( &counter );
	QueryPerformanceFrequency(&freq);

	t = 1000*(double)counter.QuadPart/(double)freq.QuadPart;
#else

	struct timeval tv;
	struct timezone tz;
	gettimeofday( &tv, &tz );
	t = (int64_t)tv.tv_sec*1000 + tv.tv_usec/1000;
#endif
	return t;
}

#ifndef ANDROID_MODE
//event
TXHANDLE txCreateEvent(const TXCHAR* lpName)
{
#if defined WIN32 || defined _WIN32 || defined WINCE
	return ::CreateEvent(NULL, FALSE, FALSE, (LPCWSTR)lpName);
#else
	return NULL;
#endif
}
int txSetEvent(void* hEvent)
{
#if defined WIN32 || defined _WIN32 || defined WINCE
	return ::SetEvent(hEvent);
#else
	return NULL;
#endif
}
int txResetEvent(TXHANDLE hEvent)
{
#if defined WIN32 || defined _WIN32 || defined WINCE
	return ::ResetEvent(hEvent);
#else
	return NULL;
#endif
}
int txCloseEvent(TXHANDLE hEvent)
{
#if defined WIN32 || defined _WIN32 || defined WINCE
	return CloseHandle(hEvent);
#else
	return NULL;
#endif
}

#endif

//mutex
TXMUTEXHANDLE txCreateMutex(const TXCHAR* lpName)
{
#if defined WIN32 || defined _WIN32 || defined WINCE
	CRITICAL_SECTION * cs = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	// Initialize the critical section one time only.
	if (!InitializeCriticalSectionAndSpinCount(cs, 
		0x00000400) ) 
	{
		free(cs);
		cs = 0;
	}
	return cs;
#elif defined ANDROID_MODE
	pthread_mutex_t *m = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(m, NULL);
	return m;
#else
	AmbaKAL_MutexCreate(mutex);
	return mutex;
#endif
}

void txLock(TXMUTEXHANDLE hMutex)
{
#if defined WIN32 || defined _WIN32 || defined WINCE
	EnterCriticalSection((LPCRITICAL_SECTION)hMutex);
#elif defined ANDROID_MODE
	pthread_mutex_lock (hMutex);
#else
	AmbaKAL_MutexTake(&hMutex, AMBA_KAL_WAIT_FOREVER);
#endif
}

void txUnLock(TXMUTEXHANDLE hMutex)
{
#if defined WIN32 || defined _WIN32 || defined WINCE
	LeaveCriticalSection((LPCRITICAL_SECTION)hMutex);
#elif defined ANDROID_MODE
	pthread_mutex_unlock (hMutex);
#else
	AmbaKAL_MutexGive(&hMutex);
#endif
}

int txCloseMutex(TXMUTEXHANDLE hMutex)
{
#if defined WIN32 || defined _WIN32 || defined WINCE
	DeleteCriticalSection((LPCRITICAL_SECTION)hMutex);
#elif defined ANDROID_MODE
	pthread_mutex_destroy(hMutex);
#else
	AmbaKAL_MutexDelete(&hMutex);
#endif
	return 1;
}

//thread
#if defined WIN32 || defined _WIN32 || defined WINCE
TXHANDLE txBeginThread(unsigned (__stdcall * _StartAddress) (void *),void*arglist,int priority, int auto_start)
{
	unsigned int uid,init_flag = auto_start ? 0 : CREATE_SUSPENDED;

	TXHANDLE hThread = (TXHANDLE)_beginthreadex(NULL,0,_StartAddress,arglist,init_flag,&uid);

//	SetThreadPriority(hThread,priority);

	return hThread;

}


int txResumeThread(TXHANDLE hThread)
{
	return ResumeThread(hThread);
}

int txTerminateThread(TXHANDLE hThread)
{
	return TerminateThread(hThread,0);
}
int txCloseHandle(TXHANDLE hObject)
{
	return CloseHandle(hObject);
}

int txWaitForSingleObject(TXHANDLE hobj,long dwMilliseconds)
{
	return WaitForSingleObject(hobj,dwMilliseconds);
}

void txSleep(int ms)
{
	Sleep(ms);
}
#endif


#ifndef ANDROID_MODE

//创建消息队列
int g_MsgQuequeCount = 0;
void txMsgQueueInit(TX_ADAS_MSG_QUEUE*q)
{
	char str[128];
	q->count = 0;
	q->head = 0;
	q->tail =0;
	sprintf(str,"MsgQueueMutex_%d",g_MsgQuequeCount);
	q->hMutex = txCreateMutex((const TXCHAR*)str);

	sprintf(str,"MsgQueueEvent_%d",g_MsgQuequeCount);
	q->hEvent = txCreateEvent((const TXCHAR*)str);
	g_MsgQuequeCount++;
}

//释放
void txMsgQueueRelease(TX_ADAS_MSG_QUEUE*q)
{
	txCloseMutex(q->hMutex);
	txCloseEvent(q->hEvent);
}

//发送消息
BOOL txSendMsg(TX_ADAS_MSG_QUEUE*q,TX_ADAS_MSG msg)
{

	BOOL res;
	txLock(q->hMutex);
	if (q->count<MAX_MSG_COUNT)
	{
		q->msg[q->tail] = msg;
		q->tail = (q->tail+1) % MAX_MSG_COUNT;
		q->count++;
		res = TRUE;

		//发送事件
		txSetEvent(q->hEvent);
	}
	else
	{
		res = FALSE;
	}

	txUnLock(q->hMutex);

	return res;

}

//接收消息，没有消息的话会阻塞
BOOL txReceiveMsg(TX_ADAS_MSG_QUEUE*q,TX_ADAS_MSG* msg)
{
	BOOL res;
	txWaitForSingleObject(q->hEvent,INFINITE);
	txLock(q->hMutex);
	if (q->count > 0)
	{
		*msg = q->msg[q->head];
		q->head = (q->head+1) % MAX_MSG_COUNT;
		q->count--;
		res = TRUE;
	}
	else
	{
		res = FALSE;
	}
	txUnLock(q->hMutex);
	return res;
}

//播放声音
void txPlaySound(int id)
{

}

//设置灯光状态
void txSetLight(int id, int state)
{

}

#endif

