#include "cooperativeballpassingfsm.h"
#include <algorithm>
#include <atltypes.h>
#include <cassert>
#include <initializer_list>
#include <memory>
#include <optional>

auto CooperativeBallPassingFSM::Fish::reset(CFishInfo* target_fish, CFishAction* target_action)
	-> Fish& {
	fish   = target_fish;
	action = target_action;
	return *this;
}

CFishInfo* CooperativeBallPassingFSM::Fish::operator->() {
	return fish;
}

void CooperativeBallPassingFSM::Fish::goStraight(int speed_gear) {
	action->direction = static_cast<int>(action->direction * 0.8);
	if (speed_gear != -1) {
		action->speed = speed_gear;
	}
	fish->updateAction(*action);
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
	const auto& [execute, cons_in, cons_out] = getUnpackPolicy(policyid_);
	const auto& succs						 = std_[policyid_];

	bool result = execute(this);

	if (bool transit = cons_out(this); transit) {
		for (int policyid : succs) {
			const auto& validate = std::get<1>(getUnpackPolicy(policyid));
			if (validate(this)) {
				policyid_ = policyid;
				break;
			}
		}
	}

	//! 标定目标点
	int r = 8;
	for (int i = 0; i <= 1; ++i) {
		const auto goal = fish(i)->targetPos();
		for (int row = goal.y - r; row <= goal.y + r; ++row) {
			for (int col = goal.x - r; col <= goal.x + r; ++col) {
				auto data = reinterpret_cast<uint8_t*>(
					&visframe_->imageData[row * visframe_->widthStep + col * visframe_->nChannels]);
				data[0] = !!i ? 0x00 : 0xff;
				data[1] = 0x00;
				data[2] = !!i ? 0xff : 0x00;
			}
		}
	}

	return result;
}

CooperativeBallPassingFSM::CooperativeBallPassingFSM() noexcept {
	registerPolicy("Hover", [](CooperativeBallPassingFSM* self) {
		std::cout << "Strategy: Hover\n" << std::flush;
		self->fish(0).stableHover();
		self->fish(1).stableHover();
		return true;
	});

	registerPolicy(
		"A -> door[0] (1)",
		[](CooperativeBallPassingFSM* self) {
			std::cout << "Strategy: A -> door[0] (1)\n" << std::flush;
			self->fish(0).trivalMove(self->constants().centers[0], 3);
			return true;
		},
		[](CooperativeBallPassingFSM* self) {
			return self->fish(0)->centerPos().y < self->constants().y.half;
		},
		[](CooperativeBallPassingFSM* self) {
			auto p1 = self->fish(0)->headerPos(), p2 = self->constants().centers[0];
			return p1.x >= p2.x && p1.y <= p2.y;
		});

	registerPolicy(
		"A -> door[0] (2)",
		[](CooperativeBallPassingFSM* self) {
			std::cout << "Strategy: A -> door[0] (2)\n" << std::flush;
			if (auto& fish = self->fish(0); fish->headerPos().y >= self->door(0).y) {
				fish.goStraight(3);
			} else {
				fish.trivalMove(self->door(0), 5);
			}
			return true;
		},
		nullpolicy,
		[](CooperativeBallPassingFSM* self) {
			return self->fish(0)->centerPos().y > self->constants().y.half;
		});

	registerPolicy(
		"A -> door[1] (1)",
		[](CooperativeBallPassingFSM* self) {
			std::cout << "Strategy: A -> door[1] (1)\n" << std::flush;
			self->fish(0).trivalMove(self->constants().centers[4]);
			return true;
		},
		nullpolicy,
		[](CooperativeBallPassingFSM* self) {
			auto p1 = self->fish(0)->headerPos(), p2 = self->constants().centers[4];
			return p1.x >= p2.x || p1.y >= p2.y;
		});

	registerPolicy(
		"A -> door[1] (2)",
		[](CooperativeBallPassingFSM* self) {
			std::cout << "Strategy: A -> door[1] (2)\n" << std::flush;
			auto goal = self->door(1);
			goal.y -= self->constants().y.quarter;
			if (auto& fish = self->fish(0); fish->headerPos().y > goal.y) {
				fish.trivalMove(goal, 8);
			} else {
				fish.trivalMove(CPoint(self->constants().width, 0), 10);
			}
			return true;
		},
		nullpolicy,
		[](CooperativeBallPassingFSM* self) {
			auto p1 = self->fish(0)->headerPos(), p2 = self->constants().centers[2];
			return p1.x >= p2.x && p1.y < p2.y;
		});

	//! TODO: 在此处注册策略

	configurePolicy("Hover", {"A -> door[0] (1)"});
	configurePolicy("A -> door[0] (1)", {"A -> door[0] (2)"});
	configurePolicy("A -> door[0] (2)", {"A -> door[1] (1)"});
	configurePolicy("A -> door[1] (1)", {"A -> door[1] (2)"});

	//! TODO: 在此处配置策略

	forceSetPolicy("Hover");
}

auto CooperativeBallPassingFSM::regionPredict(const CPoint& pos) const -> Region {
	//! TODO
	return Region::Empty;
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
	constants_.width	  = visframe_->width;
	constants_.height	  = visframe_->height;
	constants_.x.half	  = constants_.width / 2;
	constants_.x.third	  = constants_.width / 3;
	constants_.x.quarter  = constants_.width / 4;
	constants_.y.half	  = constants_.height / 2;
	constants_.y.third	  = constants_.height / 3;
	constants_.y.quarter  = constants_.height / 4;
	constants_.centers[0] = CPoint(door_center_[0].x / 2, constants_.y.quarter);
	constants_.centers[1] =
		CPoint((door_center_[0].x + door_center_[1].x) / 2, constants_.y.quarter);
	constants_.centers[2] =
		CPoint((door_center_[1].x + constants_.width) / 2, constants_.y.quarter);
	constants_.centers[3] = CPoint(constants_.centers[0].x, constants_.y.quarter * 3);
	constants_.centers[4] = CPoint(constants_.centers[1].x, constants_.y.quarter * 3);
	constants_.centers[5] = CPoint(constants_.centers[2].x, constants_.y.quarter * 3);

	return true;
}

auto CooperativeBallPassingFSM::registerPolicy(const std::string& name, PolicyKernel action,
											   PolicyKernel in_constraint,
											   PolicyKernel out_constraint)
	-> std::optional<Policy> {
	//! 排除已经注册的策略
	if (auto it = simap_.find(name); it != simap_.end()) {
		return std::nullopt;
	}

	//! 构造策略对象
	PolicyConstraint constraint(in_constraint, out_constraint);
	Policy			 policy(action, std::move(constraint));

	//! 注册策略
	int policyid = simap_.insert({name, (int)simap_.size()}).second;
	policies_.insert({policyid, policy});
	return {policy};
}

bool CooperativeBallPassingFSM::configurePolicy(const std::string&				   src_policy,
												std::initializer_list<std::string> dst_policies) {
	//! 获取源策略编号
	int id = -1;
	if (auto it = simap_.find(src_policy); it != simap_.end()) {
		id = it->second;
	} else {
		return false;
	}

	// ! 获取当前后继列表
	auto [it, result] = std_.insert_or_assign(id, decltype(std_)::mapped_type{});
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

bool CooperativeBallPassingFSM::forceSetPolicy(const std::string& policy) {
	if (auto it = simap_.find(policy); it != simap_.end()) {
		policyid_ = it->second;
		return true;
	} else {
		return false;
	}
}

auto CooperativeBallPassingFSM::getUnpackPolicy(int policyid) const
	-> std::tuple<PolicyKernel, PolicyKernel, PolicyKernel> {
	assert(policies_.find(policyid) != policies_.end());
	const auto& policy = policies_.find(policyid)->second;
	return std::tuple{policy.first, policy.second.first, policy.second.second};
}

CPoint CooperativeBallPassingFSM::door(int index) const {
	assert(index >= 0 && index < 3);
	return door_center_[index];
}

auto CooperativeBallPassingFSM::fish(int index) -> Fish& {
	assert(index >= 0 && index < 2);
	return fishes_[index];
}

const CBallInfo& CooperativeBallPassingFSM::ball() const {
	return *ball_;
}

auto CooperativeBallPassingFSM::constants() const -> decltype(constants_) {
	return constants_;
}