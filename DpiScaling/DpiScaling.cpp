#include "DpiScaling.h"

#include <Windows.h>

const UINT32 dpiList[] = { 100, 125, 150, 175, 200, 225, 250, 300, 350, 400, 450, 500 };

// 获取系统推荐DPI
int GetRecommendedDPIScaling()
{
	int dpiIndex = 0;
	bool ret = SystemParametersInfo(SPI_GETLOGICALDPIOVERRIDE, 0, (LPVOID)&dpiIndex, 1);

	if (ret)
	{
		int recommendedDpi = dpiList[dpiIndex * -1];
		return recommendedDpi;
	}

	return -1;
}

// 修改DPI
void SetDpiScaling(int dpiScaleLevel)
{
	int recommendedDpiScale = GetRecommendedDPIScaling();

	if (recommendedDpiScale > 0)
	{
		int index = 0, recIndex = 0, setIndex = 0;
		for (const UINT32& scale : dpiList)
		{
			if (recommendedDpiScale == scale)
			{
				recIndex = index;
			}
			if (dpiScaleLevel == scale)
			{
				setIndex = index;
			}
			index++;
		}

		int relativeIndex = setIndex - recIndex;
		SystemParametersInfo(SPI_SETLOGICALDPIOVERRIDE, relativeIndex, (LPVOID)0, 1);
	}
}

// 重置DPI缩放级别为100%
void ResetDpiScaling()
{
	SetDpiScaling(100);
}
