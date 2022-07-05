/**
 * @brief 策略导出
 * @author zymelaii
 * @date 2022-06-29
 */

#include "./official/strategyhelper.h"
#include "./svrpipe.h"

#include "./strategy/statusviewer.h"
#include "./strategy/test_pt2measure.h"
#include "./strategy/smoothtargeting.h"
#include "./strategy/cooperativeballpassing.h"

static int g_enable_which = 1;

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

    static auto strategy = new CooperativeBallPassing(OriginImage->width, OriginImage->height);
    
    wrap(new StatusViewer, true)();
    wrap(new TEST_Pt2Measure, true)();

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

// #include <iostream>
// BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ulReasonForCall, LPVOID lpReserved) {
//     switch (ulReasonForCall) {
//         case DLL_PROCESS_ATTACH: {
//             std::cout << "Load MURStrategy.dll as process" << std::endl;
//             break;
//         }
//         case DLL_PROCESS_DETACH: {
//             std::cout << "free MURStrategy.dll process" << std::endl;
//             break;
//         }
//         case DLL_THREAD_ATTACH: {
//             std::cout << "Load MURStrategy.dll as thread" << std::endl;
//             break;
//         }
//         case DLL_THREAD_DETACH: {
//             std::cout << "free MURStrategy.dll thread" << std::endl;
//             break;
//         }
//     }
//     return TRUE;
// }