# MUR-Private
#### 导出策略示例
```c++
/// 策略导出辅助头文件
#include "./official/strategyhelper.h"

/// 策略代码
class YourStrategy : public CStrategy {
public:
	virtual bool Strategy(
		RefArray<CFishAction> aAction,
		RefArray<CFishInfo>   aFish,
		RefArray<CBallInfo>   aBallinfo,
		RefArray<OBSTAINFO>   aObstacle,
		RefArray<CHANNEL>     aChannel) {
		/// 在这里编写策略函数
		/// 不要更改函数名，可以增加成员函数、成员变量辅助策略编写
		/// 其中可以像使用数组一样使用给出的参数，可通过成员函数size()获取长度
		/// 详情见refarray.h
	}
};

/// 导出策略
/// 通过ExportStrategy导出的策略是静态策略，策略对象将一次初始化，持久保存
/// 单个项目中只能导出一个策略
ExportStrategy(YourStrategy);
```
导出策略的第二种方式
```c++
/// 替换ExportStrategy，其他部分不变
/// 依次定义原始图像、识别图像、动作列表、鱼列表、水球列表、障碍物列表、通道列表的变量名
/// 图像的结构详见common.h，剩余的皆为RefArray同上
/// 使用BeginExportMURStrategy/EndExport以便更灵活的编写代码
BeginExportMURStrategy(OriginImage, RecogImage, aAction, aFish, aBallinfo, aObstacle, aChannel)
	/// 参考流程
	/// 1. 策略开始前的准备
	/// 2. 构造策略对象并执行策略
	/// 3. 本次策略执行完毕后的收尾工作
EndExport()
```