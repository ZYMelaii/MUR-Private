#include "cooperativeballpassing.h"
#include <algorithm>
#include <bitset>
#include <malloc.h>
#include <float.h>


CooperativeBallPassing::CooperativeBallPassing(int width, int height) :
    initialized_(false), width_(width), height_(height), image_(nullptr), stage_(Phase::UnStaged) {
    /**
     * @note 相关信息初始皆被设定为可判定的无效量，下面给出数据无效的判定：
     *  fish_[*].centerPos() == CPoint(CFishInfo::nil, CFishInfo::nil)
     *  fish_[*].centerPos() == CPoint(-1, -1)
     *  ball_.getCenter() == CPoint(-1, -1)
     *  door_center_[*] == CPoint(0, 0)
     */
}

void CooperativeBallPassing::stableHoverInstruct(CFishInfo &fish, CFishAction &action) {
    if (!initialized_) return;
    action.speed = 1;
    action.direction = 12;
    fish.updateAction(action);
}

void CooperativeBallPassing::bevelMove(const CPoint &goal, CFishInfo &fish, CFishAction &action) {
    // ! 默认先按在运动方向左侧处理
    const auto delta = normalizeAngle(
        getVecAngle(fish.centerPos(), fish.headerPos()) -
        getVecAngle(fish.centerPos(), goal)) / M_PI * 180;
    const auto distance = getDistance(goal, fish.headerPos());

    if (delta < 15) {
        action.direction = 7;
        action.speed = 7;
        return;
    }

    // TODO

    action.direction = 9;
}

void CooperativeBallPassing::trivalMove(
    const CPoint &goal, CFishInfo &fish, CFishAction &action, int speed_gear) {

    const int r = 8;
    for (int row = goal.y - r; row <= goal.y + r; ++row) {
        for (int col = goal.x - r; col <= goal.x + r; ++col) {
            auto data = reinterpret_cast<uint8_t*>(&image_->imageData[row * image_->widthStep + col * image_->nChannels]);
            data[0] = 0x00;
            data[1] = 0x00;
            data[2] = 0xff;
        }
    }

    fish.setTarget(goal, normalizeAngle(getVecAngle(fish.headerPos(), goal)));
    action.speed = speed_gear == -1 ? action.speed : speed_gear;
    spinP2PMove(fish.targetPos(), fish, action);
}

CooperativeBallPassing::Region CooperativeBallPassing::regionPredict(const CPoint &pos) const {
    int flag = Region::Empty;
    flag |= pos.y <= height_ / 2 ? Region::Upper : Region::Lower;
    flag |= pos.x <= width_ / 3 ? Region::Left : (pos.x >= width_ * 2 / 3 ? Region::Right : Region::Middle);
    return static_cast<Region>(flag);
}

void CooperativeBallPassing::installImage(IplImage *image) {
    image_ = image;
}

CooperativeBallPassing::Phase CooperativeBallPassing::phasePredict(
    const CPoint &a_pos, const CPoint &b_pos, const CPoint &ball_pos) const {

    const auto reg_1 = regionPredict(a_pos);
    const auto reg_2 = regionPredict(b_pos);

    std::cout << "[INFO] A鱼所处区域：bin(" << std::bitset<8>(reg_1) << ")\n";
    std::cout << "[INFO] B鱼所处区域：bin(" << std::bitset<8>(reg_2) << ")\n";
    std::cout << std::flush;

    if ((reg_1 & Region::Left) && reg_2 == Region::LL) {
        return Phase::Stage_1st;
    } else if (reg_1 == Region::UR && !(reg_2 & Region::Left)) {
        return Phase::Stage_3rd;
    } else {
        return Phase::Stage_2nd;
    }
}

void CooperativeBallPassing::StageInstruct_1st(
    CFishInfo &fish_a, CFishAction &action_a,
    CFishInfo &fish_b, CFishAction &action_b,
    CBallInfo &ball) {
    puts("[INFO] 正在执行第一阶段策略");

    const auto threshold = getDistance(door_center_[0], CPoint(width_ / 7, height_ * 2 / 6));
    const auto distance = getDistance(fish_a.headerPos(), door_center_[0]);

    /**
     * @note A鱼三段行径，先接近一号门上方，后接近一号门，最后穿门过到达下区域
     */
    if ((regionPredict(fish_a.headerPos()) & Region::Lower) || distance < threshold * 0.3) {
        auto goal = CPoint(door_center_[0].x, door_center_[0].y + height_ / 4);
        trivalMove(goal, fish_a, action_a, 7);
    } else if (distance < threshold) {
        auto goal = CPoint(door_center_[0].x, door_center_[0].y - height_ / 6);
        fish_a.setTarget(goal, normalizeAngle(getVecAngle(fish_a.headerPos(), goal)));
        trivalMove(goal, fish_a, action_a, 6);
    } else {
        auto goal = CPoint(door_center_[0].x - width_ / 7, door_center_[0].y - height_ / 4);
        trivalMove(goal, fish_a, action_a, 5);
    }

    /**
     * @note B鱼先停留在水池左下角落
     */
    stableHoverInstruct(fish_b, action_b);
}

void CooperativeBallPassing::StageInstruct_2nd(
    CFishInfo &fish_a, CFishAction &action_a,
    CFishInfo &fish_b, CFishAction &action_b,
    CBallInfo &ball) {
    puts("[INFO] 正在执行第二阶段策略");

    const auto threshold = getDistance(door_center_[0], CPoint(width_ / 7, height_ * 2 / 6));
    const auto reg_1 = regionPredict(fish_a.centerPos());
    const auto reg_2 = regionPredict(fish_b.centerPos());

    switch (reg_1) {
        case Region::LM: {
            puts("[INFO] A鱼正按规划的二段路线正常执行穿越二门动作");
            const auto distance = getDistance(fish_a.centerPos(), door_center_[1]);
            const auto goal = distance < threshold * 1.2 ?
                CPoint(door_center_[1].x, door_center_[1].y - height_ * 2 / 6) :
                CPoint(door_center_[1].x - width_ / 7, door_center_[1].y + height_ / 6);
            trivalMove(goal, fish_a, action_a, 5);
            break;
        }
        case Region::UM: [[fallthrough]];
        case Region::UR: {
            puts("[INFO] A鱼正在穿越二门，即将前往水池右上角待机");
            const auto distance = getDistance(fish_a.headerPos(), door_center_[1]);
            if (reg_1 == Region::UR && distance < threshold * 1.6 || distance > threshold) {
                const auto goal = CPoint(width_, 0);
                trivalMove(goal, fish_a, action_a, 7);
            } else {
                stableHoverInstruct(fish_a, action_a);
            }
            break;
        }
        case Region::LR: {
            puts("[INFO] 检测到A鱼越过二门，从右侧重新导航");
            const auto distance = getDistance(fish_a.centerPos(), door_center_[1]);
            const auto goal = distance < threshold * 1.2 ?
                CPoint(door_center_[1].x, door_center_[1].y - height_ * 2 / 6) :
                CPoint(door_center_[1].x + width_ / 7, door_center_[1].y + height_ / 6);
            trivalMove(goal, fish_a, action_a, 6);
            break;
        }
        default: {
            puts("[INFO] 检测到A鱼偏航，正加速回程");
            const auto goal = CPoint(
                static_cast<decltype(CPoint().x)>(door_center_[1].x * 0.9 + door_center_[0].x * 0.1),
                static_cast<decltype(CPoint().y)>(door_center_[1].y * 0.9 + door_center_[0].y * 0.1));
            trivalMove(goal, fish_a, action_a, 8);
        }
    }

    puts("[WARN] 当前B鱼忽略水球执行穿越动作");

    const auto dist_head2center = getDistance(fish_b.headerPos(), fish_b.centerPos());
    if ((reg_2 & Region::Lower) && !(reg_2 & Region::Right) ||
        (reg_2 & Region::Upper) && getDistance(fish_b.headerPos(), CPoint(door_center_[0])) < dist_head2center) {
        /**
        * @note B鱼一号门穿越三段行径，同A鱼第一过程
        */
        puts("[INFO] B鱼正执行穿越一门动作");
        const auto distance = getDistance(fish_b.centerPos(), door_center_[0]);
        if ((regionPredict(fish_b.headerPos()) & Region::Upper) || distance < threshold * 0.3) {
            auto goal = CPoint(door_center_[0].x, door_center_[0].y - height_ / 4);
            fish_b.setTarget(goal, normalizeAngle(getVecAngle(fish_b.headerPos(), goal)));
            action_b.speed = 7;
            spinP2PMove(fish_b.targetPos(), fish_b, action_b);
        } else if (distance < threshold * 0.6) {
            auto goal = door_center_[0];
            fish_b.setTarget(goal, normalizeAngle(getVecAngle(fish_b.headerPos(), goal)));
            action_b.speed = 6;
            spinP2PMove(fish_b.targetPos(), fish_b, action_b);
        } else {
            auto goal = CPoint(door_center_[0].x - width_ / 7, door_center_[0].y + height_ / 6);
            fish_b.setTarget(goal, normalizeAngle(getVecAngle(fish_b.headerPos(), goal)));
            action_b.speed = 5;
            spinP2PMove(fish_b.targetPos(), fish_b, action_b);
        }
    } else {
        /**
         * @note B鱼已经穿越一门，为了更好的顶球表现，B鱼将（略偏下侧）平行前往门二
         */
        puts("[INFO] B鱼正平行驱往二门");
        const auto goal = CPoint(door_center_[1].x, fish_b.headerPos().y);
        trivalMove(goal, fish_b, action_b, 6);
    }
}

void CooperativeBallPassing::StageInstruct_3rd(
    CFishInfo &fish_a, CFishAction &action_a,
    CFishInfo &fish_b, CFishAction &action_b,
    CBallInfo &ball) {
    puts("[INFO] 正在执行第三阶段策略");

    const auto threshold = getDistance(door_center_[0], CPoint(width_ / 7, height_ * 2 / 6));
    auto region = regionPredict(fish_b.centerPos());

    if (region & Region::Upper) {
        puts("[INFO] B鱼正在穿越二门");
        const auto distance = getDistance(fish_b.headerPos(), door_center_[1]);
        const auto goal = CPoint(door_center_[1].x, distance > threshold * 0.25 ? door_center_[1].y : door_center_[2].y);
        trivalMove(goal, fish_b, action_b, 5);
    } else {
        puts("[INFO] B鱼正在进攻二门");
        const auto goal = door_center_[2];
        trivalMove(goal, fish_b, action_b, 4);

        // TODO
    }
}

bool CooperativeBallPassing::Strategy(
    RefArray<CFishAction> aAction,
    RefArray<CFishInfo>   aFish,
    RefArray<CBallInfo>   aBallinfo,
    RefArray<OBSTAINFO>   aObstacle,
    RefArray<CHANNEL>     aChannel) {

    const auto threshold = getDistance(door_center_[0], CPoint(width_ / 7, height_ * 2 / 6));
    /**
     * @note 未初始化完成，等待球门标定。
     */
    if (!init(aFish, aBallinfo, aChannel)) return false;

    auto &fish_a = aFish[index_[0]];
    auto &fish_b = aFish[index_[1]];
    auto &action_a = aAction[index_[0]];
    auto &action_b = aAction[index_[1]];
    auto &ball = aBallinfo[index_[2]];

    /**
     * @note 当标定丢失时，不再信赖视觉识别信息
     * @note 模拟计算行为待实现
     */
    trackAndUpdate(fish_a, action_a, fish_b, action_b, ball);

    switch (phasePredict(fish_a.centerPos(), fish_b.centerPos(), ball.getCenter())) {
        case Phase::Stage_1st:
            StageInstruct_1st(fish_a, action_a, fish_b, action_b, ball);
        break;
        case Phase::Stage_2nd:
            StageInstruct_2nd(fish_a, action_a, fish_b, action_b, ball);
        break;
        case Phase::Stage_3rd:
            StageInstruct_3rd(fish_a, action_a, fish_b, action_b, ball);
        break;
        default: break;
    }

    return true;
}

bool CooperativeBallPassing::init(
    const RefArray<CFishInfo> &aFish,
    const RefArray<CBallInfo> &aBallinfo,
    const RefArray<CHANNEL>   &aChannel) {
    /**
     * @note 初始化已经完成
     */
    if (initialized_) return true;

    /**
     * @note 等待鱼和水球标定完成
     */
    if (aFish[index_[0]].centerPos() == CPoint(-1, -1)) return false;
    if (aFish[index_[1]].centerPos() == CPoint(-1, -1)) return false;
    if (aBallinfo[index_[2]].getCenter() == CPoint(-1, -1)) return false;

    /**
     * @note 未指定三个球门的坐标，等待标定；
     *  当标定位置大于三时，假定存在错误标定，继续等待。
     */
    if (aChannel.size() != 3) return false;

    /**
     * @note door_center_三个坐标分别对应三个球门中心，
     *  其横坐标应当是递增的，为了防止标定时的错误，此
     *  此处做出额外调整。
     * @note 参考坐标：(220,236)、(474,240)、(600,280)
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
    CFishInfo &fish_a, CFishAction &action_a,
    CFishInfo &fish_b, CFishAction &action_b,
    CBallInfo &ball) {
    /**
     * @brief A、B鱼的编号与水球的编号
     * @note 协作顶球非竞技类项目，鱼可以都归为己队。其中己方
     *  队伍鱼信息索引在{0...7}，索引为i的鱼编号为i+1。鱼的动
     *  作与鱼的信息同索引一一对应。
     * @note 编号为i的水球对应于水球信息列表索引为i-1的位置。
     */

    /**
     * @note 校验对水球与鱼的追踪是否有效
     */

    // TODO

    /**
     * @note 更新相关信息
     * @note 若信息处于初始化阶段未被有效获取，则设定为默认值
     */

    // TODO

    return true;
}