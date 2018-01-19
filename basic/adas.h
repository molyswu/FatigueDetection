#pragma once

#include "../basic/core.h"

#ifdef __cplusplus
extern "C" {
#endif

//车型
typedef enum _VehicleType
{
	VehicleType_Car = 0,
	VehicleType_Medium = 1,
	VehicleType_Large = 2
}VehicleType;

//车道偏离预警状态
typedef enum _LDW_State
{
	LDW_Normal = 0,
	LDW_OffsetToLeft = 1,
	LDW_OffsetToRight = 2,
	LDW_CrossLeft = 3,
	LDW_CrossRight = 4
}LDW_State;

//车辆预警状态
typedef enum _FCW_State
{
	FCW_Normal = 0,
	FCW_Careful = 1,
	FCW_Dangerous = 2
}FCW_State;

//前车起步提醒
typedef enum _StopGo_State
{		
	StopGo_Normal = 0,		//正常		
	StopGo_Remind = 1		//提醒
}StopGo_State;

//行人预警状态
typedef enum _PCW_State
{
	PCW_Normal = 0,
	PCW_Careful = 1,
	PCW_Dangerous = 2
}PCW_State;

//疲劳驾驶预警状态
typedef enum _DFW_State
{
	DFW_Unknown	= 0,			
	DFW_Normal = 1,				//正常		
	DFW_Fatigue_Medium = 2,		//中度疲劳或注意力不集中
	DFW_Fatigue_Serious = 3		//严重疲劳或严重分神

}DFW_State;

//头部姿态
typedef enum _Head_Posture
{
	Head_Unknown = 0,
	Head_Normal = 1,
	Head_TurnLeft = 2,
	Head_TurnRight = 3,
	Head_Tilt = 4
	
}Head_Posture;

//睁闭眼状态
typedef enum _Eye_State
{
	EyeState_UnKnown = 0,	
	EyeState_Open = 1,
	EyeState_Close = 2,
}Eye_State;


//ADAS系统参数
typedef struct _Adas_Params
{
	float			pitch;				//俯仰角
	float			roll;				//横滚角
	int				vehicleType;		//车型(小、中、大或其他具体车型)
	float			height;				//安装高度
	float			width;				//车辆宽度
	float			horizonLevel;		//地平线在图像上的纵向位置比例，As % of image height, range is 0-100, topmost row being 0 
	float			horizonPan;			//正前方在图像上的横向位置比例，As % of image width, range is 0-100, topmost row being 0
}Adas_Params;

//车道偏离预警参数
typedef struct _LDW_Params
{
	int             IsEnabled;			//是否开启
	float			minVelocity;		//最低速度 km/h
	float			sensitivity;		//敏感度(0--1.0)
}LDW_Params;


//前车预警参数
typedef struct _FCW_Params
{
	int             IsEnabled;			//是否开启
	float			minVelocity;		//最低速度 km/h
	float			sensitivity;		//敏感度(0--1.0)
}FCW_Params;

//疲劳预警参数
typedef struct _DFW_Params
{
	int             IsEnabled;			//是否开启
	float			minVelocity;		//最低速度 km/h
	float			sensitivity;		//敏感度(0--1.0)
}DFW_Params;

//车道线信息
typedef struct _RoadMarkLine_Info
{
	int isCredible;			//是否可信
	TxPoint2f points[2];	                                                                                                                

}RoadMarkLine_Info;

//车道偏离输出信息
typedef struct _LDW_Output
{
	LDW_State		state;				//车道偏离状态

	RoadMarkLine_Info left;				//左车道线信息	
	RoadMarkLine_Info right;			//左车道线信息	
}LDW_Output;

//前车预警输出信息
typedef struct _FCW_Output
{
	FCW_State		state;			//前车预警状态
	float			dis;			//前车距离
	float			t;				//估计碰撞时间
	float			s;				//安全车距

	int				carNum;			//车辆个数,0-3
	TxRectf			car[3];			//车辆位置信息
}FCW_Output;

//驾驶员信息
typedef struct _Driver_Info
{
	DFW_State state;			//当前状态
	Head_Posture headMoving;	//近期的头部运动
}Driver_Info;

//车辆行为
typedef struct _Vehicle_Behavior
{
	int		motion;			//加速or减速
	int		direction;		//直行or拐弯
}Vehicle_Behavior;

//人脸信息
typedef struct _Face_Info
{
	Eye_State state;//人脸检测框
	TxRectf faceRect;
	TxRectf regionRect;
	TxVec2f eyePoints[68];

	float yaw, pitch, roll;
	int bValidPose;     // 0无角度; 1有角度,可信度低; 2、角度有效，可信度高

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

	short iEYE_Clf_Label;			// 单帧输出墨镜label
	short rSunGlassesLabel;			// global墨镜label
	char video_name[64];
}Face_Info;

//疲劳预警输出信息
typedef struct _DFW_Output
{
	Driver_Info driver;
	Face_Info face;
}DFW_Ouput;

//GPS 信息
typedef struct _GPS_Info
{
	float lat;			//latitude in degrees
	float lon;			//longitude in degrees
	float alt;			//altitude in meters
	float speed;		//单位:m/s
	float heading;		//单位:degree，与正北的顺时针夹角
	float hAcc;			//水平精度
	float vAcc;			//垂直精度	
	char  utcTime[20];	//时间戳 Current time	hhmmss.sss
	char  date[7];		//Current Date in Day, Month Year format ddmmyy
	char  flag;			//是否有效
}GPS_Info;

//GSensor信息
typedef struct _GSensor_Info
{
	float heading;		//方向角,单位:degree
	float pitch;		//俯仰角,单位:degree
	float roll;			//横滚角,单位:degree
	
	float acc[3];		//accelerometer data(x,y and z direction) in m/s^2
	float gyro[3];		//3-axis gryoscope data in degrees/sec
	float vg[3];		//gravity vector data in ms^2

}GSensor_Info;

//辅助信息
typedef struct _AUX_DATA
{
	GPS_Info gps;
	GSensor_Info sensor;
	Driver_Info driver;
	Vehicle_Behavior behavior;
}AUX_DATA;


//场景状态
typedef struct _Scene_Status
{
	float	x0;				//实时灭点X
	float	y0;				//实时灭点Y
	float	x1;				//稳定灭点
	float	y1;				//稳定灭点
	float	laneWidth;		//平均车道宽度
	float	vehicleHeight;	//车高
	float	laneCenter;		//车道中心位置
	// -2：标定失败；-1:标定异常； 0：初始化； 1：标定正确； 2：重新标定； 
	// 3：摄像头需要下调；4：摄像头需要上调；5：摄像头需要左调；6：摄像头需要右调
	int calibration;		//内部标定状态
	// calibrationOK：0:初始化；1：标定成功；2：标定异常
	int calibrationOK;		//全局标定是否OK
	float y_car;
}Scene_Status;

typedef struct _Calibration_Info
{
	int state;
	int  vx;
	int  vy;
}Calibration_Info;
//根据车型得到默认的车辆宽度和相机安装高度
BOOL getVehicleInfobyType(VehicleType type, float *width, float* height);

#ifdef __cplusplus
}

#endif
