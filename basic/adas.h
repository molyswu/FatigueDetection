#pragma once

#include "../basic/core.h"

#ifdef __cplusplus
extern "C" {
#endif

//����
typedef enum _VehicleType
{
	VehicleType_Car = 0,
	VehicleType_Medium = 1,
	VehicleType_Large = 2
}VehicleType;

//����ƫ��Ԥ��״̬
typedef enum _LDW_State
{
	LDW_Normal = 0,
	LDW_OffsetToLeft = 1,
	LDW_OffsetToRight = 2,
	LDW_CrossLeft = 3,
	LDW_CrossRight = 4
}LDW_State;

//����Ԥ��״̬
typedef enum _FCW_State
{
	FCW_Normal = 0,
	FCW_Careful = 1,
	FCW_Dangerous = 2
}FCW_State;

//ǰ��������
typedef enum _StopGo_State
{		
	StopGo_Normal = 0,		//����		
	StopGo_Remind = 1		//����
}StopGo_State;

//����Ԥ��״̬
typedef enum _PCW_State
{
	PCW_Normal = 0,
	PCW_Careful = 1,
	PCW_Dangerous = 2
}PCW_State;

//ƣ�ͼ�ʻԤ��״̬
typedef enum _DFW_State
{
	DFW_Unknown	= 0,			
	DFW_Normal = 1,				//����		
	DFW_Fatigue_Medium = 2,		//�ж�ƣ�ͻ�ע����������
	DFW_Fatigue_Serious = 3		//����ƣ�ͻ����ط���

}DFW_State;

//ͷ����̬
typedef enum _Head_Posture
{
	Head_Unknown = 0,
	Head_Normal = 1,
	Head_TurnLeft = 2,
	Head_TurnRight = 3,
	Head_Tilt = 4
	
}Head_Posture;

//������״̬
typedef enum _Eye_State
{
	EyeState_UnKnown = 0,	
	EyeState_Open = 1,
	EyeState_Close = 2,
}Eye_State;


//ADASϵͳ����
typedef struct _Adas_Params
{
	float			pitch;				//������
	float			roll;				//�����
	int				vehicleType;		//����(С���С�����������峵��)
	float			height;				//��װ�߶�
	float			width;				//�������
	float			horizonLevel;		//��ƽ����ͼ���ϵ�����λ�ñ�����As % of image height, range is 0-100, topmost row being 0 
	float			horizonPan;			//��ǰ����ͼ���ϵĺ���λ�ñ�����As % of image width, range is 0-100, topmost row being 0
}Adas_Params;

//����ƫ��Ԥ������
typedef struct _LDW_Params
{
	int             IsEnabled;			//�Ƿ���
	float			minVelocity;		//����ٶ� km/h
	float			sensitivity;		//���ж�(0--1.0)
}LDW_Params;


//ǰ��Ԥ������
typedef struct _FCW_Params
{
	int             IsEnabled;			//�Ƿ���
	float			minVelocity;		//����ٶ� km/h
	float			sensitivity;		//���ж�(0--1.0)
}FCW_Params;

//ƣ��Ԥ������
typedef struct _DFW_Params
{
	int             IsEnabled;			//�Ƿ���
	float			minVelocity;		//����ٶ� km/h
	float			sensitivity;		//���ж�(0--1.0)
}DFW_Params;

//��������Ϣ
typedef struct _RoadMarkLine_Info
{
	int isCredible;			//�Ƿ����
	TxPoint2f points[2];	                                                                                                                

}RoadMarkLine_Info;

//����ƫ�������Ϣ
typedef struct _LDW_Output
{
	LDW_State		state;				//����ƫ��״̬

	RoadMarkLine_Info left;				//�󳵵�����Ϣ	
	RoadMarkLine_Info right;			//�󳵵�����Ϣ	
}LDW_Output;

//ǰ��Ԥ�������Ϣ
typedef struct _FCW_Output
{
	FCW_State		state;			//ǰ��Ԥ��״̬
	float			dis;			//ǰ������
	float			t;				//������ײʱ��
	float			s;				//��ȫ����

	int				carNum;			//��������,0-3
	TxRectf			car[3];			//����λ����Ϣ
}FCW_Output;

//��ʻԱ��Ϣ
typedef struct _Driver_Info
{
	DFW_State state;			//��ǰ״̬
	Head_Posture headMoving;	//���ڵ�ͷ���˶�
}Driver_Info;

//������Ϊ
typedef struct _Vehicle_Behavior
{
	int		motion;			//����or����
	int		direction;		//ֱ��or����
}Vehicle_Behavior;

//������Ϣ
typedef struct _Face_Info
{
	Eye_State state;//��������
	TxRectf faceRect;
	TxRectf regionRect;
	TxVec2f eyePoints[68];

	float yaw, pitch, roll;
	int bValidPose;     // 0�޽Ƕ�; 1�нǶ�,���Ŷȵ�; 2���Ƕ���Ч�����Ŷȸ�

	int flag;  
	float leftconfidence[3];
	float rightconfidence[3];

	float saveleftconfidence[30];
	float saverightconfidence[30];

	float confidence_face;
	float confidence_mouth;
	float confidence_glasses[2];

	short bTrackingFace;
	int nlastglassnum[10];

	int nframe;

	int delspot;
    unsigned char ucLeftSportGray;
    unsigned char ucRightSportGray;
    //TxVec2f pLeftSpotPoint[50];
    //TxVec2f pRightSpotPoint[50];

	short iEYE_Clf_Label;			// ��֡���ī��label
	short rSunGlassesLabel;			// globalī��label
	char video_name[64];
}Face_Info;

//ƣ��Ԥ�������Ϣ
typedef struct _DFW_Output
{
	Driver_Info driver;
	Face_Info face;
}DFW_Ouput;

//GPS ��Ϣ
typedef struct _GPS_Info
{
	float lat;			//latitude in degrees
	float lon;			//longitude in degrees
	float alt;			//altitude in meters
	float speed;		//��λ:m/s
	float heading;		//��λ:degree����������˳ʱ��н�
	float hAcc;			//ˮƽ����
	float vAcc;			//��ֱ����	
	char  utcTime[20];	//ʱ��� Current time	hhmmss.sss
	char  date[7];		//Current Date in Day, Month Year format ddmmyy
	char  flag;			//�Ƿ���Ч
}GPS_Info;

//GSensor��Ϣ
typedef struct _GSensor_Info
{
	float heading;		//�����,��λ:degree
	float pitch;		//������,��λ:degree
	float roll;			//�����,��λ:degree
	
	float acc[3];		//accelerometer data(x,y and z direction) in m/s^2
	float gyro[3];		//3-axis gryoscope data in degrees/sec
	float vg[3];		//gravity vector data in ms^2

}GSensor_Info;

//������Ϣ
typedef struct _AUX_DATA
{
	GPS_Info gps;
	GSensor_Info sensor;
	Driver_Info driver;
	Vehicle_Behavior behavior;
}AUX_DATA;


//����״̬
typedef struct _Scene_Status
{
	float	x0;				//ʵʱ���X
	float	y0;				//ʵʱ���Y
	float	x1;				//�ȶ����
	float	y1;				//�ȶ����
	float	laneWidth;		//ƽ���������
	float	vehicleHeight;	//����
	float	laneCenter;		//��������λ��
	// -2���궨ʧ�ܣ�-1:�궨�쳣�� 0����ʼ���� 1���궨��ȷ�� 2�����±궨�� 
	// 3������ͷ��Ҫ�µ���4������ͷ��Ҫ�ϵ���5������ͷ��Ҫ�����6������ͷ��Ҫ�ҵ�
	int calibration;		//�ڲ��궨״̬
	// calibrationOK��0:��ʼ����1���궨�ɹ���2���궨�쳣
	int calibrationOK;		//ȫ�ֱ궨�Ƿ�OK
	float y_car;
}Scene_Status;

typedef struct _Calibration_Info
{
	int state;
	int  vx;
	int  vy;
}Calibration_Info;
//���ݳ��͵õ�Ĭ�ϵĳ�����Ⱥ������װ�߶�
BOOL getVehicleInfobyType(VehicleType type, float *width, float* height);

#ifdef __cplusplus
}

#endif
