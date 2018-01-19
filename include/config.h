#pragma once

#ifdef __cplusplus
extern "C" {
#endif


//输入图像分辨率
#define IMG_WIDTH 640
#define IMG_HEIGHT 360

// 后置摄像头
#define REAR_IMG_WIDTH 640
#define REAR_IMG_HEIGHT 360


// 输入视频帧率
#define FRAME_PER_SECOND		30
#define FACE_PROCESS_STEP		3
#define FRONT_CAR_REMIND_STEP	6


//疲劳检测帧数
#define Fatigue_RecordCount		600


#ifdef __cplusplus
}
#endif
