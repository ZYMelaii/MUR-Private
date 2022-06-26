#include "official/strategyhelper.h"
#include "official/strategy.h"

#include <iostream>

static int MUR_GlobalCounter__zymelaii = 0;

ExportMURStrategy(unused1, unused2, actions, fish, ballinfo, obstacles, channels) {
	static CStrategy *pFishStrategy = new CStrategy();
	pFishStrategy->Strategynew(actions, fish, ballinfo, obstacles, channels);
	printf("called strategy for %d times: address of [static] CStrategy = 0x%p\n",
		++MUR_GlobalCounter__zymelaii, pFishStrategy);
	return true;
}