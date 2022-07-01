/**
 * @brief 姘翠腑鍗忎綔椤剁悆绛栫暐瀵煎嚭
 * @author zymelaii
 * @date 2022-06-29
 */

#include "./official/strategyhelper.h"

#include "./strategy/v1.h"
#include "./strategy/statusviewer.h"
#include "./strategy/test_pt2measure.h"
#include "./strategy/smoothtargeting.h"

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