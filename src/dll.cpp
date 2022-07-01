/**
 * @brief ˮ��Э��������Ե���
 * @author zymelaii
 * @date 2022-06-29
 */

#define _CRT_SECURE_NO_WARNINGS

#include "./mathtools.h"
#include "./winhelper.h"
#include "./oshelper.h"

#include "./official/strategyhelper.h"
#include "./strategy/v1.h"
#include "./strategy/statusviewer.h"
#include "./strategy/test_pt2measure.h"

#include <cstdlib>
#include <iostream>
#include <thread>
#include <queue>
#include <ctime>
#include <cstdlib>
#include <functional>

class LittleLittleFishDashTargetBallToDeathAndSlowDownSmoothlyAsDoveStrategy : public CStrategy {
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
            action.speed = std::max(action.speed, 10);
            fish.updateAction(action);
            fish.update(fish.centerPos(), fish.currentDirection(), 1);
            init = false;
        }

        std::cout << __LINE__ << "# A��ִ�ж�������ˮ����һ�����Ϸ�100��λ���봦" << std::endl;

        //! ��������
        const auto goal = CPoint(aChannel[0].center.x, aChannel[0].center.y - 100);
        auto hitPoint = ball.calcHitPoint(goal);
        std::cout << __LINE__ << "# ��������㣺" << hitPoint
            << "��������" << normalizeAngle(getVecAngle(ball.getCenter(), goal)) / M_PI * 180 << "deg" << std::endl;

        //! ����λ�Ʋ��Сʱ������Ԥ���㱻�赲������
        if (getDistance(fish.centerPos(), fish.lastCenterPos()) < 5 && rand() % 16 == 1) {
            //! �˶�����ķ����������п��ܰ��ѿ���״̬�ķ���
            auto vec = normalizeAngle(getVecAngle(fish.lastCenterPos(), fish.centerPos()) + M_PI / 2);
            const auto distance = getDistance(fish.centerPos(), ball.getCenter());
            /// ����λ�ơ�[10, 20)
            const auto perp_pulse = 10 * (1 + atan(distance - 2) / M_PI);
            std::cout << __LINE__ << "# ̽�⵽A����뿨��״̬����������Ŷ�����λ��" << perp_pulse << "cm" << std::endl;

            //! ��Ŀ���Ϊ������λ�ƣ���������
            // hitPoint.x += perp_pulse * cos(vec);
            // hitPoint.y += perp_pulse * sin(vec);

            //! ��������ƫ����β������Ϊ������λ��
            hitPoint.x = fish.centerPos().x + perp_pulse * cos(vec) - (fish.headerPos().x - fish.centerPos().x) * 2;
            hitPoint.y = fish.centerPos().y + perp_pulse * sin(vec) - (fish.headerPos().y - fish.centerPos().y) * 2;
        }

        //! �趨Ŀ���
        fish.setTarget(hitPoint, normalizeAngle(getVecAngle(fish.headerPos(), hitPoint)));

        //! ����Ŀ�����ƽ����һ���˶��ٶ�
        const auto distance = getDistance(fish.headerPos(), fish.targetPos());
        action.speed = static_cast<int>((1 + atan(distance / 30) / M_PI) * 5);

        //! �����㵽���ƶ�ָ��
        spinP2PMove(fish.targetPos(), fish, action);

        return true;
    }
};

BeginExportMURStrategy(OriginImage, RecogImage, aAction, aFish, aBallinfo, aObstacle, aChannel)
    clear();

    auto wrap = [&](CStrategy *self, bool deleteLater = false) {
        return [&] {
            self->Strategy(aAction, aFish, aBallinfo, aObstacle, aChannel);
            if (deleteLater) {
                delete self;
            }
        };
    };

    wrap(new StatusViewer, true)();
    wrap(new TEST_Pt2Measure, true)();
    wrap(new LittleLittleFishDashTargetBallToDeathAndSlowDownSmoothlyAsDoveStrategy)();

EndExport()