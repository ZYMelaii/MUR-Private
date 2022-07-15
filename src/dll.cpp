/**
 * @brief 策略导出
 * @author zymelaii
 * @date 2022-06-29
 */

#include "./official/strategyhelper.h"
#include "./strategy/smoothtargeting.h"
#include "./strategy/cooperativeballpassing.h"

static int g_enable_which = 1;

BeginExportMURStrategy(OriginImage, RecogImage, aAction, aFish, aBallinfo, aObstacle, aChannel)
    static auto CodePageTrigger = system("chcp 65001");

    clear();

    auto wrap = [&](CStrategy *self, bool deleteLater = false) {
        return [&, self, deleteLater] {
            self->Strategy(aAction, aFish, aBallinfo, aObstacle, aChannel);
            if (deleteLater) {
                delete self;
            }
        };
    };

    static auto strategy = new CooperativeBallPassing(
        OriginImage->width, OriginImage->height, OriginImage);

    switch (g_enable_which) {
        case 0:
            puts("[INFO] 当前策略：SmoothTargeting");
            wrap(new LittleLittleFishDashTargetBallToDeathAndSlowDownSmoothlyAsDoveStrategy, true)();
        break;
        case 1:
            puts("[INFO] 当前策略：CooperativeBallPassing");
            wrap(strategy)();
        break;
    }

EndExport()