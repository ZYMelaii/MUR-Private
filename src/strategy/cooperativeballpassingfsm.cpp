#include "cooperativeballpassingfsm.h"
#include <algorithm>
#include <cassert>
#include <initializer_list>
#include <memory>
#include <optional>

CooperativeBallPassingFSM::Fish& CooperativeBallPassingFSM::Fish::reset(
	CFishInfo* target_fish, CFishAction* target_action) {
	fish   = target_fish;
	action = target_action;
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

bool CooperativeBallPassingFSM::updateVisualFrame(IplImage* image) {
	if (image == visframe_) {
		return false;
	} else {
		visframe_ = image;
		return true;
	}
}

bool CooperativeBallPassingFSM::refreshScene(RefArray<CFishAction> aAction,
											 RefArray<CFishInfo>   aFish,
											 RefArray<CBallInfo>   aBallinfo,
											 RefArray<OBSTAINFO>   aObstacle,
											 RefArray<CHANNEL> aChannel, bool wait_for_track) {
	//! 中断通道数不为3的情形
	if (aChannel.size() != 3) {
		return false;
	}

	//! 视觉标定判定
	if (wait_for_track) {
		auto p	  = CPoint(-1, -1);
		auto cond = true;
		//! 无效坐标判定
		p = aFish[0].centerPos();
		cond &= p.x >= 0 && p.x <= visframe_->width;
		cond &= p.y >= 0 && p.y <= visframe_->height;
		p = aFish[1].centerPos();
		cond &= p.x >= 0 && p.x <= visframe_->width;
		cond &= p.y >= 0 && p.y <= visframe_->height;
		p = aBallinfo[0].getCenter();
		cond &= p.x >= 0 && p.x <= visframe_->width;
		cond &= p.y >= 0 && p.y <= visframe_->height;
		//! TODO: 速度-位移冲突判定
		//! 任一判定出错，中断刷新
		if (!cond) {
			return false;
		}
	}

	//! 假定通道与门顺次对应
	door_center_[0] = aChannel[0].center;
	door_center_[1] = aChannel[1].center;
	door_center_[2] = aChannel[2].center;

	//! 假定鱼与动作跟A鱼、B鱼顺次对应
	fishes_[0].reset(&aFish[0], &aAction[0]);
	fishes_[1].reset(&aFish[1], &aAction[1]);

	//! 假定水球为一号标定
	ball_ = &aBallinfo[0];

	//! 常量更新
	constants_.width  = visframe_->width;
	constants_.height = visframe_->height;

	return true;
}

std::optional<CooperativeBallPassingFSM::Policy> CooperativeBallPassingFSM::registerPolicy(
	std::string name, CooperativeBallPassingFSM::PolicyKernel action,
	CooperativeBallPassingFSM::PolicyKernel in_constraint,
	CooperativeBallPassingFSM::PolicyKernel out_constraint) {
	//! 排除已经注册的策略
	if (auto it = simap_.find(name); it != simap_.end()) {
		return std::nullopt;
	}

	//! 构造策略对象
	PolicyConstraint constraint(in_constraint, out_constraint);
	Policy			 policy(action, std::move(constraint));

	//! 注册策略
	int policyid = simap_.insert({name, simap_.size()}).second;
	policies_.insert({policyid, policy});
	return {policy};
}

bool CooperativeBallPassingFSM::configurePolicy(std::string						   src_policy,
												std::initializer_list<std::string> dst_policies) {
	//! 获取源策略编号
	int id = -1;
	if (auto it = simap_.find(src_policy); it != simap_.end()) {
		id = it->second;
	} else {
		return false;
	}

	//! 获取当前后继列表
	auto [it, result] = std_.insert_or_assign(id, decltype(std_)::value_type{});
	auto& succs		  = it->second;

	for (const auto& dst_policy : dst_policies) {
		if (auto it = simap_.find(dst_policy); it != simap_.end()) {
			succs.push_back(it->second);
		}
	}

	//! 后继去重
	succs.erase(std::unique(succs.begin(), succs.end()), succs.end());
	return true;
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

const auto& CooperativeBallPassingFSM::constants() const {
	return constants_;
}