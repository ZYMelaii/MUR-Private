#ifndef MUR_STRATEGYHELPER_H
#define MUR_STRATEGYHELPER_H

#ifndef MURSTRATEGY_EXPORTS
#define MURSTRATEGY_EXPORTS 1
#endif

#ifdef STRATEGY_API
#undef STRATEGY_API
#endif

#ifdef MURSTRATEGY_EXPORTS
#define STRATEGY_API extern "C" __declspec(dllexport)
#else
#define STRATEGY_API __declspec(dllimport)
#endif

#include "../official/struct.h"
#include "../official/targetver.h"
#include "../official/externrobot.h"
#include "../official/ballinfo.h"
#include "../official/fishaction.h"
#include "../official/fishinfo.h"
#include "../official/strategy.h"

#define BeginExportMURStrategy(OriginImage, RecogImage, aAction, aFish, aBallinfo, aObstacle, aChannel) \
STRATEGY_API BOOL MURStrategy( \
	IplImage *OriginImage, IplImage *RecogImage, \
	CFishAction *_##aAction, int n_##aAction, \
	CFishInfo *_##aFish, int n_##aFish, \
	CBallInfo *_##aBallinfo, int n_##aBallinfo, \
	OBSTAINFO *_##aObstacle, int n_##aObstacle, \
	CHANNEL *_##aChannel, int n_##aChannel) { \
	RefArray aAction(_##aAction, n_##aAction); \
	RefArray aFish(_##aFish, n_##aFish); \
	RefArray aBallinfo(_##aBallinfo, n_##aBallinfo); \
	RefArray aObstacle(_##aObstacle, n_##aObstacle); \
	RefArray aChannel(_##aChannel, n_##aChannel);

#define EndExport() return true; }

#define ExportStategy(StategyClass) \
	BeginExportMURStrategy(unused1, unused2, _1, _2, _3, _4, _5); \
	static auto strategy = new StategyClass(); \
	strategy->Strategy(_1, _2, _3, _4, _5); \
	return true; \
	EndExport()

#endif /*MUR_STRATEGYHELPER_H*/