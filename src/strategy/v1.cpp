#include "v1.h"
#include "../mathtools.h"
#include <iostream>

/**
 * @note 以下所有全局变量所在的参考系皆为全局视觉的图像系
 * @note 关于门的边界信息需参考比赛场地示意图进行理解
 * @param width 水池宽度
 * @param height 水池高度
 * @param doorwidth 球门宽度
 * @param middle 水池中轴线高度
 * @param middleup 水池上半区中轴线高度
 * @param middledown 水池下半区中轴线高度
 * @param onedoorleft 门1左边界横坐标
 * @param onedoorright 门1右边界横坐标
 * @param twodoorleft 门2左边界横坐标
 * @param twodoorright 门2右边界横坐标
 * @param finaldoordown 球门框左侧入口下界限高度
 * @param finaldoordown1 球门框下界限高度
 * @param finaldoorx 球门框入口横轴界限
 * @param radius 水球半径
 * @param speedA A鱼当前速度
 * @param speedB B鱼当前速度
 * @param A A鱼编号
 * @param B B鱼编号
 * @param turndistance
 * @param shopdistance
 * @param distance0
 * @param findpoint 是否处于处于找点的状态（存疑） 
 * @author zymelaii
 * @date 2022-06-27
 */

int width = REGBMP_WIDTH;
int height = REGBMP_HEIGHT;
int doorwidth = 45;
int middle, middleup, middledown;
int onedoorleft, onedoorright;
int twodoorleft, twodoorright;
int finaldoordown, finaldoordown1, finaldoorx;
int radius = 13;
int speedA = 9, speedB = 9;
int turndistance = 30, shopdistance = 18, distance0 = 200;
const int A = 0, B = 1;
bool findpoint = false;

static int
	flag1  = 0, flag2  = 0, flag3  = 0, flag4  = 0,
	flag5  = 0, flag6  = 0, flag7  = 0, flag8  = 0,
	flag9  = 0, flag10 = 0, flag11 = 0, flag12 = 0,
	flag13 = 0;
int Q = 0, W = 0, E = 0;

/**
 * @note 功能未知的函数（主要是一坨flag的意义不清楚）
 */
void deletezero(int mode) {
    switch (mode) {
        case 10: {
            flag3  = 0; flag4  = 0; flag5  = 0; flag6  = 0;
            flag7  = 0; flag8  = 0; flag9  = 0; flag10 = 0;
            flag11 = 0; flag12 = 0;
            break;
        }
        case 11: {
            flag1  = 0; flag2  = 0; flag3  = 0; flag4  = 0;
            flag5  = 0; flag6  = 0; flag9  = 0; flag10 = 0;
            flag11 = 0; flag12 = 0;
            break;
        }
        case 20: {
            flag1  = 0; flag2  = 0; flag5  = 0; flag6  = 0;
            flag7  = 0; flag8  = 0; flag9  = 0; flag10 = 0;
            flag11 = 0; flag12 = 0;
            break;
        }
        case 21: {
            flag1  = 0; flag2  = 0; flag3  = 0; flag4  = 0;
            flag7  = 0; flag8  = 0; flag9  = 0; flag10 = 0;
            flag11 = 0; flag12 = 0;
            break;
        }
        case 31: {
            flag1  = 0; flag2  = 0; flag3  = 0; flag4  = 0;
            flag5  = 0; flag6  = 0; flag7  = 0; flag8  = 0;
            flag11 = 0; flag12 = 0;
            break;
        }
        case 30: {
            flag1  = 0; flag2  = 0; flag3  = 0; flag4  = 0;
            flag5  = 0; flag6  = 0; flag7  = 0; flag8  = 0;
            flag9  = 0; flag10 = 0;
            break;
        }
    }
}

/**
 * @brief 策略相关数据初始化
 * @note 以下id及鱼信息、鱼动作信息数组相关，取值说明：
 *  0-7是A队的，8-15是B队的，例如，A队的第一条鱼的动作是actions[0]，
 *  B队的第一条鱼的动作是actions[8]。
 */
void initdata(const RefArray<CHANNEL> &aChannel) {
    middle = aChannel[0].center.y;
    middledown = middle + 35;
    middleup = middle - 35;
    onedoorleft = aChannel[0].center.x - 40;
    onedoorright = aChannel[0].center.x + 40;
    twodoorleft = aChannel[1].center.x - 40;
    twodoorright = aChannel[1].center.x + 40;
    finaldoorx = aChannel[2].center.x;
    finaldoordown = middle - 80;
    finaldoordown1 = finaldoordown-35;
}

/**
 * @brief 上平面区域判断
 */
bool judgeareaup(CPoint point, int area_id) {
    switch (area_id) {
        case 1: /// 上平面
            return point.y < middle ;
        case 4: /// 在上左平面
            return point.y < middle && point.x < width / 2;
        case 5: /// 在上右平面
            return point.y < middle && point.x > width / 2;
        case 6: /// 在一门右向左区域，在靠下
            return point.y < middle && point.y > middleup - 30 && point.x < onedoorright + 20;
        case 7: /// 二球门左,右之间
            return point.y < middle && point.x > twodoorleft && point.x < twodoorright;
        case 8: /// 二球门右
            return point.y < middle && point.x > twodoorright;
        case 9: /// 一球门右二球门左
            return point.y < middle && point.x > onedoorright + 20 && point.x < twodoorleft;
        case 10: /// 一球门右以左
            return point.y < middle && point.x < onedoorright + 20;
        case 11: /// 二球门右靠下
            return point.y < middle - 50 && point.y > middleup - 30 && point.x > twodoorright;
        case 12: /// 球门区域（一二球门之间）
            return point.y < middle - 50 && point.y > middleup && point.x < twodoorright && point.x > twodoorleft;
        case 13: /// 一球门右中门以左
            return point.y < middle  && point.x > onedoorright && point.x < width / 2;
        case 14: /// 中门以右二左门以左靠上
            return point.y < middleup - 30 && point.x > width / 2 && point.x < twodoorleft;
        case 15: /// 球门区域（一二球门之间）
            return point.y < middle - 50 && point.y > middleup && point.x < onedoorright && point.x > onedoorleft;
        default:
            return false;
    }
}

/**
 * @brief 下平面区域判断
 */
bool judgearea(CPoint point, int area_id) {
    switch (area_id) {
        case 2: /// 下平面
            return point.y>middle;
        case 3: /// 在下区域卡球
            return point.y>middle&&point.y<middledown&&point.x<width/2&&point.x>onedoorright;
        case 7: /// 在下左平面
            return point.y>height/2&&point.x<width/2;
        case 8: /// 在下右平面
            return point.y>height/2&&point.x>width/2;
        case 9: /// 在下左平面(靠下，在第一门左面）
            return point.y>350&&point.y<450&&point.x<onedoorleft;
        case 10: /// 在下左平面(在第一门之间）
            return point.y>middledown&&point.x>onedoorleft&&point.x<onedoorright;
        case 11: /// 在下左平面(靠上，在第一门左面）
            return point.y>middle+50&&point.y<middledown+30&&point.x<onedoorleft+10;
        case 12: /// 在下右平面(在第二门之间,middle和middledown之间)
            return point.y>middle+50&&point.y<middledown&&point.x>twodoorleft&&point.x<twodoorright;
        case 13: /// 在下左平面(在第一门之间,middle和middledown之间)
            return point.y>middle&&point.y<middledown&&point.x>onedoorleft+10&&point.x<onedoorright;
        case 14: /// 在下左平面(在第一门偏右,middle和middledown之间)
            return point.y>middle+50&&point.y<middledown&&point.x<onedoorright+100&&point.x>onedoorright;
        case 15: /// 在下右平面(在第二门偏右,middle和middledown之间)
            return point.y>middle+50&&point.y<middledown&&point.x<twodoorright+100&&point.x>twodoorright;
        case 16: /// 在下右平面(二球门左)
            return point.y>middle&&point.x<twodoorright;
        case 17: /// 在下右平面(二球门右)
            return point.y>middle&&point.x>=twodoorright;
        case 18: /// 在下右平面(一球门左)
            return point.y>middle&&point.x<onedoorright;
        case 19: /// 在下右平面(一球门右)
            return point.y>middle&&point.x>=onedoorright;
        case 20: /// 在最后门下面一点点
            return point.y>finaldoordown1&&point.y<finaldoordown&&point.x>610&&point.x<finaldoorx;
        case 21: /// 在最后门上面一条线
            return point.y<finaldoordown1&&point.y>middle&&point.x>finaldoorx&&point.x<finaldoorx;
        case 22: /// 在第一球门左面击球区域
            return point.y>middledown&&point.x<width/2&&point.x>onedoorright;
        case 23: /// 最后射门死区
            return point.y>middle&&point.y<finaldoordown1&&point.x>finaldoorx;
        case 24: /// 理想最后射门区域
            return point.y>middle&&point.x<finaldoorx;
        case 25: /// 最后球门的区域，一二球门之间靠上
            return point.y>middle+50&&point.y<middledown&&point.x<twodoorright&&point.x>onedoorright;
        case 26:
            return point.y>middledown+50&&point.x>twodoorleft&&point.x<finaldoorx;
        case 27:
            return point.y>350&&point.x>finaldoorx;
        case 28:
            return point.y<350&&point.x>finaldoorx;
        case 29: /// 在下右平面(在第二门之间,middle和middledown之间)
            return point.y>middle&&point.y<middledown&&point.x>twodoorleft&&point.x<twodoorright;
        case 30: /// 在下左平面(在第一门偏右,middle和middledown之间)
            return point.y>middle&&point.y<middledown+20&&point.x<onedoorright+100&&point.x>onedoorright;
        case 31: /// 在下右平面(在第二门偏右,middle和middledown之间)
            return point.y>middle&&point.y<middledown&&point.x<twodoorright+100&&point.x>twodoorright;
        case 32: /// 在下左平面(在第一门之间,middle和middledown之间)
            return point.y>middle&&point.y<middledown&&point.x>onedoorleft&&point.x<onedoorright;
        case 33: /// 在下左平面(靠下，在第一门左面）
            return point.y<480&&point.y>450&&point.x<onedoorleft;
        default:
            return false;
    }
}

/**
 * @brief 判断球当前的状态
 * @note 返回值表示的是鱼当前准备进攻几号门
 */
int judgeball(CPoint point) {
    Q = 1;
    if (point.y < middle - 10 && point.x < onedoorright + 20) {
        W = 1;
    }
    if (W == 1 && judgearea(point, 32)) { // 回退到一
        W = 0;
    }
    if (W == 1 && (point.y > middle + 10 && point.y < middledown + 10 && point.x > twodoorleft - 10 && point.x < twodoorright + 10)) {
        E = 1;
    }
    if (E == 1 && judgeareaup(point, 7)) { // 回退到二
        E = 0;
    }
    if (Q == 1 && W == 0 && E == 0) return 1;
    if (Q == 1 && W == 1 && E == 0) return 2;
    if (Q == 1 && W == 1 && E == 1) return 3;
    assert(false);
    return 0;
}

/**
 * @brief 垂直向上，下顶球
 */
void StrategyCloneV1::movingshot1(int mode,CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo,CPoint doorpt,CPoint shotpt0) {
    CPoint FishPt = m_FishInfo.GetCenterPoint(); // 鱼身坐标
    CPoint FishHeadPt = m_FishInfo.GetHeaderPoint(); // 鱼头坐标
    double FishDir = normalizeAngle(getVecAngle(FishPt, FishHeadPt)) / M_PI * 180; // 鱼的朝向角度（鱼身向鱼头）
    CPoint templeft0, templeft1, templeft2, tempright0, tempright1, tempright2; // 临时击球点

    templeft0.y=aimpt.y;
    templeft0.x=aimpt.x-70;
    if(templeft0.x<0)templeft0.x=0;
    if(templeft0.x>740)templeft0.x=720;
    if(templeft0.y<0)templeft0.y=0;
    if(templeft0.y>480)templeft0.y=480;
    templeft1.y=aimpt.y+70;
    templeft1.x=aimpt.x-50;
    if(templeft1.x<0)templeft1.x=0;
    if(templeft1.x>740)templeft1.x=720;
    if(templeft1.y<0)templeft1.y=0;
    if(templeft1.y>480)templeft1.y=480;
    templeft2.y=aimpt.y-70;
    templeft2.x=aimpt.x-50;
    if(templeft2.x<0)templeft2.x=0;
    if(templeft2.x>740)templeft2.x=720;
    if(templeft2.y<0)templeft2.y=0;
    if(templeft2.y>480)templeft2.y=480;
    tempright0.y=aimpt.y;
    tempright0.x=aimpt.x+50;
    if(tempright0.x<0)tempright0.x=0;
    if(tempright0.x>740)tempright0.x=720;
    if(tempright0.y<0)tempright0.y=0;
    if(tempright0.y>480)tempright0.y=480;
    tempright1.y=aimpt.y+70;
    tempright1.x=aimpt.x+50;
    if(tempright1.x<0)tempright1.x=0;
    if(tempright1.x>740)tempright1.x=720;
    if(tempright1.y<0)tempright1.y=0;
    if(tempright1.y>480)tempright1.y=480;
    tempright2.y=aimpt.y-70;
    tempright2.x=aimpt.x+50;
    if(tempright2.x<0)tempright2.x=0;
    if(tempright2.x>740)tempright2.x=720;
    if(tempright2.y<0)tempright2.y=0;
    if(tempright2.y>480)tempright2.y=480;
    if(mode==0)
    {
        if((FishPt.y>aimpt.y+shopdistance&&flag1==0&&flag2==0)||FishPt.y>=430||FishPt.x<40)
        {
            //action.speed=speed1;
            flag1=0;
            flag2=0;
            std::cout<<"慢击球"<<std::endl;
            spinP2PMove(shotpt0, m_FishInfo, action);
        }
        if(FishPt.x<aimpt.x&&FishPt.y<aimpt.y+shopdistance+5&&FishPt.y<=430&&FishPt.x>40)
        {
            flag1=1;
        }
        if(FishPt.x>=aimpt.x&&FishPt.y<aimpt.y+shopdistance+5&&FishPt.y<=430&&FishPt.x>40)
        {
            flag2=1;
        }
        if(flag1==1)
        {
            flag2=0;
            if((FishDir>-180&&FishDir<0)&&getDistance(FishPt,aimpt)<120&&findpoint==0)
            {
                std::cout<<"左行"<<std::endl;
                action.direction=1;
                //Roundp2pleft(templeft1,action,0,m_FishInfo);
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                spinP2PMove(templeft1, m_FishInfo, action);
                if(getDistance(FishPt,templeft1)<turndistance)
                {
                    findpoint=0;
                    flag1=0;
                }
            }
        }
        if(flag2==1)
        {
            flag1=0;
            if((FishDir>-180&&FishDir<0)&&getDistance(FishPt,aimpt)<120&&findpoint==0)
            {
                std::cout<<"右行"<<std::endl;
                action.direction=12;
                //Roundp2pright(tempright1,action,0,m_FishInfo);
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                spinP2PMove(tempright1, m_FishInfo, action);
                if(getDistance(FishPt,tempright1)<turndistance)
                {
                    findpoint=0;
                    flag2=0;
                }
            }
        }
    }
    if(mode==1)
    {
        if((FishPt.y<aimpt.y-shopdistance&&flag7==0&&flag8==0)||FishPt.y>=430||FishPt.x<40)
        {
            //action.speed=speed1;
            flag7=0;
            flag8=0;
            std::cout<<"慢击球"<<std::endl;
            spinP2PMove(shotpt0, m_FishInfo, action);
        }
        if(FishPt.x<aimpt.x&&FishPt.y>=aimpt.y-shopdistance&&FishPt.y>=40&&FishPt.y<=430)
        {
            flag7=1;
        }
        if(FishPt.x>aimpt.x&&FishPt.y>=aimpt.y-shopdistance&&FishPt.y>=40&&FishPt.y<=440)
        {
            flag8=1;
        }
        if(flag7==1)
        {
            flag8=0;
            if((FishDir>0||FishDir<-150)&&getDistance(FishPt,aimpt)<120&&findpoint==0)
            {
                std::cout<<"右行"<<std::endl;
                action.direction=12;
                //Roundp2pright(templeft2,action,0,m_FishInfo);
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                spinP2PMove(templeft2, m_FishInfo, action);
                if(getDistance(FishPt,templeft2)<turndistance)
                {
                    findpoint=0;
                    flag7=0;
                }
            }

        }
        if (flag8 == 1) {
            flag7 = 0;
            if ((FishDir > -30 || FishDir < -170) && getDistance(FishPt, aimpt) < 120 && findpoint == 0) {
                std::cout << "左行" << std::endl;
                action.direction = 1;
                //Roundp2pleft(tempright2, action, 0, m_FishInfo);
            } else {
                findpoint = 1;
            }
            if (findpoint == 1) {
                std::cout << "找点" << std::endl;
                spinP2PMove(tempright2, m_FishInfo, action);
                if (getDistance(FishPt, tempright2) < turndistance) {
                    findpoint = 0;
                    flag8 = 0;
                }
            }
        }
    }
}

void StrategyCloneV1::movingshot2(
    int mode, CPoint aimpt,
    CFishAction &action, int fishID,
    CFishInfo m_FishInfo,
    CPoint doorpt,
    CPoint shotpt0) //垂直向左，向右顶球
{
    CPoint FishPt = m_FishInfo.GetCenterPoint(); // 鱼身坐标
    CPoint FishHeadPt = m_FishInfo.GetHeaderPoint(); // 鱼头坐标
    double FishDir = normalizeAngle(getVecAngle(FishPt, FishHeadPt)) / M_PI * 180; // 鱼的朝向角度（鱼身向鱼头）
    CPoint tempup0, tempup1, tempup2, tempdown0, tempdown1, tempdown2; // 临时击球点

    tempup0.x = aimpt.x;
    tempup0.y = aimpt.y - 50;
    tempup0.x = clamp(tempup0.x, 0, REGBMP_WIDTH);
    tempup0.y = clamp(tempup0.y, 0, REGBMP_HEIGHT);

    tempup1.x = aimpt.x + 70;
    tempup1.y = aimpt.y - 50;
    tempup1.x = clamp(tempup1.x, 0, REGBMP_WIDTH);
    tempup1.y = clamp(tempup1.y, 0, REGBMP_HEIGHT);

    tempup2.x = aimpt.x - 70;
    tempup2.y = aimpt.y - 50;
    if (judgeball(aimpt) == 3) {
        tempup2.x = aimpt.x - 90;
        tempup2.y = aimpt.y - 50;
    }
    tempup2.x = clamp(tempup2.x, 0, REGBMP_WIDTH);
    tempup2.y = clamp(tempup2.y, 0, REGBMP_HEIGHT);

    tempdown0.x = aimpt.x;
    tempdown0.y = aimpt.y + 50;
    tempdown0.x = clamp(tempdown0.x, 0, REGBMP_WIDTH);
    tempdown0.y = clamp(tempdown0.y, 0, REGBMP_HEIGHT);

    tempdown1.y = aimpt.y - 50;
    tempdown1.x = aimpt.x + 70;
    tempdown1.x = clamp(tempdown1.x, 0, REGBMP_WIDTH);
    tempdown1.y = clamp(tempdown1.y, 0, REGBMP_HEIGHT);

    tempdown2.y = aimpt.y + 40;
    tempdown2.x = aimpt.x - 70;
    if (judgeball(aimpt) == 3) {
        tempdown2.y = aimpt.y + 50;
        tempdown2.x = aimpt.x - 90;
    }
    tempdown2.x = clamp(tempdown2.x, 0, REGBMP_WIDTH);
    tempdown2.y = clamp(tempdown2.y, 0, REGBMP_HEIGHT);

    if (mode == 0) {
        if ((FishPt.x > aimpt.x + shopdistance && flag1 == 0 && flag4 == 0) || FishPt.x <= 80 || FishPt.x > 680) {
            flag3 = 0;
            flag4 = 0;
            std::cout << "慢击球" << std::endl;
            spinP2PMove(shotpt0, m_FishInfo, action);
        }
        if (FishPt.x < aimpt.x + shopdistance && FishPt.x >= 80 && FishPt.x < 680) {
            if (FishPt.y > aimpt.y) {
                flag3 = 1;
            } else if (FishPt.y < aimpt.y) {
                flag4 = 1;
            }
        }
        if (flag3 == 1) {
            flag4 = 0;
            if ((FishDir < -90 || FishDir > 90) && getDistance(FishPt, aimpt) < 120 && findpoint == 0) {
                std::cout << "左行" << std::endl;
                action.direction = 1;
            } else {
                findpoint = 1;
            }
            if (findpoint == 1) {
                std::cout << "找点" << std::endl;
                spinP2PMove(tempdown1, m_FishInfo, action);
                if (getDistance(FishPt, tempdown1) < turndistance) {
                    findpoint = 0;
                    flag3 = 0;
                }
            }
        }
        if (flag4 == 1) {
            flag3 = 0;
            if ((FishDir < -90 || FishDir > 0) && getDistance(FishPt, aimpt) < 120 && findpoint == 0) {
                std::cout << "右行" << std::endl;
                action.direction = 12;
            } else {
                findpoint = 1;
            }
            if (findpoint == 1) {
                std::cout << "找点" << std::endl;
                spinP2PMove(tempup1, m_FishInfo, action);
                if (getDistance(FishPt, tempup1) < turndistance) {
                    findpoint = 0;
                    flag4 = 0;
                }
            }
        }
    }
    if (mode == 1) {
        if ((FishPt.x < aimpt.x - shopdistance && flag5 == 0 && flag6 == 0) || FishPt.x <= 80 || FishPt.x > 680) {
            flag5 = 0;
            flag6 = 0;
            std::cout << "击球" << std::endl;
            spinP2PMove(shotpt0, m_FishInfo, action);
        }
        if (FishPt.x > aimpt.x - shopdistance && FishPt.x >= 80) {
            if (FishPt.y < aimpt.y) {
                flag5 = 1;
                flag6 = 0;
            } else if (FishPt.y > aimpt.y) {
                flag5 = 0;
                flag6 = 1;
            }
        }
        if (flag5 == 1) {
            flag6 = 0;
            if ((FishDir < 90 || FishDir > 160) && getDistance(FishPt, aimpt) < 180 && findpoint == 0) {
                std::cout << "左行" << FishDir << std::endl;
                action.direction = 1;
            } else {
                findpoint = 1;
            }
            if (findpoint == 1) {
                std::cout << "找点" << std::endl;
                spinP2PMove(tempup2, m_FishInfo, action);
                if (getDistance(FishPt, tempup2) < turndistance) {
                    findpoint = 0;
                    flag5 = 0;
                }
            }
        }
        if (flag6 == 1) {
            flag5 = 0;
            if ((FishDir < -160 || FishDir > -90) && getDistance(FishPt,aimpt) < 120 && findpoint == 0) {
                std::cout << "右行" << std::endl;
                action.direction = 12;
            } else {
                findpoint = 1;
            }
            if (findpoint == 1) {
                std::cout << "找点" << std::endl;
                spinP2PMove(tempdown2, m_FishInfo, action);
                if (getDistance(FishPt, tempdown2) < turndistance) {
                    findpoint = 0;
                    flag6 = 0;
                }
            }
        }
    }
}

void StrategyCloneV1::movingshot3(int mode,CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo,CPoint doorpt,CPoint shotpt0) { //垂直向左，向右顶球
    CPoint FishPt = m_FishInfo.GetCenterPoint(); // 鱼身坐标
    CPoint FishHeadPt = m_FishInfo.GetHeaderPoint(); // 鱼头坐标
    double FishDir = normalizeAngle(getVecAngle(FishPt, FishHeadPt)) / M_PI * 180; // 鱼的朝向角度（鱼身向鱼头）
    CPoint tempup0, tempup1, tempup2, tempdown0, tempdown1, tempdown2; // 临时击球点

    tempup0.y=160;//向最左面顶球的姿态调整点
    tempup0.x=aimpt.x+70;;
    if(tempup0.x<0)tempup0.x=0;
    if(tempup0.x>740)tempup0.x=720;
    if(tempup0.y<0)tempup0.y=0;
    if(tempup0.y>480)tempup0.y=480;
    tempup1.y=tempup0.y;
    tempup1.x=720;
    if(tempup1.x<0)tempup1.x=0;
    if(tempup1.x>740)tempup1.x=720;
    if(tempup1.y<0)tempup1.y=0;
    if(tempup1.y>480)tempup1.y=480;
    tempup2.y=tempup0.y;
    tempup2.x=0;
    if(tempup2.x<0)tempup2.x=0;
    if(tempup2.x>740)tempup2.x=720;
    if(tempup2.y<0)tempup2.y=0;
    if(tempup2.y>480)tempup2.y=480;
    tempdown0.y=350;//向最右面顶球的姿态调整点
    tempdown0.x=aimpt.x-70;
    if(judgeball(aimpt)==3)
    {
        if(aimpt.y<middledown + 10) {
            tempdown0.y=350; //向最右面顶球的姿态调整点
            tempdown0.x=twodoorright;
        } else {
            tempdown0.y=350; //向最右面顶球的姿态调整点
            tempdown0.x=aimpt.x-70;
        }
    }
    if(tempdown0.x<0)tempdown0.x=0;
    if(tempdown0.x>740)tempdown0.x=720;
    if(tempdown0.y<0)tempdown0.y=0;
    if(tempdown0.y>480)tempdown0.y=480;
    tempdown1.y=tempdown1.y;
    tempdown1.x=720;
    if(tempdown1.x<0)tempdown1.x=0;
    if(tempdown1.x>740)tempdown1.x=720;
    if(tempdown1.y<0)tempdown1.y=0;
    if(tempdown1.y>480)tempdown1.y=480;
    tempdown2.y=tempdown1.y;
    tempdown2.x=0;
    if(tempdown2.x<0)tempdown2.x=0;
    if(tempdown2.x>740)tempdown2.x=720;
    if(tempdown2.y<0)tempdown2.y=0;
    if(tempdown2.y>480)tempdown2.y=480;
    if(mode==0)
    {
        if((FishPt.x>aimpt.x+shopdistance&&flag11==0&&flag12==0)||FishPt.x>680||FishPt.x<=80)
        {
            flag11=0;
            flag12=0;
            std::cout<<"击球"<<std::endl;
            spinP2PMove(shotpt0, m_FishInfo, action);
        }
        if(FishPt.x<aimpt.x+shopdistance&&FishPt.y>aimpt.y&&FishPt.x<=680)
        {
            flag11=1;
        }
        if(FishPt.x<aimpt.x+shopdistance&&FishPt.y<aimpt.y&&FishPt.y>aimpt.y-200&&FishPt.x<=680)
        {
            flag12=1;

        }
        if(flag11==1)
        {
            flag12=0;
            if(FishDir<180&&FishDir>0&&getDistance(FishPt,aimpt)<180&&findpoint==0)
            {
                std::cout<<"左行"<<std::endl;
                action.direction=1;
                //Roundp2pright(tempup0,action,0,m_FishInfo);
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                spinP2PMove(tempup0, m_FishInfo, action);
                if(getDistance(FishPt,tempup0)<turndistance)
                {
                    findpoint=0;
                    flag11=0;
                }
            }
        }
        if(flag12==1)
        {
            flag11=0;
            if((FishDir<0||FishDir>90)&&getDistance(FishPt,aimpt)<180&&findpoint==0)
            {
                std::cout<<"右行"<<std::endl;
                action.direction=12;
                //Roundp2pright(tempup0,action,0,m_FishInfo);
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                spinP2PMove(tempup0, m_FishInfo, action);
                if(getDistance(FishPt,tempup0)<turndistance)
                {
                    findpoint=0;
                    flag12=0;
                }
            }

        }

    }
    if(mode==1)
    {
        if((FishPt.x<aimpt.x-shopdistance&&flag9==0&&flag10==0)||FishPt.x>680||FishPt.x<=80)
        {
            flag9=0;
            flag10=0;
            std::cout<<"击球"<<std::endl;
            spinP2PMove(shotpt0, m_FishInfo, action);
        }
        if(FishPt.x>aimpt.x-shopdistance&&FishPt.y<aimpt.y-20&&FishPt.x>=80)
        {
            flag9=1;
        }
        if(judgeball(aimpt)==3)
        {
            shopdistance=22;
        }
        if(FishPt.x>aimpt.x-shopdistance&&FishPt.y>aimpt.y-20&&FishPt.x>=80)
        {
            flag10=1;
        }
        if(flag9==1)
        {
            flag10=0;
            if(FishDir>-180&&FishDir<0&&getDistance(FishPt,aimpt)<180&&findpoint==0)
            {
                std::cout<<"左行"<<std::endl;
                action.direction=1;
                //Roundp2pright(tempdown0,action,0,m_FishInfo);
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                spinP2PMove(tempdown0, m_FishInfo, action);
                if(getDistance(FishPt,tempdown0)<turndistance)
                {
                    findpoint=0;
                    flag9=0;
                }
            }
        }
        if(flag10==1)
        {
            flag9=0;
            if(FishDir>-90||FishDir<-150&&getDistance(FishPt,aimpt)<180&&findpoint==0)
            {
                std::cout<<"右行"<<std::endl;
                action.direction=12;
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                spinP2PMove(tempdown0, m_FishInfo, action);
                if(getDistance(FishPt,tempdown0)<turndistance)
                {
                    findpoint=0;
                    flag10=0;
                }
            }
        }

    }
}

/**
 * @brief 水中协作顶球策略
 * @note 版本：最终版2 红4 - 改
 */
bool StrategyCloneV1::Strategy(RefArray<CFishAction> aAction, RefArray<CFishInfo> aFish,
	RefArray<CBallInfo> aBallinfo, RefArray<OBSTAINFO> aObstacle, RefArray<CHANNEL> aChannel) {

    CPoint FishPt,FishPt1;// 鱼中心的位置
    double FishDir,distfishtoaimpt,distfishtoheadpt,dirfishtoaimpt,dirfishtoheadpt,distballtodoor,diraimpttodoor,diraimpttodoor1,diraimpttodoor3,dirspeed;// 鱼头的方向
    CPoint FishHeadPt;//鱼头的位置
    CPoint FishTailPt;//鱼尾巴的位置
    CPoint tempup,tempdown;//临时击球点
    CPoint aimpt;//球的坐标
    CPoint doorpt,doorpt1,doorpt3;//球门坐标
    CPoint shotpt0,shotpt1,shotpt3,shotptup,shotptdown,shotptright,shotptleft,finalshot;//击球点坐标
    doorpt=aChannel[A].center;
    doorpt1=aChannel[B].center;
    doorpt3=aChannel[2].center;
    //std::cout<<doorpt1.x<<std::endl;
    aimpt=aBallinfo[0].getCenter();
    std::cout<<aimpt.x<<" "<<aimpt.y<<std::endl;
    FishPt=aFish[A].GetCenterPoint();//鱼中心点坐标
    FishPt1=aFish[B].GetCenterPoint();//鱼中心点坐标
    FishHeadPt = aFish[A].GetHeaderPoint();//鱼头坐标

    FishDir = normalizeAngle(getVecAngle(FishPt, FishHeadPt) * 180 / M_PI, false); // 鱼的角度

    distfishtoaimpt = getDistance(FishPt,aimpt);//球到鱼中心的距离
    distfishtoheadpt = getDistance(FishPt,FishHeadPt);//球到鱼头的距离(有问题）

    dirfishtoheadpt = normalizeAngle(getVecAngle(aimpt, FishHeadPt)) * 180 / M_PI; //球到鱼头的角度
    dirfishtoaimpt = normalizeAngle(getVecAngle(aimpt, FishPt)) * 180 / M_PI; //球到鱼中心的角度
    distballtodoor = getDistance(aimpt, doorpt); //球到门的距离
    diraimpttodoor = normalizeAngle(getVecAngle(aimpt, doorpt)) * 180 / M_PI; //球到球门的角度
    diraimpttodoor1 = normalizeAngle(getVecAngle(aimpt, doorpt1)) * 180 / M_PI; //球到球门的角度
    diraimpttodoor3 = normalizeAngle(getVecAngle(aimpt, doorpt3)) * 180 / M_PI; //球到球门的角度
    dirspeed = fabs(normalizeAngle(FishDir - getVecAngle(FishPt, aimpt)) * 180 / M_PI); // 鱼转向角速度

    std::cout << "dirspeed" << dirspeed << std::endl; //实时获取参数值
    initdata(aChannel); //到具体的场地以后将范围扩大化

    //击球点初始化
    shotpt0.x=aimpt.x-radius*cos(diraimpttodoor);
    shotpt0.y=aimpt.y-radius*sin(diraimpttodoor);

    shotpt1.x=aimpt.x-radius*cos(diraimpttodoor1);
    shotpt1.y=aimpt.y-radius*sin(diraimpttodoor1);

    shotpt3.x=aimpt.x-radius*cos(diraimpttodoor3);
    shotpt3.y=aimpt.y-radius*sin(diraimpttodoor3);
    shotptup.y=aimpt.y+radius;//垂直向上顶球点
    shotptup.x=aimpt.x;
    shotptdown.y=aimpt.y-radius;//垂直向下顶球点
    shotptdown.x=aimpt.x;
    shotptright.y=aimpt.y-5;//最右面的点
    shotptright.x=aimpt.x+radius;
    shotptleft.y=aimpt.y+5;//最左面的点
    shotptleft.x=aimpt.x-radius;
    finalshot.y=middle;
    finalshot.x=finaldoorx;
    aAction[B].speed=speedB;
    aAction[B].state=0;
    aAction[B].mode=0;
    int speed=0;
    if(dirspeed>60)speed=9;
    else
        speed=9;
    aAction[A].state=0;
    aAction[A].mode=0;
    aAction[A].speed=speed;
    //测试顶球射门的策略
    //movingshot(aimpt,aAction[A],A,aFish[A],doorpt,shotpt0);
    //movingshot2(0,shotptup,aAction[A],A,aFish[A],doorpt,CPoint(shotptright.x,shotptright.y+5));
    /*deletezero(20);
    movingshot4(0,aimpt,aAction[A],A,aFish[A],doorpt,CPoint(aimpt.x+radius,aimpt.y));
    */
    // 最原始让鱼先右游一会，然后向右开mode;

    static int stop0=0,stop1=0,count0=0,count1=0,movement=0;;
    // A鱼顶左面球门策略
    std::cout << judgeball(aimpt) << std::endl;
    if (movement == 0) {
        std::cout << "1" << std::endl;
        if (P2PMove(CPoint(doorpt.x, doorpt.y - 100), aFish[B], aAction[B])) {
            ++movement;
        }
    }
    if(movement==1) {
        std::cout << "2" << std::endl;
        P2PMove(CPoint(doorpt.x, doorpt.y + 20), aFish[B], aAction[B]);
        if(FishPt1.y > height / 2 + 10) {
            movement++;
        }
    }
    if(movement==2)
    {
        std::cout<<"3"<<std::endl;
        if (P2PMove(CPoint(doorpt.x - 60 , doorpt.y + 60), aFish[B], aAction[B])) {
            ++movement;
        }
    }
    if(movement==3)
    {
        std::cout<<"4"<<std::endl;
        P2PMove(CPoint(doorpt.x, doorpt.y - 20), aFish[B], aAction[B]);
        if(FishPt1.y < height / 2 - 10) {
            ++movement;
        }
    }
    if(movement==4)
    {
        std::cout<<"5"<<std::endl;
        P2PMove(CPoint(width, 0), aFish[B], aAction[B]); // 门全开，在右上角等待
        if(judgeball(aimpt) == 2) { // 准备进攻二号门时
            ++movement;
        }
    }
    if (movement == 5)	{ // 在左上角等待
        std::cout<<"B等待3"<<std::endl;
        spinP2PMove(CPoint(0, 0), aFish[B], aAction[B]);
        if(judgeball(aimpt) == 1) {
            movement--;
        }
    }
    if(movement==6)		//--暂时没用
    {
        std::cout<<"B等待3"<<std::endl;
        aAction[B].direction=14;
        if(getDistance(FishPt1,aimpt)>80)
        {
            movement--;
        }
    }
    //middle=middle-200;
    if (movement >= 2)		//--B鱼顶开一号门后，A鱼再开始移动
    {
        if (judgeball(aimpt) == 1)
        {
            std::cout << "A鱼目标为一号门" << std::endl;
            if (judgearea(FishPt, 2))//鱼在下平面
            {
                middle = middle - 50;
                if (judgearea(aimpt, 2))//球在下平面
                {
                    if (aimpt.y > middle&&aimpt.x < onedoorright)//在一球门左
                    {
                        //优先判断是否处于卡住状态
                        if (!judgearea(aimpt, 13) && judgearea(FishPt, 13) && (FishDir > -60 && FishDir < 60))//鱼在第二个球门左面被卡住区域
                        {
                            std::cout << "一门左卡住" << FishDir << std::endl;
                            aAction[A].direction = 15;
                        }
                        else if (aimpt.y > middle&&aimpt.y<middledown&&aimpt.x>onedoorleft + 10 && aimpt.x < onedoorright)//球在门口的时候
                        {
                            std::cout << "球在门口的时候" << std::endl;
                            shopdistance = 23;
                            /*		if(FishPt.x<aimpt.x&&FishPt.y<aimpt.y+20)
                            		{
                            			deletezero(31);
                            			movingshot3(1,aimpt,aAction[A],A,aFish[A],doorpt,CPoint(aimpt.x,aimpt.y+10));
                            		}*/

                            //movingshot1(0,aimpt,aAction[A],A,aFish[A],doorpt,shotpt0);//对球最下面顶
                            if (FishPt.x < aimpt.x - 10)
                            {
                                deletezero(10);
                                movingshot1(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptup.x + 3, shotptup.y));//对球最下面顶
                            }
                            if (FishPt.x > aimpt.x + 10)
                            {
                                deletezero(10);
                                movingshot1(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptup.x - 3, shotptup.y));//对球最下面顶
                            }
                            if (FishPt.x <= aimpt.x + 10 && FishPt.x >= aimpt.x - 10)
                            {
                                deletezero(10);
                                movingshot1(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptup.x, shotptup.y));//对球最下面顶
                            }
                        }
                        else if (aimpt.y > 350 && aimpt.y < 450 && aimpt.x < onedoorleft)////在下左平面(靠下，在第一门左面）
                        {
                            deletezero(10);
                            std::cout << "aimpt.y-350-450" << std::endl;
                            //if(FishPt.x<aimpt.x&&FishPt.y<aimpt.y+30)
                            //{
                            //                       movingshot2(1,aimpt,aAction[A],A,aFish[A],doorpt,shotpt0);//对球最下面顶
                            //}
                            //if(FishPt.x>aimpt.x&&FishPt.y<aimpt.y+30)
                            //{
                            //                       movingshot2(0,aimpt,aAction[A],A,aFish[A],doorpt,shotpt0);//对球最下面顶
                            //}

                            movingshot1(0, aimpt, aAction[A], A, aFish[A], doorpt, shotpt0);//对球最下面顶

                        }
                        else if (aimpt.y < 480 && aimpt.y>450 && aimpt.x < onedoorright)////在下左平面(靠下，在第一门左面）
                        {
                            deletezero(20);
                            std::cout << "在最下面" << std::endl;
                            movingshot2(0, aimpt, aAction[A], A, aFish[A], doorpt, shotptup);//对球最下面顶
                        }
                        else if (aimpt.y > middledown&&aimpt.x > onedoorleft&&aimpt.x < onedoorright)////在下左平面(在第一门之间）
                        {
                            deletezero(10);
                            std::cout << "在第一门之间" << std::endl;
                            if (FishPt.x > aimpt.x&&FishPt.y < aimpt.y + 20)
                            {
                                movingshot2(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptup.x + 5, shotptup.y));//对球最下面顶
                            }
                            else
                            {
                                movingshot1(0, aimpt, aAction[A], A, aFish[A], doorpt, shotpt0);//对球最下面顶
                                //if(FishPt.x<aimpt.x-10)
                                //{
                                //	movingshot1(0,aimpt,aAction[A],A,aFish[A],doorpt,CPoint(shotptup.x+5,shotptup.y));//对球最下面顶
                                //}
                                //if(FishPt.x>aimpt.x+10)
                                //{
                                //	movingshot1(0,aimpt,aAction[A],A,aFish[A],doorpt,CPoint(shotptup.x,shotptup.y));//对球最下面顶
                                //}
                                //if(FishPt.x<=aimpt.x+10&&FishPt.x>=aimpt.x-10)
                                //{
                                //	movingshot1(0,aimpt,aAction[A],A,aFish[A],doorpt,CPoint(shotptup.x,shotptup.y));//对球最下面顶
                                //}
                            }
                        }
                        else if (aimpt.y > middle&&aimpt.y < middledown + 30 && aimpt.x < onedoorleft + 10)//在下左平面(靠上，在第一门左面)
                        {
                            deletezero(31);
                            if (aimpt.x < 1 * onedoorleft / 3)
                            {
                                std::cout << "向右对球下面顶" << std::endl;
                                movingshot3(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - 3, aimpt.y + 13));
                            }
                            else if (aimpt.x < 2 * onedoorleft / 3)
                            {
                                std::cout << "向右对球下面顶" << std::endl;
                                movingshot3(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - 10, aimpt.y + 8));
                            }
                            else
                            {
                                std::cout << "向右对球最下面顶" << std::endl;
                                movingshot3(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - 8, aimpt.y + 10));
                            }
                        }
                        else
                        {
                            deletezero(10);
                            std::cout << "对球最下面顶" << std::endl;
                            if (FishPt.x < aimpt.x - 10)
                            {
                                movingshot1(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptup.x + 5, shotptup.y));//对球最下面顶
                            }
                            if (FishPt.x > aimpt.x + 10)
                            {
                                movingshot1(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptup.x - 5, shotptup.y));//对球最下面顶
                            }
                            if (FishPt.x <= aimpt.x + 10 && FishPt.x >= aimpt.x - 10)
                            {
                                movingshot1(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptup.x, shotptup.y));//对球最下面顶
                            }
                            //movingshot5(0,aimpt,aAction[A],A,aFish[A],doorpt,shotptup);//对球最下面顶
                        }
                    }
                    else if (aimpt.y > middle&&aimpt.x >= onedoorright)//球在一球门右
                    {
                        //优先判断是否处于卡住状态
                        if ((aimpt.y > middle + 50 && aimpt.y<middledown&&aimpt.x>twodoorleft&&aimpt.x < twodoorright) && (FishDir > -60 && FishDir < 60))//鱼在第二个球门左面被卡住区域
                        {
                            std::cout << "二门左卡住" << std::endl;
                            aAction[A].direction = 15;
                        }
                        else if ((aimpt.y > middle&&aimpt.x < twodoorright) && (FishPt.y > middle + 50 && FishPt.y < middledown&&FishPt.x<twodoorright + 100 && FishPt.x>twodoorright) && (FishDir > 130 || FishDir < -130))//球在二球门左,鱼在右
                        {
                            std::cout << "二门右卡住" << std::endl;
                            aAction[A].direction = 15;
                        }
                        else if (aimpt.y > middle&&aimpt.y < middledown + 20 && aimpt.x<width / 2 && aimpt.x>onedoorright)//卡球区域
                        {
                            deletezero(20);
                            std::cout << "对着最上面击球" << std::endl;
                            movingshot2(0, aimpt, aAction[A], A, aFish[A], doorpt, shotptdown);//对球最右面顶
                        }
                        else if (aimpt.y > middledown + 20 && aimpt.x<width / 2 && aimpt.x>onedoorright)//在第一球门右面击球区域
                        {
                            deletezero(20);
                            std::cout << "在第一球门右面击球区域" << std::endl;
                            if (aimpt.y > 350)
                            {
                                movingshot2(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x + radius, aimpt.y + 5));//对球最右面顶
                            }
                            else
                            {
                                movingshot2(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x + radius, aimpt.y + 8));//对球最右面顶
                            }
                        }
                        else if (aimpt.y > middle&&aimpt.x >= onedoorright)
                        {
                            deletezero(20);
                            std::cout << "正常区域" << std::endl;
                            movingshot2(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x + radius, aimpt.y));//对球最右面顶
                        }
                    }
                }
            }
            //middle=middle+30;
            if (judgeareaup(FishPt, 4))//鱼在上平面
            {
                std::cout << "left fish 1->2" << middle << std::endl;
                if (fabs(diraimpttodoor) < 20)
                {
                    std::cout << "鱼在上平面直游" << std::endl;
                    aAction[A].direction = 7;
                }
                else
                {
                    spinP2PMove(doorpt, aFish[A], aAction[A]);
                }
            }
            if (judgeareaup(FishPt, 5))//鱼在上平面
            {
                std::cout << "right fish 1->2" << std::endl;
                spinP2PMove(doorpt1, aFish[A], aAction[A]);
            }

        }
        //鱼顶右面球门的策略
        if (judgeball(aimpt) == 2)
        {
            std::cout << "A鱼目标为二号门" << std::endl;
            if (judgeareaup(FishPt, 1))//鱼在上平面
            {
                middle = middle + 50;
                if (judgeareaup(aimpt, 1))//球在上平面
                {
                    /*if(!(judgeareaup(aimpt,12)||judgeareaup(aimpt,15))&&(judgeareaup(FishPt,12)||judgeareaup(FishPt,15))&&FishDir>-150&&FishDir<-30)
                    {
                    std::cout<<"鱼从下面下来了"<<std::endl;
                    aAction[A].direction=7;
                    }*/
                    if (aimpt.y < middle&&aimpt.x < onedoorright + 10)//球在一球门以左
                    {
                        if (aimpt.y<middle&&aimpt.y>middleup - 30)//在一门区域向左靠下
                        {
                            deletezero(21);
                            std::cout << "在一门区域向左靠下" << std::endl;
                            movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x - 10, aimpt.y + 3));
                        }
                        else if (aimpt.y < middleup - 30 && aimpt.y>100)
                        {
                            deletezero(21);
                            std::cout << "一球门以左中间" << std::endl;
                            if (FishPt.y < aimpt.y)
                            {
                                movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x - 10, aimpt.y - 5));
                            }
                            else
                            {
                                movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x - 10, aimpt.y + 5));
                            }
                        }
                        else
                        {
                            deletezero(21);
                            std::cout << "在一门区域向左靠上" << std::endl;
                            movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x - 10, aimpt.y));
                        }
                    }
                    else if (aimpt.y<middle&&aimpt.x>onedoorright + 10 && aimpt.x < twodoorleft)//一球门以右，二球门以左
                    {
                        if (aimpt.y<middle&&aimpt.x>onedoorright&&aimpt.x < twodoorleft&&aimpt.y < 80)
                        {
                            deletezero(21);
                            std::cout << "一门二门靠上" << std::endl;
                            movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x - 10, aimpt.y - 5));
                        }
                        else if (aimpt.y<middle&&aimpt.x>onedoorright&&aimpt.x < twodoorleft - 50 && aimpt.y < 160 && aimpt.y >= 80)
                        {
                            std::cout << "一门二门中间" << std::endl;
                            if (FishPt.x<aimpt.x&&FishPt.y>aimpt.y - 60)
                            {
                                deletezero(21);
                                movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x, aimpt.y - 10));
                            }
                            else
                            {
                                if (FishPt.x > aimpt.x)
                                {
                                    deletezero(11);
                                    movingshot1(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x - 8, aimpt.y - 13));
                                }
                                else
                                {
                                    deletezero(11);
                                    movingshot1(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x - 8, aimpt.y - 8));
                                }
                            }
                        }
                        else
                        {
                            deletezero(21);
                            std::cout << "一门二门靠下" << std::endl;
                            if (aimpt.y > middle&&aimpt.y < middle - 40)
                            {
                                if (aimpt.x < twodoorleft - 20)
                                {
                                    movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x - 8, aimpt.y - 5));
                                }
                                else
                                {
                                    movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x - 8, aimpt.y - 15));
                                }
                            }
                            else
                            {
                                if (aimpt.x < width / 2)
                                {
                                    movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x - 8, aimpt.y - 12));
                                }
                                else
                                {
                                    movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x, aimpt.y - 12));
                                }
                            }

                        }
                    }
                    else if (aimpt.y<middle&&aimpt.x>twodoorleft&&aimpt.x < twodoorright)//两球门之间
                    {

                        std::cout << "两球门之间" << std::endl;
                        if (aimpt.y > 170)//已经在球门口了
                        {
                            if (FishPt.x<aimpt.x&&FishPt.y>aimpt.y - 30)
                            {
                                deletezero(21);
                                movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - 5, aimpt.y - 16));
                            }
                            if (FishPt.x > aimpt.x&&FishPt.y > aimpt.y - 30)
                            {
                                deletezero(20);
                                movingshot2(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x + 5, aimpt.y - 16));
                            }
                            else
                            {
                                deletezero(11);
                                shopdistance = 23;
                                if (FishPt.x < aimpt.x - 10)
                                {
                                    movingshot1(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptdown.x + 5, shotptdown.y));//对球最下面顶
                                }
                                if (FishPt.x > aimpt.x + 10)
                                {
                                    movingshot1(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptdown.x - 5, shotptdown.y));//对球最下面顶
                                }
                                if (FishPt.x <= aimpt.x + 10 && FishPt.x >= aimpt.x - 10)
                                {
                                    movingshot1(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptdown.x, shotptdown.y));//对球最下面顶
                                }
                            }
                        }
                        else if (aimpt.y < 60)
                        {
                            deletezero(21);
                            movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - radius, aimpt.y - 10));
                            /*if(FishPt.x<aimpt.x)
                            {
                            std::cout<<"左击"<<std::endl;
                            deletezero(21);
                            movingshot5(1,aimpt,aAction[A],A,aFish[A],doorpt,CPoint(shotptdown.x,shotptdown.y));
                            }
                            else
                            {
                            std::cout<<"右击"<<std::endl;
                            deletezero(20);
                            movingshot5(0,aimpt,aAction[A],A,aFish[A],doorpt,CPoint(shotptdown.x,shotptdown.y));
                            }*/
                        }
                        else
                        {
                            if (FishPt.x < aimpt.x - 10)
                            {
                                movingshot1(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptdown.x + 5, shotptdown.y));//对球最下面顶
                            }
                            if (FishPt.x > aimpt.x + 10)
                            {
                                movingshot1(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(shotptdown.x - 5, shotptdown.y));//对球最下面顶
                            }
                            if (FishPt.x <= aimpt.x + 10 && FishPt.x >= aimpt.x - 10)
                            {
                                movingshot1(1, aimpt, aAction[A], A, aFish[A], doorpt, shotpt1);//对球最下面顶
                            }
                        }
                    }
                    else if (aimpt.y<middle&&aimpt.x>twodoorright)//二球门以右
                    {
                        if (aimpt.y<middle&&aimpt.y>middleup - 30)//二球门右靠下
                        {
                            deletezero(30);
                            //std::cout<<"二球门右靠下"<<middleup<<std::endl;
                            if (FishPt.x > twodoorright + 20)
                            {
                                std::cout << "二球门右靠下偏右" << middleup << std::endl;
                                movingshot3(0, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x + 10, aimpt.y - 6));//对球最右面顶
                            }
                            else
                            {
                                std::cout << "二球门右靠下偏左" << middleup << std::endl;
                                movingshot3(0, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x + 10, aimpt.y - 15));//对球最右面顶
                            }
                        }
                        else
                        {
                            std::cout << "二球门右靠上" << middleup << std::endl;
                            if (aimpt.y < 80)
                            {
                                std::cout << "向右对最下面顶" << std::endl;
                                deletezero(21);
                                movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x - radius, aimpt.y - 8));//对球最右面顶
                            }
                            else
                            {
                                std::cout << "往下往最上面偏右顶" << std::endl;
                                deletezero(11);
                                movingshot1(1, aimpt, aAction[A], A, aFish[A], doorpt1, CPoint(aimpt.x + 3, aimpt.y - 10));//对球最右面顶
                            }
                        }
                    }
                    else
                    {
                        movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt1, shotpt1);//对球最右面顶
                    }

                }
            }
            static int uint = 0;
            if (judgearea(FishPt, 7))//鱼在下平面
            {
                std::cout << "left fish 2->1" << std::endl;
                if (FishPt.x > onedoorleft && (FishDir > 0 || FishDir < -120))
                {
                    std::cout << "右转" << std::endl;
                    aAction[A].speed = speedA;
                    aAction[A].direction = 15;
                }
                else if (FishPt.x > twodoorright + 30 && FishPt.y < doorpt1.y + 30 && stop0 == 0 && stop1 == 0)
                {

                    std::cout << "xia" << std::endl;
                    spinP2PMove(CPoint(doorpt1.x, doorpt1.y + 400), aFish[A], aAction[A]);
                }
                else
                {
                    if (aimpt.x > doorpt.x)
                    {
                        std::cout << "偏右" << std::endl;
                        spinP2PMove(CPoint(doorpt.x - 5, doorpt.y), aFish[A], aAction[A]);
                    }
                    else
                    {
                        std::cout << "偏左" << std::endl;
                        spinP2PMove(CPoint(doorpt.x - 15, doorpt.y), aFish[A], aAction[A]);
                    }
                    //Roundp2p(doorpt,aAction[A],A,aFish[A]);
                }

            }
            else if (stop0 == 0 && stop1 == 0 && judgearea(FishPt, 8))//鱼在下平面
            {
                std::cout << "right fish 2->1" << std::endl;
                if (FishPt.x > twodoorleft && (FishDir > 0 || FishDir < -120))
                {
                    std::cout << "右转" << std::endl;
                    aAction[A].speed = speedA;
                    aAction[A].direction = 15;
                }
                else
                {
                    //std::cout<<"right fish 2->1"<<std::endl;
                    spinP2PMove(doorpt1, aFish[A], aAction[A]);
                }
            }
        }

        //最终进球进门
        if (judgeball(aimpt) == 3)
        {
            std::cout << "A鱼目标为三号门" << std::endl;
            if (stop0 == 0 && stop1 == 0 && judgearea(FishPt, 2))//鱼在下平面
            {
                middle = middle - 50;
                if (judgearea(aimpt, 2))//球在下平面
                {
                    if (judgearea(aimpt, 18))//在一球门左
                    {
                        //优先判断是否处于卡住状态
                        if (judgearea(FishPt, 13) && (FishDir > -60 && FishDir < 60))//鱼在第二个球门左面被卡住区域
                        {
                            std::cout << "一门左卡住" << FishDir << std::endl;
                            aAction[A].direction = 15;
                        }
                        else if (judgearea(aimpt, 18) && judgearea(FishPt, 14) && (FishDir > 130 || FishDir < -130))//球在一球门左,鱼在右
                        {
                            std::cout << "一门右卡住" << std::endl;
                            aAction[A].direction = 15;
                            aAction[A].speed = speedA;
                        }
                        else
                        {
                            deletezero(21);
                            std::cout << "向右对球的上面顶" << std::endl;
                            movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - radius, aimpt.y - 3));//对球最下面顶
                        }

                    }
                    else if (judgearea(aimpt, 19))//球在一球门右
                    {
                        //优先判断是否处于卡住状态
                        if (judgearea(FishPt, 12) && (FishDir > -60 && FishDir < 60))//鱼在第二个球门左面被卡住区域
                        {
                            std::cout << "二门左卡住" << std::endl;
                            aAction[A].direction = 15;
                        }
                        else if (judgearea(aimpt, 16) && judgearea(FishPt, 15) && (FishDir > 130 || FishDir < -130))//球在二球门左,鱼在右
                        {
                            std::cout << "二门右卡住" << std::endl;
                            aAction[A].direction = 15;
                        }
                        else if (aimpt.y > middle&&aimpt.y < middledown + 10 && aimpt.x<twodoorright&&aimpt.x>onedoorright)
                        {
                            deletezero(21);
                            std::cout << "卡球区域" << std::endl;
                            /*					if(FishPt.x<onedoorright)
                            					{
                            						Roundp2p(CPoint(onedoorright,350),aAction[A],A,aFish[A]);
                            					}*/
                            movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - 10, aimpt.y - 13));//对球最右面顶
                        }
                        else if (aimpt.y > 430)
                        {
                            deletezero(21);
                            std::cout << "球到下面" << std::endl;
                            movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - 10, aimpt.y + 10));
                        }

                        else if (aimpt.x < twodoorright&&aimpt.y<440 && aimpt.y>middledown + 10)
                        {
                            deletezero(21);
                            std::cout << "击球区域" << std::endl;
                            if (FishPt.y < aimpt.x)
                            {
                                movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - 13, aimpt.y));//对球最右面顶
                            }
                            else
                            {
                                movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt, shotpt3);//对球最右面顶
                            }
                        }
                        else if (aimpt.y > doorpt3.y+(aChannel[2].width)/2 - 7 && aimpt.x > twodoorright + 30)
                        {
                            deletezero(10);
                            //std::cout<<"从下往上击球"<<std::endl;
                            //////---------------------//////
                            if (aimpt.x > doorpt3.x-30)
                            {
                                std::cout << "从上往下击球0000" << std::endl;
                                movingshot1(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x + 5, aimpt.y - radius));//对球右上顶
                            }
                            else
                            {
                                std::cout << "从下往上击球" << std::endl;
                                //movingshot1(0,aimpt,aAction[A],A,aFish[A],doorpt,shotpt3);//对球最右面顶
                                if (FishPt.x > aimpt.x)
                                {
                                    movingshot1(0, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - 13, aimpt.y + 8));//对球最右面顶
                                }
                                else
                                {
                                    movingshot1(0, aimpt, aAction[A], A, aFish[A], doorpt, shotpt3);//对球最右面顶
                                }
                            }
                        }
                        else if (aimpt.y<doorpt3.y + (aChannel[2].width) / 2 - 7 && aimpt.x>twodoorright)
                        {
                            deletezero(21);
                            std::cout << "向右击球" << std::endl;
                            if (FishPt.y < aimpt.y)
                            {
                                movingshot3(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - 15, aimpt.y + 12));//对球最右面顶
                            }
                            else
                            {
                                movingshot3(1, aimpt, aAction[A], A, aFish[A], doorpt, CPoint(aimpt.x - 15, aimpt.y + 8));//对球最右面顶
                            }
                        }
                        else if (aimpt.y > doorpt3.y + (aChannel[2].width) / 2 - 7 && aimpt.x < twodoorright + 30)
                        {
                            deletezero(21);
                            std::cout << "向右击球" << std::endl;
                            movingshot2(1, aimpt, aAction[A], A, aFish[A], doorpt, shotpt3);//对球最右面顶
                        }
                    }
                }
            }
            if (judgeareaup(FishPt, 4))//鱼在上平面
            {
                std::cout << "left fish 1->2" << middle << std::endl;
                spinP2PMove(doorpt, aFish[A], aAction[A]);
            }
            if (judgeareaup(FishPt, 5))//鱼在上平面
            {
                std::cout << "right fish 1->2" << std::endl;
                spinP2PMove(doorpt1, aFish[A], aAction[A]);
            }
        }
    }
    else		//--A鱼先静止
    {
        std::cout << "A鱼静止" << std::endl;
        aAction[A].state = 0;
        aAction[A].mode = 0;
        aAction[A].speed = 0;
    }
    std::cout << "speed" << speed << std::endl << std::endl;
    return TRUE;
}