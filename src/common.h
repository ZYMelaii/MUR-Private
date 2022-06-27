#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>
#include <float.h>
#include <malloc.h>
#include <string.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <omp.h>

#if __has_include(<atltypes.h>)
#include <atltypes.h>
#else
#include "./point.h"
#endif

#if __has_include(<cv.h>)
#include <cv.h>
#else
typedef struct _IplTileInfo {} IplTileInfo;
typedef struct _IplROI {
    int  coi;
    int  xOffset;
    int  yOffset;
    int  width;
    int  height;
} IplROI;
typedef struct _IplImage {
	int  nSize;
	int  ID;
	int  nChannels;
	int  alphaChannel;
	int  depth;
	char colorModel[4];
	char channelSeq[4];
	int  dataOrder;
	int  origin;
	int  align;
	int  width;
	int  height;
	struct _IplROI *roi;
	struct _IplImage *maskROI;
	void  *imageId;
	struct _IplTileInfo *tileInfo;
	int  imageSize;
	char *imageData;
	int  widthStep;
	int  BorderMode[4];
	int  BorderConst[4];
	char *imageDataOrigin;
} IplImage;
#endif

#endif /*COMMON_H*/