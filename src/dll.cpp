/**
 * @brief 策略导出
 * @author zymelaii
 * @date 2022-06-29
 */

#include "./official/strategyhelper.h"
#include "./pipelogger.h"

#include "./strategy/v1.h"
#include "./strategy/statusviewer.h"
#include "./strategy/test_pt2measure.h"
#include "./strategy/smoothtargeting.h"

BeginExportMURStrategy(OriginImage, RecogImage, aAction, aFish, aBallinfo, aObstacle, aChannel)
    PipeLogger("SvrLogger").submit("clear");

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
    wrap(new LittleLittleFishDashTargetBallToDeathAndSlowDownSmoothlyAsDoveStrategy, true)();
EndExport()

#include <iostream>
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ulReasonForCall, LPVOID lpReserved) {
    switch (ulReasonForCall) {
        case DLL_PROCESS_ATTACH: {
            std::cout << "Load MURStrategy.dll as process" << std::endl;
            break;
        }
        case DLL_PROCESS_DETACH: {
            std::cout << "free MURStrategy.dll process" << std::endl;
            break;
        }
        case DLL_THREAD_ATTACH: {
            std::cout << "Load MURStrategy.dll as thread" << std::endl;
            break;
        }
        case DLL_THREAD_DETACH: {
            std::cout << "free MURStrategy.dll thread" << std::endl;
            break;
        }
    }
    return TRUE;
}