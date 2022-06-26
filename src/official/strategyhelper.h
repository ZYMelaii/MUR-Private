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

#define ExportMURStrategy(OriginImage, RecogImage, action, fishinfo, ballinfo, obstacle, channel) \
STRATEGY_API BOOL MURStrategy( \
	IplImage *OriginImage, IplImage *RecogImage, \
	CFishAction *action, int n_##action, \
	CFishInfo *fishinfo, int n_##fishinfo, \
	CBallInfo *ballinfo, int n_##ballinfo, \
	OBSTAINFO *obstacle, int n_##obstacle, \
	CHANNEL *channel, int n_##channel)

#endif /*MUR_STRATEGYHELPER_H*/