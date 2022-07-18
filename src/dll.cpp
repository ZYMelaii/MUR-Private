/**
 * @brief 策略导出
 * @author zymelaii
 * @date 2022-06-29
 */

#if 1
#include "./strategy/cooperativeballpassingfsm.h"

BeginExportMURStrategy(OriginImage, RecogImage, aAction, aFish, aBallinfo, aObstacle, aChannel)
    //! 清屏
    static auto CodePageTrigger = system("chcp 65001");
    // clear();
    static CooperativeBallPassingFSM strategy;

    //! 策略运行
    strategy.updateVisualFrame(OriginImage);
    if (bool ignore = strategy.refreshScene(aAction, aFish, aBallinfo, aObstacle, aChannel); !ignore) {
        strategy.Strategy(aAction, aFish, aBallinfo, aObstacle, aChannel);
    }
EndExport()
#else
#include "./strategy/cooperativeballpassing.h"
BeginExportMURStrategy(OriginImage, RecogImage, aAction, aFish, aBallinfo, aObstacle, aChannel)
    //! 清屏
    static auto CodePageTrigger = system("chcp 65001");
    clear();

    //! 策略运行
    static CooperativeBallPassing strategy(OriginImage->width, OriginImage->height);
    strategy.Strategy(aAction, aFish, aBallinfo, aObstacle, aChannel);
EndExport()
#endif