#include "../official/strategy.h"
#include "../official/externrobot.h"
#include "../mathtools.h"
#include <iostream>
#include <vector>

CStrategy::CStrategy() :
    state_period_(15), turn_mode_(0) {
    // PASS
}

void CStrategy::spinP2PMove(const CPoint &goal, CFishInfo &fish, CFishAction &action) {
    CPoint fishpt = fish.GetCenterPoint(); // 鱼身位置
    double fishdir = fish.GetDirection(); // 鱼头方向角
    double distance = getDistance(fishpt, goal); // 计算鱼身和目标点的距离
    double direction = normalizeAngle(getVecAngle(fishpt, goal) - fishdir) / M_PI * 180; /// -180 <= direction <= 180

    fish.SetAction(action);

    /**
     * @note 以下是一坨调参，分别对应远距离运动与近距离运动方向决策
     * @note 在某种意义上，下列参数是可靠的，但是为了获得更好的效果
     *  可以尝试对系列参数进行调参。
     * @author zymelaii
     * @date 2022-06-26
     */

    /**
     * @note 调优参数是一组“角度范围-控制方向档位”的二元组。
     * @note 调整参数时请务必确保角度范围的并集为[-180,180]，
     *  以免出现移动死角。
     */
    using minmax_t = std::pair<int, int>;
    using RangeDirectionPair = std::pair<minmax_t, int>;

    const std::vector<RangeDirectionPair> remoteMap {
        {{-5, 5}, 7}, {{-10, -5}, 5}, {{-30, -10}, 4}, {{-50, -30}, 2},
        {{-70, -50}, 1}, {{-90, -70}, 0}, {{-180, -90}, 0}, {{5, 10}, 10},
        {{10, 20}, 11}, {{20, 40}, 13}, {{40, 50}, 13}, {{50, 80}, 14},
        {{80, 90}, 14}, {{90, 180}, 14},
    };
    const std::vector<RangeDirectionPair> nearMap {
        {{-7,  7}, 7}, {{-20, -7}, 4}, {{-40, -20}, 1}, {{-60, -40}, 0},
        {{-70, -60}, 0}, {{-90, -70}, 0}, {{-180, -90}, 0}, {{7, 20}, 10},
        {{20, 30}, 12}, {{30, 50}, 13}, {{50, 70}, 14}, {{70, 90}, 14},
        {{90, 180}, 14},
    };

    const int distanceThreshold = 40; // 划分远近距离的阈值
    const std::vector<RangeDirectionPair> &RDMap = distance > distanceThreshold ? remoteMap : nearMap;

    for (const auto &item : RDMap) {
        auto &range = item.first;
        if (direction > range.first && direction <= range.second) {
            action.direction = item.second;
            break;
        }
    }

    fish.SetAction(action);
}

void CStrategy::spinP2PMoveFromLeft(const CPoint &goal, CFishInfo &fish, CFishAction &action) {
    CPoint fishpt = fish.GetCenterPoint(); // 鱼身位置
    double fishdir = fish.GetDirection(); // 鱼头方向角
    double distance = getDistance(fishpt, goal); // 计算鱼身和目标点的距离
    double direction = normalizeAngle(getVecAngle(fishpt, goal) - fishdir) / M_PI * 180; /// -180 <= direction <= 180

    fish.SetAction(action);

    using minmax_t = std::pair<int, int>;
    using RangeDirectionPair = std::pair<minmax_t, int>;

    const std::vector<RangeDirectionPair> remoteMap {
        {{-5, 5}, 7}, {{-10, -5}, 5}, {{-30, -10}, 4}, {{-50, -30}, 0},
        {{-70, -50}, 0}, {{-90, -70}, 0}, {{-180, -90}, 0}, {{5, 10}, 0},
        {{10, 20}, 0}, {{20, 40}, 0}, {{40, 50}, 0}, {{50, 80}, 0},
        {{80, 90}, 0}, {{90, 180}, 0},
    };
    const std::vector<RangeDirectionPair> nearMap {
        {{-5, 5}, 7}, {{-20, -5}, 4}, {{-40, -20}, 1}, {{-60, -40}, 0},
        {{-70, -60}, 0}, {{-90, -70}, 0}, {{-180, -90}, 0}, {{5, 20}, 0},
        {{20, 30}, 0}, {{30, 50}, 0}, {{50, 70}, 0}, {{70, 90}, 0},
        {{90, 120}, 0}, {{120, 180}, 0},
    };

    const int distanceThreshold = 100; // 划分远近距离的阈值
    const std::vector<RangeDirectionPair> &RDMap = distance > distanceThreshold ? remoteMap : nearMap;

    for (const auto &item : RDMap) {
        auto &range = item.first;
        if (direction > range.first && direction <= range.second) {
            action.direction = item.second;
            break;
        }
    }

    fish.SetAction(action);
}

void CStrategy::spinP2PMoveFromRight(const CPoint &goal, CFishInfo &fish, CFishAction &action) {
    CPoint fishpt = fish.GetCenterPoint(); // 鱼身位置
    double fishdir = fish.GetDirection(); // 鱼头方向角
    double distance = getDistance(fishpt, goal); // 计算鱼身和目标点的距离
    double direction = normalizeAngle(getVecAngle(fishpt, goal) - fishdir) / M_PI * 180; /// -180 <= direction <= 180

    fish.SetAction(action);

    using minmax_t = std::pair<int, int>;
    using RangeDirectionPair = std::pair<minmax_t, int>;

    const std::vector<RangeDirectionPair> remoteMap {
        {{-5, 5}, 7}, {{-10, -5}, 5}, {{-30, -10}, 14}, {{-50, -30}, 14},
        {{-70, -50}, 14}, {{-90, -70}, 14}, {{-180, -90}, 14}, {{5, 10}, 9},
        {{10, 20}, 10}, {{20, 40}, 12}, {{40, 50}, 12}, {{50, 80}, 14},
        {{80, 90}, 14}, {{90, 180}, 14},
    };
    const std::vector<RangeDirectionPair> nearMap {
        {{-5, 5}, 7}, {{-20, -5}, 14}, {{-40, -20}, 14}, {{-60, -40}, 14},
        {{-70, -60}, 14}, {{-90, -70}, 14}, {{-180, -120}, 14}, {{5, 20}, 10},
        {{20, 30}, 12}, {{30, 50}, 13}, {{50, 70}, 14}, {{70, 90}, 14},
        {{90, 120}, 14}, {{120, 180}, 14},
    };

    const int distanceThreshold = 100; // 划分远近距离的阈值
    const std::vector<RangeDirectionPair> &RDMap = distance > distanceThreshold ? remoteMap : nearMap;

    for (const auto &item : RDMap) {
        auto &range = item.first;
        if (direction > range.first && direction <= range.second) {
            action.direction = item.second;
            break;
        }
    }

    fish.SetAction(action);
}

bool CStrategy::P2PMove(const CPoint &goal, CFishInfo &fish, CFishAction &action) {
    CPoint fishpt = fish.GetCenterPoint(); // 鱼身位置
    double fishdir = fish.GetDirection(); // 鱼头方向角
    double distance = getDistance(fishpt, goal); // 计算鱼身和目标点的距离
    double direction = normalizeAngle(getVecAngle(fishpt, goal) - fishdir) / M_PI * 180; /// -180 <= direction <= 180

    spinP2PMove(goal, fish, action);

    /**
     * @note 该阈值的意义并不是那么明确，总之先不要动它
     */
    const int distanceThreshold = 30;
    return getDistance(fishpt, goal) < distanceThreshold;
}

void CStrategy::stop(RefArray<CFishInfo> &aFish, RefArray<CFishAction> &aAction, int id) {
    aAction[id].speed = 0;
    aAction[id].direction = 7;
    aAction[id].mode = 0;

    if (++aAction[id].state >= state_period_) {
        aAction[id].state = 0;
        turn_mode_ = 0;
    }

    aFish[id].SetAction(aAction[id]);
}

void CStrategy::go(RefArray<CFishInfo> &aFish, RefArray<CFishAction> &aAction,
    int id, int speed, int direction, int mode) {

    aAction[id].speed = speed;
    aAction[id].direction = direction;
    aAction[id].mode = mode;

    if (turn_mode_) { // 正在执行原始转弯模式
        if (++aAction[id].state >= state_period_) {
            aAction[id].state = 0;
            turn_mode_ = 0;
        }
    } else if (mode) {
        aAction[id].state = 0;
        turn_mode_ = mode;
    } if (++aAction[id].state >= state_period_) {
        aAction[id].state = 0;
    }

    aFish[id].SetAction(aAction[id]);
}