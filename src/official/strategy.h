/**
 * @note 源码已经稍做调整以适配项目
 * @author zymelaii
 * @date 2022-06-26
 */

/**
 * @name:          多水中机器人控制无色标平台 
 * @description:   版权所有，授权使用
 * @version:       1.2.0.0
 * @copyright:     国际水中机器人联盟
 * @author:        ZhangX,XiongML
 * @date:          14-04-04 16:55
 */

#ifndef MUR_STRATEGY_H
#define MUR_STRATEGY_H

#include "../official/struct.h"
#include "../official/ballinfo.h"
#include "../official/fishaction.h"
#include "../official/fishinfo.h"

/**
 * @brief 机器鱼策略控制算法类
 */
class CStrategy {
public:
	int m_TurnYuan;
	int m_StatePeriod;
public:
	CStrategy();
	BOOL StrategyPtr(BOOL (*ptFunc)( IplImage*, IplImage*, CFishAction*,int, CFishInfo*,int, CBallInfo*,int, OBSTAINFO*,int, CHANNEL*, int));
	BOOL Strategy(IplImage* imgOrig=NULL,IplImage* imgRecog=NULL,CFishAction action[]=NULL, int n_action=0 ,CFishInfo fishinfo[]=NULL, int n_fishinfo=0,
		          CBallInfo ballinfo[]=NULL, int n_ballinfo=0, OBSTAINFO obst[]=NULL, int n_obst=0, CHANNEL channel[]=NULL, int n_channel=0);
	void movingshot(CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo,CPoint doorpt,CPoint shotpt0);
	BOOL Strategynew(CFishAction m_action[],CFishInfo m_FishInfo[],CBallInfo  m_goalinfo[], OBSTAINFO m_obst[],CHANNEL m_Channel[]);
	void movingshot1(int mode,CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo,CPoint doorpt,CPoint shotpt0);
	void movingshot2(int mode,CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo,CPoint doorpt,CPoint shotpt0);
	void movingshot3(int mode,CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo,CPoint doorpt,CPoint shotpt0);
	void ptop(CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo);
	void BasicActionStop(CFishInfo fish[],CFishAction action[],int id); //fish[]所有鱼的信息；action[]所有鱼的动作信息；id要控制的鱼的鱼号
	void BasicActionGo(CFishInfo fish[],CFishAction action[],int id,int speed,int direction,int mode); //fish[]所有鱼的信息；action[]所有鱼的动作信息；id要控制的鱼的鱼号；speed速度档；direction方向档；mode模态，0通用转弯1原始转弯
protected:
	void Roundp2p(CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo);
	void turn();
	void con2tempt(CPoint aimpt,CFishAction m_action[],CFishInfo m_FishInfo[]);
	CPoint shotpt(CPoint aimpt,CPoint doorpt);
};

#endif /*MUR_STRATEGY_H*/