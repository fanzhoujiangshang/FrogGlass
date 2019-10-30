#include <math.h>
#include "sensorsalignment.h"
#include "maths.h"
#include "axis.h"
#include "storageTask.h"

/********************************************************************************	 
 * ���������������������	
 * ��������:2018/5/2
 * �汾��V1.2
 * ��Ȩ���У�����ؾ���
 * All rights reserved
********************************************************************************/


static bool standardBoardAlignment = true;     
static float boardRotation[3][3];//��ת����΢���ã�


static bool isBoardAlignmentStandard(const boardAlignment_t *boardAlignment)
{
    return !boardAlignment->rollDeciDegrees && !boardAlignment->pitchDeciDegrees && !boardAlignment->yawDeciDegrees;
}

void applyBoardAlignment(int16_t *vec)
{
	if (standardBoardAlignment) 
	{
		return;
	}

	int16_t x = vec[X];
	int16_t y = vec[Y];
	int16_t z = vec[Z];

	vec[X] = lrintf(boardRotation[0][X] * x + boardRotation[1][X] * y + boardRotation[2][X] * z);
	vec[Y] = lrintf(boardRotation[0][Y] * x + boardRotation[1][Y] * y + boardRotation[2][Y] * z);
	vec[Z] = lrintf(boardRotation[0][Z] * x + boardRotation[1][Z] * y + boardRotation[2][Z] * z);
}


void applySensorAlignment(int16_t * dest, int16_t * src, uint8_t rotation)
{
    const int16_t x = src[X];
    const int16_t y = src[Y];
    const int16_t z = src[Z];

    switch (rotation) {
    default:
    case CW0_DEG:
        dest[X] = x;
        dest[Y] = y;
        dest[Z] = z;
        break;
    case CW90_DEG:
        dest[X] = y;
        dest[Y] = -x;
        dest[Z] = z;
        break;
    case CW180_DEG:
        dest[X] = -x;
        dest[Y] = -y;
        dest[Z] = z;
        break;
    case CW270_DEG:
        dest[X] = -y;
        dest[Y] = x;
        dest[Z] = z;
        break;
    case CW0_DEG_FLIP:
        dest[X] = -x;
        dest[Y] = y;
        dest[Z] = -z;
        break;
    case CW90_DEG_FLIP:
        dest[X] = y;
        dest[Y] = x;
        dest[Z] = -z;
        break;
    case CW180_DEG_FLIP:
        dest[X] = x;
        dest[Y] = -y;
        dest[Z] = -z;
        break;
    case CW270_DEG_FLIP:
        dest[X] = -y;
        dest[Y] = -x;
        dest[Z] = -z;
        break;
    }
}

