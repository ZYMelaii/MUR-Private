#include "cooperativeballpassingfsm.h"
#include <cassert>

CooperativeBallPassingFSM::Fish& CooperativeBallPassingFSM::Fish::reset(CFishInfo*	 targetFish,
																		CFishAction* targetAction) {
	fish   = targetFish;
	action = targetAction;
	return *this;
}

void CooperativeBallPassingFSM::Fish::trivalMove(const CPoint& goal, int speed_gear) {
	auto direction = normalizeAngle(getVecAngle(fish->headerPos(), goal));
	fish->setTarget(goal, direction);
	if (speed_gear != -1) {
		action->speed = speed_gear;
	}
	spinP2PMove(fish->targetPos(), *fish, *action);
	fish->updateAction(*action);
}

void CooperativeBallPassingFSM::Fish::stableHover() {
	action->speed	  = 1;
	action->direction = 12;
	fish->updateAction(*action);
}

void CooperativeBallPassingFSM::Fish::bevelMove(const CPoint& goal) {
	//! TODO
}

bool CooperativeBallPassingFSM::Strategy(RefArray<CFishAction> aAction, RefArray<CFishInfo> aFish,
										 RefArray<CBallInfo> aBallinfo,
										 RefArray<OBSTAINFO> aObstacle,
										 RefArray<CHANNEL>	 aChannel) {
	//! TODO
	return true;
}

CooperativeBallPassingFSM::CooperativeBallPassingFSM() noexcept {
	//! TODO
}

CooperativeBallPassingFSM::Region CooperativeBallPassingFSM::regionPredict(
	const CPoint& pos) const {
	//! TODO
	return Region::Empty;
}

CooperativeBallPassingFSM::Phase CooperativeBallPassingFSM::phasePredict(
	const CPoint& a_pos, const CPoint& b_pos, const CPoint& ball_pos) const {
	//! TODO
	return Phase::Origin;
}


CPoint CooperativeBallPassingFSM::door(int index) const {
	assert(index >= 0 && index < 3);
	return door_center_[index];
}

CooperativeBallPassingFSM::Fish& CooperativeBallPassingFSM::fish(int index) {
	assert(index >= 0 && index < 2);
	return fishes_[index];
}

const CBallInfo& CooperativeBallPassingFSM::ball() const {
	return *ball_;
}