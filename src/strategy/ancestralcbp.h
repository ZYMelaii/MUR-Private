#include "../official/strategyhelper.h"

/**
 * @brief 水中协作顶球祖传策略
 * @note 未调试完全，可维护性极低，已废弃
 */
class AncestralCBP : public CStrategy {
public:
	void movingshot1(int mode, CPoint aimpt, CFishAction &action, int fishID, CFishInfo m_FishInfo, CPoint doorpt, CPoint shotpt0);
	void movingshot2(int mode, CPoint aimpt, CFishAction &action, int fishID, CFishInfo m_FishInfo, CPoint doorpt, CPoint shotpt0);
	void movingshot3(int mode, CPoint aimpt, CFishAction &action, int fishID, CFishInfo m_FishInfo, CPoint doorpt, CPoint shotpt0);
public:
	virtual bool Strategy(
		RefArray<CFishAction> aAction,
		RefArray<CFishInfo>   aFish,
		RefArray<CBallInfo>   aBallinfo,
		RefArray<OBSTAINFO>   aObstacle,
		RefArray<CHANNEL>     aChannel) override;
};