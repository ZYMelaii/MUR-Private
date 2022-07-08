#ifndef STRATEGY_SMOOTHTARGETING_H
#define STRATEGY_SMOOTHTARGETING_H

#include "../official/strategyhelper.h"

#define MAX_FN_ONLY_FOR_CURRENT_FILE(x, y) (((x)>(y))?(x):(y))

class SmoothTargeting : public CStrategy {
public:
    virtual bool Strategy(
        RefArray<CFishAction> aAction,
        RefArray<CFishInfo> aFish,
        RefArray<CBallInfo> aBallinfo,
        RefArray<OBSTAINFO> aObstacle,
        RefArray<CHANNEL> aChannel) override {

        auto& ball = aBallinfo[0];
        auto& fish = aFish[0];
        auto& action = aAction[0];

        static bool init = true;
        if (init) {
            action.speed = MAX_FN_ONLY_FOR_CURRENT_FILE(action.speed, 10);
            fish.updateAction(action);
            fish.update(fish.centerPos(), fish.currentDirection(), 1);
            init = false;
        }

        std::cout << __LINE__ << "# A鱼执行动作：将水球顶至一号门上方100单位距离处" << std::endl;

        //! 计算击球点
        const auto goal = CPoint(aChannel[0].center.x, aChannel[0].center.y - 100);
        auto hitPoint = ball.calcHitPoint(goal);
        std::cout << __LINE__ << "# 期望击球点：" << hitPoint
            << "，击球方向：" << normalizeAngle(getVecAngle(ball.getCenter(), goal)) / M_PI * 180 << "deg" << std::endl;

        //! 当鱼位移差过小时，可以预测鱼被阻挡而卡死
        if (getDistance(fish.centerPos(), fish.lastCenterPos()) < 5 && rand() % 16 == 1) {
            //! 运动方向的法向方向是最有可能摆脱卡死状态的方向
            auto vec = normalizeAngle(getVecAngle(fish.lastCenterPos(), fish.centerPos()) + M_PI / 2);
            const auto distance = getDistance(fish.centerPos(), ball.getCenter());
            /// 法向位移∈[10, 20)
            const auto perp_pulse = 10 * (1 + atan(distance - 2) / M_PI);
            std::cout << __LINE__ << "# 探测到A鱼介入卡死状态，引入随机扰动法向位移" << perp_pulse << "cm" << std::endl;

            //! 以目标点为基法向位移（已舍弃）
            // hitPoint.x += perp_pulse * cos(vec);
            // hitPoint.y += perp_pulse * sin(vec);

            //! 以鱼中心偏向鱼尾的坐标为基法向位移
            hitPoint.x = static_cast<decltype(hitPoint.x)>(
                fish.centerPos().x + perp_pulse * cos(vec) - (fish.headerPos().x - fish.centerPos().x) * 2);
            hitPoint.y = static_cast<decltype(hitPoint.y)>(
                fish.centerPos().y + perp_pulse * sin(vec) - (fish.headerPos().y - fish.centerPos().y) * 2);
        }

        //! 设定目标点
        fish.setTarget(hitPoint, normalizeAngle(getVecAngle(fish.headerPos(), hitPoint)));

        //! 根据目标距离平滑归一化运动速度
        const auto distance = getDistance(fish.headerPos(), fish.targetPos());
        action.speed = static_cast<int>((1 + atan(distance / 30) / M_PI) * 5);

        //! 发出点到点移动指令
        spinP2PMove(fish.targetPos(), fish, action);

        return true;
    }
};

using LittleLittleFishDashTargetBallToDeathAndSlowDownSmoothlyAsDoveStrategy = SmoothTargeting;

#undef MAX_FN_ONLY_FOR_CURRENT_FILE

#endif /*STRATEGY_SMOOTHTARGETING_H*/