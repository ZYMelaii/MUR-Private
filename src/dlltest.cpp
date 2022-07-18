#include "./official/strategyhelper.h"
#include "./strategy/cooperativeballpassingfsm.h"
#include <windows.h>

int main(int argc, char* argv[]) {
	CooperativeBallPassingFSM strategy;

	//! construct tricks
	CFishAction action[16];
	CFishInfo	fish[16];
	CBallInfo	ball[8];
	OBSTAINFO	obstacle[16];
	CHANNEL		channel[16]{{.center{220, 236}}, {.center{474, 240}}, {.center{600, 280}}};

	IplImage origin{
		.nChannels = 3,
		.width	   = 720,
		.height	   = 480,
		.imageData = new char[origin.width * origin.height * origin.nChannels],
		.widthStep = (origin.width * origin.nChannels),
	};

	//! execute strategy
#define MURArgs {action, 16}, {fish, 16}, {ball, 16}, {obstacle, 16}, {channel, 3}
	strategy.updateVisualFrame(&origin);
	strategy.refreshScene(MURArgs);
	strategy.Strategy(MURArgs);
#undef MURArgs

	//! cleanup
	delete[] origin.imageData;
	origin.imageData = nullptr;

	return 0;
}