/**
 * @note 示例程序
 * @author zymelaii
 * @date 2022-06-28
 **
 * /// 策略导出辅助头文件
 * #include "./official/strategyhelper.h"
 *
 * /// 策略代码
 * class YourStrategy : public CStrategy {
 * public:
 * 	virtual bool Strategy(
 * 		RefArray<CFishAction> aAction,
 * 		RefArray<CFishInfo>   aFish,
 * 		RefArray<CBallInfo>   aBallinfo,
 * 		RefArray<OBSTAINFO>   aObstacle,
 * 		RefArray<CHANNEL>     aChannel) {
 * 		/// 在这里编写策略函数
 * 		/// 不要更改函数名，可以增加成员函数、成员变量辅助策略编写
 * 		/// 其中可以像使用数组一样使用给出的参数，可通过成员函数size()获取长度
 * 		/// 详情见refarray.h
 * 		return true;
 * 	}
 * };
 *
 * /// 导出策略
 * /// 通过ExportStategy导出的策略是静态策略，策略对象将一次初始化，持久保存
 * /// 单个项目中只能导出一个策略
 * ExportStategy(YourStrategy);
 **
 */

#include "./official/strategyhelper.h"

#include "./strategy/v1.h"
#include <iostream>

BeginExportMURStrategy(OriginImage, RecogImage, aAction, aFish, aBallinfo, aObstacle, aChannel)
    puts("[INFO] entry MURStrategy");
    printf(
        "[STATUS]\n"
        "\tOriginImage: 0x%p\n"
        "\tRecogImage: 0x%p\n"
        "\taAction: size()=%d\n"
        "\taFish: size()=%d\n"
        "\taBallinfo: size()=%d\n"
        "\taObstacle: size()=%d\n"
        "\taChannel: size()=%d\n",
        OriginImage,
        RecogImage,
        aAction.size(),
        aFish.size(),
        aBallinfo.size(),
        aObstacle.size(),
        aChannel.size()
    );
    
    static auto *strategy = new StrategyCloneV1;

    puts("[INFO] begin running StrategyCloneV1");
    strategy->Strategy(aAction, aFish, aBallinfo, aObstacle, aChannel);
    puts("[INFO] finished StrategyCloneV1::Strategy");
EndExport()