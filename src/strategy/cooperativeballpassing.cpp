#include "cooperativeballpassing.h"
#include <algorithm>

CooperativeBallPassing::CooperativeBallPassing(int width, int height) :
    initialized_(false), width_(width), height_(height) {
    /**
     * @note 相关信息初始皆被设定为可判定的无效量，下面给出
     *  数据无效的判定：
     *  fish_[*].centerPos() == CPoint(CFishInfo::nil, CFishInfo::nil)
     *  ball_.getCenter() == CPoint(-1, -1)
     *  door_center_[*] == CPoint(0, 0)
     */
}

void CooperativeBallPassing::stableHoverInstruct(CFishInfo &fish, CFishAction &action) {
    if (!initialized_) return;
    // TODO
}

CooperativeBallPassing::Region CooperativeBallPassing::regionPredict(const CPoint &pos) const {
    int flag = Region::Empty;
    flag |= pos.y <= height_ / 2 ? Region::Upper : Region::Lower;
    flag |= pos.x <= width_ / 3 ? Region::Left : (pos.x >= width_ * 2 / 3 ? Region::Right : Region::Middle);
    return static_cast<Region>(flag);
}

CooperativeBallPassing::Phase CooperativeBallPassing::phasePredict() const {
    const auto reg_1 = regionPredict(fish_[0].centerPos());
    const auto reg_2 = regionPredict(fish_[1].centerPos());
    if ((reg_1 & Region::Left) && reg_2 == Region::LL) {
        return Phase::Stage_1st;
    } else if (reg_1 == Region::UR && !(reg_2 & Region::Left)) {
        return Phase::Stage_3rd;
    } else {
        return Phase::Stage_2nd;
    }
}

void CooperativeBallPassing::StageInstruct_1st(RefArray<CFishAction> aAction) {
    puts("[INFO] 正在执行第一阶段策略");

    const auto threshold = getDistance(door_center_[0],
        CPoint(width_ / 7, height_ * 2 / 6));
    const auto distance = getDistance(fish_[0].headerPos(), door_center_[0]);

    /**
     * @note A鱼三段行径，先接近一号门上方，后接近一号门，最后穿门过到达下区域
     */
    if ((regionPredict(fish_[0].headerPos()) & Region::Lower) || distance < threshold * 0.3) {
        auto goal = CPoint(door_center_[0].x, door_center_[0].y + height_ / 4);
        spinP2PMove(goal, fish_[0], aAction[0]);
    } else if (distance < threshold * 0.6) {
        spinP2PMove(door_center_[0], fish_[0], aAction[0]);
    } else {
        auto goal = CPoint(door_center_[0].x - width_ / 7, door_center_[0].y - height_ / 6);
        spinP2PMove(goal, fish_[0], aAction[0]);
    }

    /**
     * @note B鱼先停留在水池左下角落
     */
    spinP2PMove(CPoint(0, height_), fish_[1], aAction[1]);
}

void CooperativeBallPassing::StageInstruct_2nd(RefArray<CFishAction> aAction) {
    puts("[INFO] 正在执行第二阶段策略");
}

void CooperativeBallPassing::StageInstruct_3rd(RefArray<CFishAction> aAction) {
    puts("[INFO] 正在执行第三阶段策略");
}

bool CooperativeBallPassing::Strategy(
    RefArray<CFishAction> aAction,
    RefArray<CFishInfo>   aFish,
    RefArray<CBallInfo>   aBallinfo,
    RefArray<OBSTAINFO>   aObstacle,
    RefArray<CHANNEL>     aChannel) {
    /**
     * @note 未初始化完成，等待球门标定。
     */
    if (!init(aChannel)) return false;

    /**
     * @note 当标定丢失时，不再信赖视觉识别信息
     * @note 模拟计算行为待实现
     */
    trackAndUpdate(aAction, aFish, aBallinfo);

    switch (phasePredict()) {
        case Phase::Stage_1st:
            StageInstruct_1st(aAction);
        break;
        case Phase::Stage_2nd:
            StageInstruct_2nd(aAction);
        break;
        case Phase::Stage_3rd:
            StageInstruct_3rd(aAction);
        break;
    }

    return true;
}

bool CooperativeBallPassing::init(const RefArray<CHANNEL> &aChannel) {
    /**
     * @note 初始化已经完成
     */
    if (initialized_) return true;

    /**
     * @note 未指定三个球门的坐标，等待标定；
     *  当标定位置大于三时，假定存在错误标定，继续等待。
     */
    if (aChannel.size() != 3) return false;

    /**
     * @note door_center_三个坐标分别对应三个球门中心，
     *  其横坐标应当是递增的，为了防止标定时的错误，此
     *  此处做出额外调整。
     */

    int door_id[3] = {0, 1, 2};
    CPoint refs[3] = {
        aChannel[door_id[0]].center,
        aChannel[door_id[1]].center,
        aChannel[door_id[2]].center,
    };

    if (refs[door_id[0]].x > refs[door_id[1]].x) {
        std::swap(door_id[0], door_id[1]);
    }

    if (refs[door_id[0]].x > refs[door_id[2]].x) {
        std::swap(door_id[0], door_id[2]);
    }

    if (refs[door_id[1]].x > refs[door_id[2]].x) {
        std::swap(door_id[1], door_id[2]);
    }

    door_center_[0] = refs[door_id[0]];
    door_center_[1] = refs[door_id[1]];
    door_center_[2] = refs[door_id[2]];

    return (initialized_ = true);
}

bool CooperativeBallPassing::trackAndUpdate(
    const RefArray<CFishAction> &aAction,
    const RefArray<CFishInfo>   &aFish,
    const RefArray<CBallInfo>   &aBallinfo) {
    /**
     * @brief A、B鱼的编号与水球的编号
     * @note 协作顶球非竞技类项目，鱼可以都归为己队。其中己方
     *  队伍鱼信息索引在{0...7}，索引为i的鱼编号为i+1。鱼的动
     *  作与鱼的信息同索引一一对应。
     * @note 编号为i的水球对应于水球信息列表索引为i-1的位置。
     */

    constexpr int fish_A_id = 1, fish_B_id = 2;
    constexpr int ball_id = 1;
    constexpr auto nilFishPos = CPoint(CFishInfo::nil, CFishInfo::nil);
    constexpr auto nilBallPos = CPoint(-1, -1);

    int fish_index[2] = {-1, -1};
    int ball_index = -1;

    /**
     * @note 校验对水球与鱼的追踪是否有效
     */

    if (aFish.size() > (fish_A_id - 1) &&
        aFish[fish_A_id - 1].centerPos() != nilFishPos) {
        fish_index[0] = fish_A_id - 1;
    }

    if (aFish.size() > (fish_B_id - 1) &&
        aFish[fish_B_id - 1].centerPos() != nilFishPos) {
        fish_index[1] = fish_B_id - 1;
    }

    if (aBallinfo.size() > (ball_id - 1) &&
        aBallinfo[ball_id - 1].getCenter() != nilBallPos) {
        ball_index = ball_id - 1;
    }

    /**
     * @note 更新相关信息
     * @note 若信息处于初始化阶段未被有效获取，则设定为默认值
     */

    if (fish_index[0] != -1) {
        fish_[0] = aFish[fish_index[0]];
    } else if (fish_[0].centerPos() == nilFishPos) {
        fish_[0].centerPos() = CPoint(width_ / 7, height_ * 2 / 6);
    } else {
        // TODO
    }

    if (fish_index[1] != -1) {
        fish_[1] = aFish[fish_index[1]];
    } else if (fish_[1].centerPos() == nilFishPos) {
        fish_[1].centerPos() = CPoint(width_ / 7, height_ * 4 / 6);
    } else {
        // TODO
    }

    if (ball_index != -1) {
        ball_ = aBallinfo[ball_index];
    } else if (ball_.getCenter() == nilBallPos) {
        ball_.setProperty(CPoint(width_ * 2 / 7, height_ * 4 / 6), 128, 13);
    } else {
        // TODO
    }

    return
        fish_index[0] == -1 ||
        fish_index[1] == -1 ||
        ball_index == -1;
}