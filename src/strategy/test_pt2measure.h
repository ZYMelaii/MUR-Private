#ifndef STRATEGY_TEST_PT2MEASURE_H
#define STRATEGY_TEST_PT2MEASURE_H

#include "../official/strategyhelper.h"
#include "../mathtools.h"
#include "../oshelper.h"

/**
 * @brief 测试用策略，输出两点之间的相关测算参数
 * @note 第一个点和第二个点分别由标号一和标号二的两个水球指定
 * @author zymelaii
 * @date 2022-07-01
 */
class TEST_Pt2Measure : public CStrategy {
public:
    bool Strategy(
        RefArray<CFishAction> aAction,
        RefArray<CFishInfo> aFish,
        RefArray<CBallInfo> aBallinfo,
        RefArray<OBSTAINFO> aObstacle,
        RefArray<CHANNEL> aChannel) override {

        if (aBallinfo.size() < 2) return false;

        auto A = aBallinfo[0].getCenter();
        auto B = aBallinfo[1].getCenter();

        const auto T = B - A;
        const auto distance = getDistance(A, B);
        const auto angle = getVecAngle(A, B) / M_PI * 180;

        std::cout <<
            "[TESTCASE] Information of Measure and Calculation {" << "\n" <<
            "    { alias ball[1] } A: " << A << "\n" <<
            "    { alias ball[2] } B: " << B << "\n" <<
            "    { alias vector(A -> B) } T: " << T << "\n" <<
            "    distance(A, B): " << distance << "cm\n" <<
            "    angle(T): " << angle << "deg\n" <<
            "}\n";

        std::cout << std::flush;

        return true;
    }
};

#endif /*STRATEGY_TEST_PT2MEASURE_H*/