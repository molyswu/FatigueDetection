#include "adas.h"

//根据车型得到默认的车辆宽度和相机安装高度
BOOL getVehicleInfobyType(VehicleType type, float *width, float* height)
{
	const float Config_VehicleWidth[3] = {1.6f, 1.8f, 2.0f};
	const float Config_CameraHeight[3] = {1.2f, 1.4f, 1.6f};
	if (type>VehicleType_Large)
	{
		return FALSE;
	}

	*width = Config_VehicleWidth[type];
	*height = Config_CameraHeight[type];

	return TRUE;
}