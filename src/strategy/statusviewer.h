#ifndef STRATEGY_STATUSVIEWER_H
#define STRATEGY_STATUSVIEWER_H

#include "../official/strategyhelper.h"
#include "../oshelper.h"
#include "../pipelogger.h"

/**
 * @brief 状态显示策略
 * @author zymelaii
 * @date 2022-07-01
 */
class StatusViewer : public CStrategy {
public:
    bool Strategy(
        RefArray<CFishAction> aAction,
        RefArray<CFishInfo> aFish,
        RefArray<CBallInfo> aBallinfo,
        RefArray<OBSTAINFO> aObstacle,
        RefArray<CHANNEL> aChannel) override {

#if 0
        for (int i = 0; i < aFish.size(); ++i) {
            std::cout << aFish[i] << "\n" << aAction[i] << "\n";
        }

        for (int i = 0; i < aBallinfo.size(); ++i) {
            std::cout << "[" << i + 1 << "] " << aBallinfo[i] << "\n";
        }
#else
        std::cout << aFish[0] << "\n" << aAction[0] << "\n";
        std::cout << aFish[1] << "\n" << aAction[1] << "\n";
        std::cout << "[" << 1 << "] " << aBallinfo[0] << "\n";
        std::cout << "[" << 2 << "] " << aBallinfo[1] << "\n";
#endif

        for (int i = 0; i < aChannel.size(); ++i) {
            std::cout << "[" << i + 1 << "] " << aChannel[i] << "\n";
        }

        std::cout << std::flush;

        return true;
    }
};

#endif /*STRATEGY_STATUSVIEWER_H*/