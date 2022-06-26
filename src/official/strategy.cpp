#include "../official/strategy.h"
#include "../official/externrobot.h"
#include "../mathtools.h"
#include <iostream>

int  width=755;//水池的宽
int  highth=485; //高
int  doorwidth=45;//球门宽度
int  middle;  //中线
int  middledown;//下中线
int  middleup;//上中线
int  onedoorright; //第一个门右面
int  onedoorleft;  //第一个门左面
int  twodoorleft ; //第二个门左面
int  twodoorright; //第二个门右面
int  finaldoordown;//最 300//最后门y下限
int  finaldoordown1;
int  finaldoorx;  //最后门x下限
int  radus=13;  //球半径
int speed3=9;
int speedB=9;
int  turndistance=30;
int  shopdistance=18;
int  distance0=200;
#define A  0
#define B  1
static int ptopflag=0;
static int  flag1=0,flag2=0,flag3=0,flag4=0,flag5=0,flag6=0,flag7=0,flag8=0,flag9=0,flag10=0,flag11=0,flag12=0,flag13=0,findpoint=0;
int Q=0,W=0,E=0;

CStrategy::CStrategy(void)
{
    m_StatePeriod=15;
    m_TurnYuan=0;
}

BOOL CStrategy::Strategy(IplImage* imgOrig,IplImage* imgRecog,CFishAction action[], int n_action, CFishInfo fishinfo[], int n_fishinfo,
                         CBallInfo ballinfo[], int n_ballinfo, OBSTAINFO obst[],int n_obst, CHANNEL channel[], int n_channel)
{
    return TRUE;
}

//以下id及鱼信息、鱼动作信息数组相关，取值说明：
//0-7是A队的，8-15是B队的;
//例如，A队的第一条鱼的动作是actions[0]，B队的第一条鱼的动作是actions[8]
void initdata(CHANNEL m_Channel[])
{
    middle=m_Channel[0].center.y;
    middledown=middle+35;
    middleup=middle-35;
    onedoorleft=m_Channel[0].center.x-40;
    onedoorright=m_Channel[0].center.x+40;
    twodoorleft=m_Channel[1].center.x-40;
    twodoorright=m_Channel[1].center.x+40;
    finaldoorx=m_Channel[2].center.x;
    finaldoordown=middle-80;
    finaldoordown1=finaldoordown-35;
}

void CStrategy::BasicActionStop(CFishInfo fish[],CFishAction action[],int id)
{
    action[id].speed=0;
    action[id].direction=7;
    action[id].mode=0;

    action[id].state++;
    if(action[id].state==m_StatePeriod)
    {
        action[id].state=0;
        m_TurnYuan=0;
    }

    fish[id].SetAction(action[id]);//把某条鱼的action设置为制定的action
    //Fishaction 输入 待设定的action
}

double Distance(CPoint point,CPoint aimer)
{
    double a;
    a=sqrt((double)(point.x-aimer.x)*(point.x-aimer.x)+(double)(point.y-aimer.y)*(point.y-aimer.y));
    return a;
}

double Checkangle(double dir)//////////////////////////把dir换算成[-pi~pi]之间的数
{
    if(dir>M_PI)
        dir-=2*M_PI;
    else if(dir<-M_PI)
        dir+=2*M_PI;
    return dir;
}

double Angle(CPoint point,CPoint aimer)
{
    double a;
    a=atan2((double)(aimer.y-point.y),(double)(aimer.x-point.x));
    return a;
    //return point.x;
}

double Checkangle0(double dir)//////////////////////////把dir换算成[-180~180]之间的数
{
    if(dir>180)
        dir-=2*180;
    else if(dir<-180)
        dir+=2*180;
    return dir;
}

double Checkangle1(double dir)//把（-180，180）的数变成0-360;
{
    if(dir>-180&&dir<0)
        dir+=2*180;
    else
        dir=dir;
    return dir;
}

void CStrategy::BasicActionGo(CFishInfo fish[],CFishAction action[],int id,int speed,int direction,int mode)
{
    action[id].speed=speed;
    action[id].direction=direction;
    action[id].mode=mode;

    if(m_TurnYuan)//正在执行原始转弯模式
    {
        action[id].state++;
        if(action[id].state>=m_StatePeriod)
        {
            action[id].state=0;
            m_TurnYuan=0;
        }
    }
    else
    {
        if(mode)
        {
            action[id].state=0;
            m_TurnYuan=mode;
        }
        else
        {
            action[id].state++;
            if(action[id].state>=m_StatePeriod)
                action[id].state=0;
        }
    }

    fish[id].SetAction(action[id]);
}

bool judgeareaup(CPoint point, int i) //上平面区域判定
{
    switch(i)
    {
    case 1://上平面
        if(point.y<middle)
            return 1;
        else
            return 0;
    case 4://在上左平面
        if(point.y<middle&&point.x<width/2)
            return 1;
        else
            return 0;
    case 5://在上右平面
        if(point.y<middle&&point.x>width/2)
            return 1;
        else
            return 0;
    case 6://在一门右向左区域，在靠下
        if(point.y<middle&&point.y>middleup-30&&point.x<onedoorright+20)
            return 1;
        else
            return 0;
    case 7://二球门左,右之间
        if(point.y<middle&&point.x>twodoorleft&&point.x<twodoorright)
            return 1;
        else
            return 0;
    case 8://二球门右
        if(point.y<middle&&point.x>twodoorright)
            return 1;
        else
            return 0;
    case 9://一球门右二球门左
        if(point.y<middle&&point.x>onedoorright+20&&point.x<twodoorleft)
            return 1;
        else
            return 0;
    case 10://一球门右以左
        if(point.y<middle&&point.x<onedoorright+20)
            return 1;
        else
            return 0;
        return 0;
    case 11://二球门右靠下
        if(point.y<middle-50&&point.y>middleup-30&&point.x>twodoorright)
            return 1;
        else
            return 0;
    case 12://球门区域（一二球门之间）
        if(point.y<middle-50&&point.y>middleup&&point.x<twodoorright&&point.x>twodoorleft)
            return 1;
        else
            return 0;
    case 13://一球门右中门以左
        if(point.y<middle&&point.x>onedoorright&&point.x<width/2)
            return 1;
        else
            return 0;
    case 14://中门以右二左门以左靠上
        if(point.y<middleup-30&&point.x>width/2&&point.x<twodoorleft)
            return 1;
        else
            return 0;
    case 15://球门区域（一二球门之间）
        if(point.y<middle-50&&point.y>middleup&&point.x<onedoorright&&point.x>onedoorleft)
            return 1;
        else
            return 0;
    default:
        return false;
    }
}

bool judgearea(CPoint point, int i) //下平面区域判断
{
    switch(i)
    {
    case 2://下平面
        if(point.y>middle)
            return 1;
        else
            return 0;
    case 3://在下区域卡球
        if(point.y>middle&&point.y<middledown&&point.x<width/2&&point.x>onedoorright)
            return 1;
        else
            return 0;
    case 7://在下左平面
        if(point.y>highth/2&&point.x<width/2)
            return 1;
        else
            return 0;
    case 8://在下右平面
        if(point.y>highth/2&&point.x>width/2)
            return 1;
        else
            return 0;
    case 9://在下左平面(靠下，在第一门左面）
        if(point.y>350&&point.y<450&&point.x<onedoorleft)
            return 1;
        else
            return 0;
    case 10://在下左平面(在第一门之间）
        if(point.y>middledown&&point.x>onedoorleft&&point.x<onedoorright)
            return 1;
        else
            return 0;
    case 11://在下左平面(靠上，在第一门左面）
        if(point.y>middle+50&&point.y<middledown+30&&point.x<onedoorleft+10)
            return 1;
        else
            return 0;
    case 12://在下右平面(在第二门之间,middle和middledown之间)
        if(point.y>middle+50&&point.y<middledown&&point.x>twodoorleft&&point.x<twodoorright)
            return 1;
        else
            return 0;
    case 13://在下左平面(在第一门之间,middle和middledown之间)
        if(point.y>middle&&point.y<middledown&&point.x>onedoorleft+10&&point.x<onedoorright)
            return 1;
        else
            return 0;
    case 14://在下左平面(在第一门偏右,middle和middledown之间)
        if(point.y>middle+50&&point.y<middledown&&point.x<onedoorright+100&&point.x>onedoorright)
            return 1;
        else
            return 0;
    case 15://在下右平面(在第二门偏右,middle和middledown之间)
        if(point.y>middle+50&&point.y<middledown&&point.x<twodoorright+100&&point.x>twodoorright)
            return 1;
        else
            return 0;
    case 16://在下右平面(二球门左)
        if(point.y>middle&&point.x<twodoorright)
            return 1;
        else
            return 0;
    case 17://在下右平面(二球门右)
        if(point.y>middle&&point.x>=twodoorright)
            return 1;
        else
            return 0;
    case 18://在下右平面(一球门左)
        if(point.y>middle&&point.x<onedoorright)
            return 1;
        else
            return 0;
    case 19://在下右平面(一球门右)
        if(point.y>middle&&point.x>=onedoorright)
            return 1;
        else
            return 0;
    case 20://在最后门下面一点点
        if(point.y>finaldoordown1&&point.y<finaldoordown&&point.x>610&&point.x<finaldoorx)
            return 1;
        else
            return 0;
    case 21://在最后门上面一条线
        if(point.y<finaldoordown1&&point.y>middle&&point.x>finaldoorx&&point.x<finaldoorx)
            return 1;
        else
            return 0;
    case 22://在第一球门左面击球区域
        if(point.y>middledown&&point.x<width/2&&point.x>onedoorright)
            return 1;
        else
            return 0;
    case 23://最后射门死区
        if(point.y>middle&&point.y<finaldoordown1&&point.x>finaldoorx)
            return 1;
        else
            return 0;
    case 24://理想最后射门区域
        if(point.y>middle&&point.x<finaldoorx)
            return 1;
        else
            return 0;


    //最后球门的区域
    case 25://一二球门之间靠上
        if(point.y>middle+50&&point.y<middledown&&point.x<twodoorright&&point.x>onedoorright)
            return 1;
        else
            return 0;
    case 26:
        if(point.y>middledown+50&&point.x>twodoorleft&&point.x<finaldoorx)
            return 1;
        else
            return 0;
    case 27:
        if(point.y>350&&point.x>finaldoorx)
            return 1;
        else
            return 0;
    case 28:
        if(point.y<350&&point.x>finaldoorx)
            return 1;
        else
            return 0;


    case 29://在下右平面(在第二门之间,middle和middledown之间)
        if(point.y>middle&&point.y<middledown&&point.x>twodoorleft&&point.x<twodoorright)
            return 1;
        else
            return 0;
    case 30://在下左平面(在第一门偏右,middle和middledown之间)
        if(point.y>middle&&point.y<middledown+20&&point.x<onedoorright+100&&point.x>onedoorright)
            return 1;
        else
            return 0;
    case 31://在下右平面(在第二门偏右,middle和middledown之间)
        if(point.y>middle&&point.y<middledown&&point.x<twodoorright+100&&point.x>twodoorright)
            return 1;
        else
            return 0;
    case 32://在下左平面(在第一门之间,middle和middledown之间)
        if(point.y>middle&&point.y<middledown&&point.x>onedoorleft&&point.x<onedoorright)
            return 1;
        else
            return 0;
    case 33://在下左平面(靠下，在第一门左面）
        if(point.y<480&&point.y>450&&point.x<onedoorleft)
            return 1;
        else
            return 0;
    default:
        return false;
    }
}

/**
 * @brief 判断球当前的状态
 * @note 返回值表示的是鱼当前准备进攻几号门
 */
int judgeball(CPoint point) //判断球的状态
{
    Q=1;
    if(point.y<middle-10&&point.x<onedoorright+20)
    {
        W=1;
    }
    if(W==1&&judgearea(point,32))//回退到一
    {
        W=0;
    }
    if(W==1&&(point.y>middle+10&&point.y<middledown+10&&point.x>twodoorleft-10&&point.x<twodoorright+10))
    {
        E=1;
    }
    if(E==1&&judgeareaup(point,7)) //回退到二
    {
        E=0;
    }
    if(Q==1&&W==0&&E==0)return 1;
    if(Q==1&&W==1&&E==0)return 2;
    if(Q==1&&W==1&&E==1)return 3;
    assert(false);
    return 0;
}

void deletezero(int mode)
{
    if(mode==10)
    {
        flag3=0;
        flag4=0;
        flag5=0;
        flag6=0;
        flag7=0;
        flag8=0;
        flag9=0;
        flag10=0;
        flag11=0;
        flag12=0;
    }
    if(mode==11)
    {
        flag3=0;
        flag4=0;
        flag5=0;
        flag6=0;
        flag1=0;
        flag2=0;
        flag9=0;
        flag10=0;
        flag11=0;
        flag12=0;
    }
    if(mode==20)
    {
        flag1=0;
        flag2=0;
        flag5=0;
        flag6=0;
        flag7=0;
        flag8=0;
        flag9=0;
        flag10=0;
        flag11=0;
        flag12=0;
    }
    if(mode==21)
    {
        flag3=0;
        flag4=0;
        flag1=0;
        flag2=0;
        flag7=0;
        flag8=0;
        flag9=0;
        flag10=0;
        flag11=0;
        flag12=0;
    }
    if(mode==31)
    {
        flag5=0;
        flag6=0;
        flag3=0;
        flag4=0;
        flag1=0;
        flag2=0;
        flag7=0;
        flag8=0;
        flag11=0;
        flag12=0;
    }
    if(mode==30)
    {
        flag9=0;
        flag10=0;
        flag5=0;
        flag6=0;
        flag3=0;
        flag4=0;
        flag1=0;
        flag2=0;
        flag7=0;
        flag8=0;
    }
}

CPoint getpoint(int x,int y)
{
    CPoint aimpt;
    aimpt.x=x;
    aimpt.y=y;
    return aimpt;
}

void CStrategy::ptop(CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo)
{
    CPoint FishPt;// 鱼中心的位置
    double FishDir;// 鱼头的方向
    CPoint FishHeadPt;//鱼头的位置
    CPoint LastFishPt;
    CPoint FishRotatePt;//鱼的转动位置
    CPoint FishTailPt;//鱼尾巴的位置
    CPoint GoalPt1, GoalPt2;//临时目标点，GoalPt2没有用到
    CPoint Pt1, Pt2, Pt3, Pt4;
    //CPoint f_headpt;//point of fish's head
    //f_headpt=m_FishInfo[0].GetHeadPoint();

    double dir1 = 0;
    double dir0;
    double dist0;
    double dist1 = 0;
    double dir2 = 0;
    double dist2 = 0;//临时用到的方向和距离
    double dis1, dis2;
    dis1 = 0;
    dis2 = 0;
    double radius;///需要游动的半径,可以随意的设置.
    double fishdir = FishDir;
    FishPt = m_FishInfo.GetCenterPoint();
    FishDir = m_FishInfo.GetDirection();
    dist1 = Distance(FishPt, aimpt);
    dir1 = Angle(FishPt, aimpt);
    dir1 -= FishDir;
    dir1 = Checkangle(dir1) * 180 / M_PI;
    //action.speed = speed1;
    Roundp2p(aimpt,action,1, m_FishInfo);
    if(Distance(aimpt,FishPt)<30)
    {
        ptopflag=1;

    }
    else
    {
        ptopflag=0;
    }
}

void Roundp2pleft(CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo)
{
    CPoint FishPt;// 鱼中心的位置
    double FishDir;// 鱼头的方向
    CPoint FishHeadPt;//鱼头的位置
    CPoint LastFishPt;
    CPoint FishRotatePt;//鱼的转动位置
    CPoint FishTailPt;//鱼尾巴的位置
    CPoint GoalPt1, GoalPt2;//临时目标点，GoalPt2没有用到
    CPoint Pt1, Pt2, Pt3, Pt4;
    //CPoint f_headpt;//point of fish's head
    //f_headpt=m_FishInfo[0].GetHeadPoint();

    double dir1=0;
    double dir0;
    double dist0;
    double dist1=0;
    double dir2=0;
    double dist2=0;//临时用到的方向和距离
    CPoint centerpt1,centerpt2;//作为鱼要绕的中心来用
    CPoint centerpt3,centerpt4,centerpt5,centerpt6;
    CPoint centerpt0,centerpt00;
    double dis1,dis2;
    dis1=0;
    dis2=0;
    double radius;///需要游动的半径,可以随意的设置.
    FishPt=m_FishInfo.GetCenterPoint();
    FishDir=m_FishInfo.GetDirection();
    dist1=Distance(FishPt,aimpt);
    dir1=Angle(FishPt,aimpt);
    dir1-=FishDir;
    dir1=Checkangle(dir1)*180/M_PI;
    //action.speed=15;
    m_FishInfo.SetAction(action);
    if(dist1>100)
    {
        if(dir1>-5&&dir1<5)
            action.direction=7;
        else if(dir1<-5&&dir1>-10)
            action.direction=5;
        else if(dir1<-10&&dir1>-30)
            action.direction=4;
        else if(dir1<-30&&dir1>-50)
            action.direction=0;
        else if(dir1<-50&&dir1>-70)
            action.direction=0;
        else if(dir1<-70&&dir1>-90)
            action.direction=0;
        else if(dir1<-90)
            action.direction=0;
        else if(dir1>5&&dir1<10)
            action.direction=0;
        else if(dir1>10&&dir1<20)
            action.direction=0;
        else if(dir1>20&&dir1<40)
            action.direction=0;
        else if(dir1>40&&dir1<50)
            action.direction=0;
        else if(dir1>50&&dir1<80)
            action.direction=0;
        else if(dir1>80&&dir1<90)
            action.direction=0;
        else
            action.direction=0;
    }

    else
    {
        if(dir1>-5&&dir1<5)
            action.direction=7;
        else if(dir1<-5&&dir1>-20)
            action.direction=4;
        else if(dir1<-20&&dir1>-40)
            action.direction=1;
        else if(dir1<-40&&dir1>-60)
            action.direction=0;
        else if(dir1<-60&&dir1>-70)
            action.direction=0;
        else if(dir1<-70&&dir1>-90)
            action.direction=0;
        else if(dir1<-120)
            action.direction=0;
        else if(dir1>5&&dir1<20)
            action.direction=0;
        else if(dir1>20&&dir1<30)
            action.direction=0;
        else if(dir1>30&&dir1<50)
            action.direction=0;
        else if(dir1>50&&dir1<70)
            action.direction=0;
        else if(dir1>70&&dir1<90)
            action.direction=0;
        else if(dir1<120)
            action.direction=0;
        else
            action.direction=0;

    }
    m_FishInfo.SetAction(action);
}

void Roundp2pright(CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo)
{
    CPoint FishPt;// 鱼中心的位置
    double FishDir;// 鱼头的方向
    CPoint FishHeadPt;//鱼头的位置
    CPoint LastFishPt;
    CPoint FishRotatePt;//鱼的转动位置
    CPoint FishTailPt;//鱼尾巴的位置
    CPoint GoalPt1, GoalPt2;//临时目标点，GoalPt2没有用到
    CPoint Pt1, Pt2, Pt3, Pt4;
    //CPoint f_headpt;//point of fish's head
    //f_headpt=m_FishInfo[0].GetHeadPoint();

    double dir1=0;
    double dir0;
    double dist0;
    double dist1=0;
    double dir2=0;
    double dist2=0;//临时用到的方向和距离
    CPoint centerpt1,centerpt2;//作为鱼要绕的中心来用
    CPoint centerpt3,centerpt4,centerpt5,centerpt6;
    CPoint centerpt0,centerpt00;
    double dis1,dis2;
    dis1=0;
    dis2=0;
    double radius;///需要游动的半径,可以随意的设置.
    FishPt=m_FishInfo.GetCenterPoint();
    FishDir=m_FishInfo.GetDirection();
    dist1=Distance(FishPt,aimpt);
    dir1=Angle(FishPt,aimpt);
    dir1-=FishDir;
    dir1=Checkangle(dir1)*180/M_PI;
    //action.speed=15;
    m_FishInfo.SetAction(action);
    if(dist1>100)
    {
        if(dir1>-5&&dir1<5)
            action.direction=7;
        else if(dir1<-5&&dir1>-10)
            action.direction=5;
        else if(dir1<-10&&dir1>-30)
            action.direction=14;
        else if(dir1<-30&&dir1>-50)
            action.direction=14;
        else if(dir1<-50&&dir1>-70)
            action.direction=14;
        else if(dir1<-70&&dir1>-90)
            action.direction=14;
        else if(dir1<-90)
            action.direction=14;
        else if(dir1>5&&dir1<10)
            action.direction=9;
        else if(dir1>10&&dir1<20)
            action.direction=10;
        else if(dir1>20&&dir1<40)
            action.direction=12;
        else if(dir1>40&&dir1<50)
            action.direction=12;
        else if(dir1>50&&dir1<80)
            action.direction=14;
        else if(dir1>80&&dir1<90)
            action.direction=14;
        else
            action.direction=14;
    }

    else
    {
        if(dir1>-5&&dir1<5)
            action.direction=7;
        else if(dir1<-5&&dir1>-20)
            action.direction=14;
        else if(dir1<-20&&dir1>-40)
            action.direction=14;
        else if(dir1<-40&&dir1>-60)
            action.direction=14;
        else if(dir1<-60&&dir1>-70)
            action.direction=14;
        else if(dir1<-70&&dir1>-90)
            action.direction=14;
        else if(dir1<-120)
            action.direction=14;
        else if(dir1>5&&dir1<20)
            action.direction=10;
        else if(dir1>20&&dir1<30)
            action.direction=12;
        else if(dir1>30&&dir1<50)
            action.direction=13;
        else if(dir1>50&&dir1<70)
            action.direction=14;
        else if(dir1>70&&dir1<90)
            action.direction=14;
        else if(dir1<120)
            action.direction=14;
        else
            action.direction=14;

    }
    m_FishInfo.SetAction(action);
}

void CStrategy::movingshot1(int mode,CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo,CPoint doorpt,CPoint shotpt0)//垂直向上,下顶球
{
    CPoint FishPt;// 鱼中心的位置
    double FishDir,distfishtoaimpt,distfishtoheadpt,dirfishtoaimpt,dirfishtoheadpt,distballtodoor,dirballtodoor,diraimpttodoor;// 鱼头的方向
    CPoint FishHeadPt;//鱼头的位置
    CPoint FishTailPt;//鱼尾巴的位置
    CPoint templeft0,templeft1,templeft2,tempright0,tempright1,tempright2;//临时击球点
    FishPt=m_FishInfo.GetCenterPoint();//鱼中心点坐标
    FishHeadPt = m_FishInfo.GetHeaderPoint();//鱼头坐标
    FishDir=Angle(FishPt,FishHeadPt);
    FishDir=Checkangle(FishDir);
    FishDir=FishDir*180/M_PI;//鱼的角度
    //FishDir=Checkangle1(FishDir);
    distfishtoaimpt=Distance(FishPt,aimpt);//球到鱼中心的距离
    distfishtoheadpt=Distance(FishPt,FishHeadPt);//球到鱼头的距离(有问题不可以使用）
    dirfishtoheadpt=Angle(aimpt,FishHeadPt);
    dirfishtoheadpt=Checkangle(dirfishtoheadpt)*180/M_PI;//球到鱼头的角度
    //dirfishtoheadpt=Checkangle1(dirfishtoheadpt);
    dirfishtoaimpt=Angle(aimpt,FishPt);
    dirfishtoaimpt=Checkangle(dirfishtoaimpt)*180/M_PI;//球到鱼中心的角度
    //dirfishtoaimpt=Checkangle1(dirfishtoaimpt);
    distballtodoor=Distance(aimpt,doorpt);//球到门的距离
    dirballtodoor=Angle(shotpt0,doorpt);
    dirballtodoor=Checkangle(dirballtodoor)*180/M_PI;//击球点到球门的角度
    //dirballtodoor=Checkangle1(dirballtodoor);
    diraimpttodoor=Angle(aimpt,doorpt);
    diraimpttodoor=Checkangle(diraimpttodoor)*180/M_PI;//球到球门的角度
    //diraimpttodoor=Checkangle1(diraimpttodoor);
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
            Roundp2p(shotpt0,action,0,m_FishInfo);
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
            if((FishDir>-180&&FishDir<0)&&Distance(FishPt,aimpt)<120&&findpoint==0)
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
                Roundp2p(templeft1,action,0,m_FishInfo);
                if(Distance(FishPt,templeft1)<turndistance)
                {
                    findpoint=0;
                    flag1=0;
                }
            }
        }
        if(flag2==1)
        {
            flag1=0;
            if((FishDir>-180&&FishDir<0)&&Distance(FishPt,aimpt)<120&&findpoint==0)
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
                Roundp2p(tempright1,action,0,m_FishInfo);
                if(Distance(FishPt,tempright1)<turndistance)
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
            Roundp2p(shotpt0,action,0,m_FishInfo);
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
            if((FishDir>0||FishDir<-150)&&Distance(FishPt,aimpt)<120&&findpoint==0)
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
                Roundp2p(templeft2,action,0,m_FishInfo);
                if(Distance(FishPt,templeft2)<turndistance)
                {
                    findpoint=0;
                    flag7=0;
                }
            }

        }
        if(flag8==1)
        {
            flag7=0;
            if((FishDir>-30||FishDir<-170)&&Distance(FishPt,aimpt)<120&&findpoint==0)
            {
                std::cout<<"左行"<<std::endl;
                action.direction=1;
                //Roundp2pleft(tempright2,action,0,m_FishInfo);
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                Roundp2p(tempright2,action,0,m_FishInfo);
                if(Distance(FishPt,tempright2)<turndistance)
                {
                    findpoint=0;
                    flag8=0;
                }
            }
        }
    }
}

void CStrategy::movingshot2(int mode,CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo,CPoint doorpt,CPoint shotpt0)//垂直向左，向右顶球
{
    CPoint FishPt;// 鱼中心的位置
    double FishDir,distfishtoaimpt,distfishtoheadpt,dirfishtoaimpt,dirfishtoheadpt,distballtodoor,dirballtodoor,diraimpttodoor;// 鱼头的方向
    CPoint FishHeadPt;//鱼头的位置
    CPoint FishTailPt;//鱼尾巴的位置
    CPoint tempup0,tempup1,tempup2,tempdown0,tempdown1,tempdown2;//临时击球点
    FishPt=m_FishInfo.GetCenterPoint();//鱼中心点坐标
    FishHeadPt = m_FishInfo.GetHeaderPoint();//鱼头坐标
    FishDir=Angle(FishPt,FishHeadPt);
    FishDir=Checkangle(FishDir);
    FishDir=FishDir*180/M_PI;//鱼的角度
    //FishDir=Checkangle1(FishDir);
    distfishtoaimpt=Distance(FishPt,aimpt);//球到鱼中心的距离
    distfishtoheadpt=Distance(FishPt,FishHeadPt);//球到鱼头的距离(有问题不可以使用）
    dirfishtoheadpt=Angle(aimpt,FishHeadPt);
    dirfishtoheadpt=Checkangle(dirfishtoheadpt)*180/M_PI;//球到鱼头的角度
    //dirfishtoheadpt=Checkangle1(dirfishtoheadpt);
    dirfishtoaimpt=Angle(aimpt,FishPt);
    dirfishtoaimpt=Checkangle(dirfishtoaimpt)*180/M_PI;//球到鱼中心的角度
    //dirfishtoaimpt=Checkangle1(dirfishtoaimpt);
    distballtodoor=Distance(aimpt,doorpt);//球到门的距离
    dirballtodoor=Angle(shotpt0,doorpt);
    dirballtodoor=Checkangle(dirballtodoor)*180/M_PI;//击球点到球门的角度
    //dirballtodoor=Checkangle1(dirballtodoor);
    diraimpttodoor=Angle(aimpt,doorpt);
    diraimpttodoor=Checkangle(diraimpttodoor)*180/M_PI;//球到球门的角度
    //diraimpttodoor=Checkangle1(diraimpttodoor);
    tempup0.y=aimpt.y-50;
    tempup0.x=aimpt.x;
    if(tempup0.x<0)tempup0.x=0;
    if(tempup0.x>740)tempup0.x=720;
    if(tempup0.y<0)tempup0.y=0;
    if(tempup0.y>480)tempup0.y=480;
    tempup1.y=aimpt.y-50;
    tempup1.x=aimpt.x+70;
    if(tempup1.x<0)tempup1.x=0;
    if(tempup1.x>740)tempup1.x=720;
    if(tempup1.y<0)tempup1.y=0;
    if(tempup1.y>480)tempup1.y=480;
    tempup2.y=aimpt.y-50;
    tempup2.x=aimpt.x-70;
    if(judgeball(aimpt)==3)
    {
        tempup2.y=aimpt.y-50;
        tempup2.x=aimpt.x-90;
    }
    if(tempup2.x<0)tempup2.x=0;
    if(tempup2.x>740)tempup2.x=720;
    if(tempup2.y<0)tempup2.y=0;
    if(tempup2.y>480)tempup2.y=480;
    tempdown0.y=aimpt.y+50;
    tempdown0.x=aimpt.x;
    if(tempdown0.x<0)tempdown0.x=0;
    if(tempdown0.x>740)tempdown0.x=720;
    if(tempdown0.y<0)tempdown0.y=0;
    if(tempdown0.y>480)tempdown0.y=480;
    tempdown1.y=aimpt.y-50;
    tempdown1.x=aimpt.x+70;
    if(tempdown1.x<0)tempdown1.x=0;
    if(tempdown1.x>740)tempdown1.x=720;
    if(tempdown1.y<0)tempdown1.y=0;
    if(tempdown1.y>480)tempdown1.y=480;
    tempdown2.y=aimpt.y+40;
    tempdown2.x=aimpt.x-70;
    if(judgeball(aimpt)==3)
    {
        tempdown2.y=aimpt.y+50;
        tempdown2.x=aimpt.x-90;
    }
    if(tempdown2.x<0)tempdown2.x=0;
    if(tempdown2.x>740)tempdown2.x=720;
    if(tempdown2.y<0)tempdown2.y=0;
    if(tempdown2.y>480)tempdown2.y=480;
    if(mode==0)
    {
        if((FishPt.x>aimpt.x+shopdistance&&flag1==0&&flag4==0)||FishPt.x<=80||FishPt.x>680)
        {
            flag3=0;
            flag4=0;
            std::cout<<"慢击球"<<std::endl;
            Roundp2p(shotpt0,action,0,m_FishInfo);
        }
        if(FishPt.x<aimpt.x+shopdistance&&FishPt.y>aimpt.y&&FishPt.x>=80&&FishPt.x<680)
        {
            flag3=1;
        }
        if(FishPt.x<aimpt.x+shopdistance&&FishPt.y<aimpt.y&&FishPt.x>=80&&FishPt.x<680)
        {
            flag4=1;

        }
        if(flag3==1)
        {
            flag4=0;
            if((FishDir<-90||FishDir>90)&&Distance(FishPt,aimpt)<120&&findpoint==0)
            {
                std::cout<<"左行"<<std::endl;
                action.direction=1;
                //Roundp2pleft(tempdown1,action,0,m_FishInfo);
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                Roundp2p(tempdown1,action,0,m_FishInfo);
                if(Distance(FishPt,tempdown1)<turndistance)
                {
                    findpoint=0;
                    flag3=0;
                }
            }
        }
        if(flag4==1)
        {
            flag3=0;
            if((FishDir<-90||FishDir>0)&&Distance(FishPt,aimpt)<120&&findpoint==0)
            {
                std::cout<<"右行"<<std::endl;
                action.direction=12;
                //Roundp2pright(tempup1,action,0,m_FishInfo);
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                Roundp2p(tempup1,action,0,m_FishInfo);
                if(Distance(FishPt,tempup1)<turndistance)
                {
                    findpoint=0;
                    flag4=0;
                }
            }
        }
    }
    if(mode==1)
    {
        if((FishPt.x<aimpt.x-shopdistance&&flag5==0&&flag6==0)||FishPt.x<=80||FishPt.x>680)
        {
            flag5=0;
            flag6=0;
            std::cout<<"击球"<<std::endl;
            Roundp2p(shotpt0,action,0,m_FishInfo);
        }
        if(FishPt.x>aimpt.x-shopdistance&&FishPt.y<aimpt.y&&FishPt.x>=80)
        {
            flag5=1;
            flag6=0;
        }
        if(FishPt.x>aimpt.x-shopdistance&&FishPt.y>aimpt.y&&FishPt.x>=80)
        {
            flag6=1;
            flag5=0;

        }
        if(flag5==1)
        {
            flag6=0;
            //std::cout<<"左行"<<FishDir<<std::endl;
            //Roundp2p(tempup2,action,0,m_FishInfo);
            if((FishDir<90||FishDir>160)&&Distance(FishPt,aimpt)<180&&findpoint==0)
            {
                std::cout<<"左行"<<FishDir<<std::endl;
                action.direction=1;
                //Roundp2pleft(tempup2,action,0,m_FishInfo);
                //std::cout<<tempup2.x<<tempup2.y<<std::endl;
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                Roundp2p(tempup2,action,0,m_FishInfo);
                if(Distance(FishPt,tempup2)<turndistance)
                {
                    findpoint=0;
                    flag5=0;
                }
            }
        }
        if(flag6==1)
        {
            flag5=0;
            //std::cout<<"右行"<<std::endl;
            //Roundp2p(tempdown2,action,0,m_FishInfo);
            if(FishDir<-160||FishDir>-90&&Distance(FishPt,aimpt)<120&&findpoint==0)
            {
                std::cout<<"右行"<<std::endl;
                action.direction=12;
                //std::cout<<tempdown2.x<<tempdown2.y<<std::endl;
                //Roundp2pright(tempdown2,action,0,m_FishInfo);
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                Roundp2p(tempdown2,action,0,m_FishInfo);
                if(Distance(FishPt,tempdown2)<turndistance)
                {
                    findpoint=0;
                    flag6=0;
                }
            }
        }
    }
}

void CStrategy::movingshot3(int mode,CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo,CPoint doorpt,CPoint shotpt0)//垂直向左，向右顶球
{
    CPoint FishPt;// 鱼中心的位置
    double FishDir,distfishtoaimpt,distfishtoheadpt,dirfishtoaimpt,dirfishtoheadpt,distballtodoor,dirballtodoor,diraimpttodoor;// 鱼头的方向
    CPoint FishHeadPt;//鱼头的位置
    CPoint FishTailPt;//鱼尾巴的位置
    CPoint tempup0,tempup1,tempup2,tempdown0,tempdown1,tempdown2;//临时击球点
    FishPt=m_FishInfo.GetCenterPoint();//鱼中心点坐标
    FishHeadPt = m_FishInfo.GetHeaderPoint();//鱼头坐标
    FishDir=Angle(FishPt,FishHeadPt);
    FishDir=Checkangle(FishDir);
    FishDir=FishDir*180/M_PI;//鱼的角度
    //FishDir=Checkangle1(FishDir);
    distfishtoaimpt=Distance(FishPt,aimpt);//球到鱼中心的距离
    distfishtoheadpt=Distance(FishPt,FishHeadPt);//球到鱼头的距离(有问题不可以使用）
    dirfishtoheadpt=Angle(aimpt,FishHeadPt);
    dirfishtoheadpt=Checkangle(dirfishtoheadpt)*180/M_PI;//球到鱼头的角度
    //dirfishtoheadpt=Checkangle1(dirfishtoheadpt);
    dirfishtoaimpt=Angle(aimpt,FishPt);
    dirfishtoaimpt=Checkangle(dirfishtoaimpt)*180/M_PI;//球到鱼中心的角度
    //dirfishtoaimpt=Checkangle1(dirfishtoaimpt);
    distballtodoor=Distance(aimpt,doorpt);//球到门的距离
    dirballtodoor=Angle(shotpt0,doorpt);
    dirballtodoor=Checkangle(dirballtodoor)*180/M_PI;//击球点到球门的角度
    //dirballtodoor=Checkangle1(dirballtodoor);
    diraimpttodoor=Angle(aimpt,doorpt);
    diraimpttodoor=Checkangle(diraimpttodoor)*180/M_PI;//球到球门的角度
    //diraimpttodoor=Checkangle1(diraimpttodoor);
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
        if(aimpt.y<middledown+10)
        {
            tempdown0.y=350;//向最右面顶球的姿态调整点
            tempdown0.x=twodoorright;
        }
        else
        {
            tempdown0.y=350;//向最右面顶球的姿态调整点
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
            Roundp2p(shotpt0,action,0,m_FishInfo);
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
            if(FishDir<180&&FishDir>0&&Distance(FishPt,aimpt)<180&&findpoint==0)
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
                Roundp2p(tempup0,action,0,m_FishInfo);
                if(Distance(FishPt,tempup0)<turndistance)
                {
                    findpoint=0;
                    flag11=0;
                }
            }
        }
        if(flag12==1)
        {
            flag11=0;
            if((FishDir<0||FishDir>90)&&Distance(FishPt,aimpt)<180&&findpoint==0)
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
                Roundp2p(tempup0,action,0,m_FishInfo);
                if(Distance(FishPt,tempup0)<turndistance)
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
            Roundp2p(shotpt0,action,0,m_FishInfo);
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
            if(FishDir>-180&&FishDir<0&&Distance(FishPt,aimpt)<180&&findpoint==0)
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
                Roundp2p(tempdown0,action,0,m_FishInfo);
                if(Distance(FishPt,tempdown0)<turndistance)
                {
                    findpoint=0;
                    flag9=0;
                }
            }
        }
        if(flag10==1)
        {
            flag9=0;
            //std::cout<<"右行"<<std::endl;
            if(FishDir>-90||FishDir<-150&&Distance(FishPt,aimpt)<180&&findpoint==0)
            {
                std::cout<<"右行"<<std::endl;
                action.direction=12;
                //Roundp2pright(tempdown0,action,0,m_FishInfo);
            }
            else
            {
                findpoint=1;
            }
            if(findpoint==1)
            {
                std::cout<<"找点"<<std::endl;
                Roundp2p(tempdown0,action,0,m_FishInfo);
                if(Distance(FishPt,tempdown0)<turndistance)
                {
                    findpoint=0;
                    flag10=0;
                }
            }
        }

    }
}

BOOL CStrategy::Strategynew(CFishAction m_action[],CFishInfo m_FishInfo[],CBallInfo  m_goalinfo[], OBSTAINFO m_obst[],CHANNEL m_Channel[])
{
    CPoint FishPt,FishPt1;// 鱼中心的位置
    double FishDir,distfishtoaimpt,distfishtoheadpt,dirfishtoaimpt,dirfishtoheadpt,distballtodoor,diraimpttodoor,diraimpttodoor1,diraimpttodoor3,dirspeed;// 鱼头的方向
    CPoint FishHeadPt;//鱼头的位置
    CPoint FishTailPt;//鱼尾巴的位置
    CPoint tempup,tempdown;//临时击球点
    CPoint aimpt;//球的坐标
    CPoint doorpt,doorpt1,doorpt3;//球门坐标
    CPoint shotpt0,shotpt1,shotpt3,shotptup,shotptdown,shotptright,shotptleft,finalshot;//击球点坐标
    doorpt=m_Channel[A].center;
    doorpt1=m_Channel[B].center;
    doorpt3=m_Channel[2].center;
    //std::cout<<doorpt1.x<<std::endl;
    aimpt=m_goalinfo[0].getCenter();
    std::cout<<aimpt.x<<" "<<aimpt.y<<std::endl;
    FishPt=m_FishInfo[A].GetCenterPoint();//鱼中心点坐标
    FishPt1=m_FishInfo[B].GetCenterPoint();//鱼中心点坐标
    FishHeadPt = m_FishInfo[A].GetHeaderPoint();//鱼头坐标

    FishDir=Angle(FishPt,FishHeadPt);
    FishDir=Checkangle(FishDir);
    FishDir=FishDir*180/M_PI;//鱼的角度
    distfishtoaimpt=Distance(FishPt,aimpt);//球到鱼中心的距离
    distfishtoheadpt=Distance(FishPt,FishHeadPt);//球到鱼头的距离(有问题）
    dirfishtoheadpt=Angle(aimpt,FishHeadPt);
    dirfishtoheadpt=Checkangle(dirfishtoheadpt)*180/M_PI;//球到鱼头的角度
    dirfishtoaimpt=Angle(aimpt,FishPt);
    dirfishtoaimpt=Checkangle(dirfishtoaimpt)*180/M_PI;//球到鱼中心的角度
    distballtodoor=Distance(aimpt,doorpt);//球到门的距离
    diraimpttodoor=Angle(aimpt,doorpt);
    diraimpttodoor=Checkangle(diraimpttodoor)*180/M_PI;//球到球门的角度
    diraimpttodoor1=Angle(aimpt,doorpt1);
    diraimpttodoor1=Checkangle(diraimpttodoor1)*180/M_PI;//球到球门的角度
    diraimpttodoor3=Angle(aimpt,doorpt3);
    diraimpttodoor3=Checkangle(diraimpttodoor3)*180/M_PI;//球到球门的角度
    dirspeed=Angle(FishPt,aimpt);
    dirspeed=Checkangle(dirspeed)*180/M_PI;
    dirspeed=fabs(Checkangle0(FishDir-dirspeed));
    std::cout<<"dirspeed"<<dirspeed<<std::endl;
    //实时获取参数值
    initdata(m_Channel);//到具体的场地以后将范围扩大化

    //击球点初始化
    shotpt0.x=aimpt.x-radus*cos(diraimpttodoor);
    shotpt0.y=aimpt.y-radus*sin(diraimpttodoor);

    shotpt1.x=aimpt.x-radus*cos(diraimpttodoor1);
    shotpt1.y=aimpt.y-radus*sin(diraimpttodoor1);

    shotpt3.x=aimpt.x-radus*cos(diraimpttodoor3);
    shotpt3.y=aimpt.y-radus*sin(diraimpttodoor3);
    shotptup.y=aimpt.y+radus;//垂直向上顶球点
    shotptup.x=aimpt.x;
    shotptdown.y=aimpt.y-radus;//垂直向下顶球点
    shotptdown.x=aimpt.x;
    shotptright.y=aimpt.y-5;//最右面的点
    shotptright.x=aimpt.x+radus;
    shotptleft.y=aimpt.y+5;//最左面的点
    shotptleft.x=aimpt.x-radus;
    finalshot.y=middle;
    finalshot.x=finaldoorx;
    m_action[B].speed=speedB;
    m_action[B].state=0;
    m_action[B].mode=0;
    int speed=0;
    if(dirspeed>60)speed=9;
    else
        speed=9;
    m_action[A].state=0;
    m_action[A].mode=0;
    m_action[A].speed=speed;
    //测试顶球射门的策略
    //movingshot(aimpt,m_action[A],A,m_FishInfo[A],doorpt,shotpt0);
    //movingshot2(0,shotptup,m_action[A],A,m_FishInfo[A],doorpt,getpoint(shotptright.x,shotptright.y+5));
    /*deletezero(20);
    movingshot4(0,aimpt,m_action[A],A,m_FishInfo[A],doorpt,getpoint(aimpt.x+radus,aimpt.y));
    */
    //最原始让鱼先右游一会，然后向右开mode;

    static int stop0=0,stop1=0,count0=0,count1=0,movement=0;;
    //A鱼顶左面球门策略
    std::cout<<judgeball(aimpt)<<std::endl;
    if(movement==0)
    {
        std::cout<<"1"<<std::endl;
        ptop(getpoint(doorpt.x,doorpt.y-100),m_action[B],A,m_FishInfo[B]);
        if(ptopflag==1)
        {
            movement++;
        }
    }
    if(movement==1)
    {
        std::cout<<"2"<<std::endl;
        ptop(getpoint(doorpt.x,doorpt.y+20),m_action[B],A,m_FishInfo[B]);
        if(FishPt1.y>highth/2+10)
        {
            movement++;
        }
    }
    if(movement==2)
    {
        std::cout<<"3"<<std::endl;
        ptop(getpoint(doorpt1.x-60,doorpt1.y+60),m_action[B],A,m_FishInfo[B]);
        if(ptopflag==1)
        {
            movement++;
        }
    }
    if(movement==3)
    {
        std::cout<<"4"<<std::endl;
        ptop(getpoint(doorpt1.x,doorpt1.y-20),m_action[B],A,m_FishInfo[B]);
        if(FishPt1.y<highth/2-10)
        {
            movement++;
        }
    }
    if(movement==4)
    {
        std::cout<<"5"<<std::endl;
        ptop(getpoint(width,0),m_action[B],A,m_FishInfo[B]);	//--门全开，在右上角等待
        if(judgeball(aimpt) == 2)			//--准备进攻二号门时，movement++
        {
            movement++;
        }
    }
    if(movement==5)			//--movement5在左上角等待
    {
        std::cout<<"B等待3"<<std::endl;
        Roundp2p(getpoint(0,0),m_action[B],A,m_FishInfo[B]);
        if(judgeball(aimpt) == 1)
        {
            movement--;
        }
    }
    if(movement==6)		//--暂时没用
    {
        std::cout<<"B等待3"<<std::endl;
        m_action[B].direction=14;
        if(Distance(FishPt1,aimpt)>80)
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
                            m_action[A].direction = 15;
                        }
                        else if (aimpt.y > middle&&aimpt.y<middledown&&aimpt.x>onedoorleft + 10 && aimpt.x < onedoorright)//球在门口的时候
                        {
                            std::cout << "球在门口的时候" << std::endl;
                            shopdistance = 23;
                            /*		if(FishPt.x<aimpt.x&&FishPt.y<aimpt.y+20)
                            		{
                            			deletezero(31);
                            			movingshot3(1,aimpt,m_action[A],A,m_FishInfo[A],doorpt,getpoint(aimpt.x,aimpt.y+10));
                            		}*/

                            //movingshot1(0,aimpt,m_action[A],A,m_FishInfo[A],doorpt,shotpt0);//对球最下面顶
                            if (FishPt.x < aimpt.x - 10)
                            {
                                deletezero(10);
                                movingshot1(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptup.x + 3, shotptup.y));//对球最下面顶
                            }
                            if (FishPt.x > aimpt.x + 10)
                            {
                                deletezero(10);
                                movingshot1(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptup.x - 3, shotptup.y));//对球最下面顶
                            }
                            if (FishPt.x <= aimpt.x + 10 && FishPt.x >= aimpt.x - 10)
                            {
                                deletezero(10);
                                movingshot1(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptup.x, shotptup.y));//对球最下面顶
                            }
                        }
                        else if (aimpt.y > 350 && aimpt.y < 450 && aimpt.x < onedoorleft)////在下左平面(靠下，在第一门左面）
                        {
                            deletezero(10);
                            std::cout << "aimpt.y-350-450" << std::endl;
                            //if(FishPt.x<aimpt.x&&FishPt.y<aimpt.y+30)
                            //{
                            //                       movingshot2(1,aimpt,m_action[A],A,m_FishInfo[A],doorpt,shotpt0);//对球最下面顶
                            //}
                            //if(FishPt.x>aimpt.x&&FishPt.y<aimpt.y+30)
                            //{
                            //                       movingshot2(0,aimpt,m_action[A],A,m_FishInfo[A],doorpt,shotpt0);//对球最下面顶
                            //}

                            movingshot1(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, shotpt0);//对球最下面顶

                        }
                        else if (aimpt.y < 480 && aimpt.y>450 && aimpt.x < onedoorright)////在下左平面(靠下，在第一门左面）
                        {
                            deletezero(20);
                            std::cout << "在最下面" << std::endl;
                            movingshot2(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, shotptup);//对球最下面顶
                        }
                        else if (aimpt.y > middledown&&aimpt.x > onedoorleft&&aimpt.x < onedoorright)////在下左平面(在第一门之间）
                        {
                            deletezero(10);
                            std::cout << "在第一门之间" << std::endl;
                            if (FishPt.x > aimpt.x&&FishPt.y < aimpt.y + 20)
                            {
                                movingshot2(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptup.x + 5, shotptup.y));//对球最下面顶
                            }
                            else
                            {
                                movingshot1(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, shotpt0);//对球最下面顶
                                //if(FishPt.x<aimpt.x-10)
                                //{
                                //	movingshot1(0,aimpt,m_action[A],A,m_FishInfo[A],doorpt,getpoint(shotptup.x+5,shotptup.y));//对球最下面顶
                                //}
                                //if(FishPt.x>aimpt.x+10)
                                //{
                                //	movingshot1(0,aimpt,m_action[A],A,m_FishInfo[A],doorpt,getpoint(shotptup.x,shotptup.y));//对球最下面顶
                                //}
                                //if(FishPt.x<=aimpt.x+10&&FishPt.x>=aimpt.x-10)
                                //{
                                //	movingshot1(0,aimpt,m_action[A],A,m_FishInfo[A],doorpt,getpoint(shotptup.x,shotptup.y));//对球最下面顶
                                //}
                            }
                        }
                        else if (aimpt.y > middle&&aimpt.y < middledown + 30 && aimpt.x < onedoorleft + 10)//在下左平面(靠上，在第一门左面)
                        {
                            deletezero(31);
                            if (aimpt.x < 1 * onedoorleft / 3)
                            {
                                std::cout << "向右对球下面顶" << std::endl;
                                movingshot3(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - 3, aimpt.y + 13));
                            }
                            else if (aimpt.x < 2 * onedoorleft / 3)
                            {
                                std::cout << "向右对球下面顶" << std::endl;
                                movingshot3(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - 10, aimpt.y + 8));
                            }
                            else
                            {
                                std::cout << "向右对球最下面顶" << std::endl;
                                movingshot3(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - 8, aimpt.y + 10));
                            }
                        }
                        else
                        {
                            deletezero(10);
                            std::cout << "对球最下面顶" << std::endl;
                            if (FishPt.x < aimpt.x - 10)
                            {
                                movingshot1(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptup.x + 5, shotptup.y));//对球最下面顶
                            }
                            if (FishPt.x > aimpt.x + 10)
                            {
                                movingshot1(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptup.x - 5, shotptup.y));//对球最下面顶
                            }
                            if (FishPt.x <= aimpt.x + 10 && FishPt.x >= aimpt.x - 10)
                            {
                                movingshot1(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptup.x, shotptup.y));//对球最下面顶
                            }
                            //movingshot5(0,aimpt,m_action[A],A,m_FishInfo[A],doorpt,shotptup);//对球最下面顶
                        }
                    }
                    else if (aimpt.y > middle&&aimpt.x >= onedoorright)//球在一球门右
                    {
                        //优先判断是否处于卡住状态
                        if ((aimpt.y > middle + 50 && aimpt.y<middledown&&aimpt.x>twodoorleft&&aimpt.x < twodoorright) && (FishDir > -60 && FishDir < 60))//鱼在第二个球门左面被卡住区域
                        {
                            std::cout << "二门左卡住" << std::endl;
                            m_action[A].direction = 15;
                        }
                        else if ((aimpt.y > middle&&aimpt.x < twodoorright) && (FishPt.y > middle + 50 && FishPt.y < middledown&&FishPt.x<twodoorright + 100 && FishPt.x>twodoorright) && (FishDir > 130 || FishDir < -130))//球在二球门左,鱼在右
                        {
                            std::cout << "二门右卡住" << std::endl;
                            m_action[A].direction = 15;
                        }
                        else if (aimpt.y > middle&&aimpt.y < middledown + 20 && aimpt.x<width / 2 && aimpt.x>onedoorright)//卡球区域
                        {
                            deletezero(20);
                            std::cout << "对着最上面击球" << std::endl;
                            movingshot2(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, shotptdown);//对球最右面顶
                        }
                        else if (aimpt.y > middledown + 20 && aimpt.x<width / 2 && aimpt.x>onedoorright)//在第一球门右面击球区域
                        {
                            deletezero(20);
                            std::cout << "在第一球门右面击球区域" << std::endl;
                            if (aimpt.y > 350)
                            {
                                movingshot2(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x + radus, aimpt.y + 5));//对球最右面顶
                            }
                            else
                            {
                                movingshot2(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x + radus, aimpt.y + 8));//对球最右面顶
                            }
                        }
                        else if (aimpt.y > middle&&aimpt.x >= onedoorright)
                        {
                            deletezero(20);
                            std::cout << "正常区域" << std::endl;
                            movingshot2(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x + radus, aimpt.y));//对球最右面顶
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
                    m_action[A].direction = 7;
                }
                else
                {
                    Roundp2p(doorpt, m_action[A], A, m_FishInfo[A]);
                }
            }
            if (judgeareaup(FishPt, 5))//鱼在上平面
            {
                std::cout << "right fish 1->2" << std::endl;
                Roundp2p(doorpt1, m_action[A], A, m_FishInfo[A]);
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
                    m_action[A].direction=7;
                    }*/
                    if (aimpt.y < middle&&aimpt.x < onedoorright + 10)//球在一球门以左
                    {
                        if (aimpt.y<middle&&aimpt.y>middleup - 30)//在一门区域向左靠下
                        {
                            deletezero(21);
                            std::cout << "在一门区域向左靠下" << std::endl;
                            movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x - 10, aimpt.y + 3));
                        }
                        else if (aimpt.y < middleup - 30 && aimpt.y>100)
                        {
                            deletezero(21);
                            std::cout << "一球门以左中间" << std::endl;
                            if (FishPt.y < aimpt.y)
                            {
                                movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x - 10, aimpt.y - 5));
                            }
                            else
                            {
                                movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x - 10, aimpt.y + 5));
                            }
                        }
                        else
                        {
                            deletezero(21);
                            std::cout << "在一门区域向左靠上" << std::endl;
                            movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x - 10, aimpt.y));
                        }
                    }
                    else if (aimpt.y<middle&&aimpt.x>onedoorright + 10 && aimpt.x < twodoorleft)//一球门以右，二球门以左
                    {
                        if (aimpt.y<middle&&aimpt.x>onedoorright&&aimpt.x < twodoorleft&&aimpt.y < 80)
                        {
                            deletezero(21);
                            std::cout << "一门二门靠上" << std::endl;
                            movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x - 10, aimpt.y - 5));
                        }
                        else if (aimpt.y<middle&&aimpt.x>onedoorright&&aimpt.x < twodoorleft - 50 && aimpt.y < 160 && aimpt.y >= 80)
                        {
                            std::cout << "一门二门中间" << std::endl;
                            if (FishPt.x<aimpt.x&&FishPt.y>aimpt.y - 60)
                            {
                                deletezero(21);
                                movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x, aimpt.y - 10));
                            }
                            else
                            {
                                if (FishPt.x > aimpt.x)
                                {
                                    deletezero(11);
                                    movingshot1(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x - 8, aimpt.y - 13));
                                }
                                else
                                {
                                    deletezero(11);
                                    movingshot1(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x - 8, aimpt.y - 8));
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
                                    movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x - 8, aimpt.y - 5));
                                }
                                else
                                {
                                    movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x - 8, aimpt.y - 15));
                                }
                            }
                            else
                            {
                                if (aimpt.x < width / 2)
                                {
                                    movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x - 8, aimpt.y - 12));
                                }
                                else
                                {
                                    movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x, aimpt.y - 12));
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
                                movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - 5, aimpt.y - 16));
                            }
                            if (FishPt.x > aimpt.x&&FishPt.y > aimpt.y - 30)
                            {
                                deletezero(20);
                                movingshot2(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x + 5, aimpt.y - 16));
                            }
                            else
                            {
                                deletezero(11);
                                shopdistance = 23;
                                if (FishPt.x < aimpt.x - 10)
                                {
                                    movingshot1(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptdown.x + 5, shotptdown.y));//对球最下面顶
                                }
                                if (FishPt.x > aimpt.x + 10)
                                {
                                    movingshot1(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptdown.x - 5, shotptdown.y));//对球最下面顶
                                }
                                if (FishPt.x <= aimpt.x + 10 && FishPt.x >= aimpt.x - 10)
                                {
                                    movingshot1(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptdown.x, shotptdown.y));//对球最下面顶
                                }
                            }
                        }
                        else if (aimpt.y < 60)
                        {
                            deletezero(21);
                            movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - radus, aimpt.y - 10));
                            /*if(FishPt.x<aimpt.x)
                            {
                            std::cout<<"左击"<<std::endl;
                            deletezero(21);
                            movingshot5(1,aimpt,m_action[A],A,m_FishInfo[A],doorpt,getpoint(shotptdown.x,shotptdown.y));
                            }
                            else
                            {
                            std::cout<<"右击"<<std::endl;
                            deletezero(20);
                            movingshot5(0,aimpt,m_action[A],A,m_FishInfo[A],doorpt,getpoint(shotptdown.x,shotptdown.y));
                            }*/
                        }
                        else
                        {
                            if (FishPt.x < aimpt.x - 10)
                            {
                                movingshot1(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptdown.x + 5, shotptdown.y));//对球最下面顶
                            }
                            if (FishPt.x > aimpt.x + 10)
                            {
                                movingshot1(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(shotptdown.x - 5, shotptdown.y));//对球最下面顶
                            }
                            if (FishPt.x <= aimpt.x + 10 && FishPt.x >= aimpt.x - 10)
                            {
                                movingshot1(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, shotpt1);//对球最下面顶
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
                                movingshot3(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x + 10, aimpt.y - 6));//对球最右面顶
                            }
                            else
                            {
                                std::cout << "二球门右靠下偏左" << middleup << std::endl;
                                movingshot3(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x + 10, aimpt.y - 15));//对球最右面顶
                            }
                        }
                        else
                        {
                            std::cout << "二球门右靠上" << middleup << std::endl;
                            if (aimpt.y < 80)
                            {
                                std::cout << "向右对最下面顶" << std::endl;
                                deletezero(21);
                                movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x - radus, aimpt.y - 8));//对球最右面顶
                            }
                            else
                            {
                                std::cout << "往下往最上面偏右顶" << std::endl;
                                deletezero(11);
                                movingshot1(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, getpoint(aimpt.x + 3, aimpt.y - 10));//对球最右面顶
                            }
                        }
                    }
                    else
                    {
                        movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt1, shotpt1);//对球最右面顶
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
                    m_action[A].speed = speed3;
                    m_action[A].direction = 15;
                }
                else if (FishPt.x > twodoorright + 30 && FishPt.y < doorpt1.y + 30 && stop0 == 0 && stop1 == 0)
                {

                    std::cout << "xia" << std::endl;
                    Roundp2p(getpoint(doorpt1.x, doorpt1.y + 400), m_action[A], A, m_FishInfo[A]);
                }
                else
                {
                    if (aimpt.x > doorpt.x)
                    {
                        std::cout << "偏右" << std::endl;
                        Roundp2p(getpoint(doorpt.x - 5, doorpt.y), m_action[A], A, m_FishInfo[A]);
                    }
                    else
                    {
                        std::cout << "偏左" << std::endl;
                        Roundp2p(getpoint(doorpt.x - 15, doorpt.y), m_action[A], A, m_FishInfo[A]);
                    }
                    //Roundp2p(doorpt,m_action[A],A,m_FishInfo[A]);
                }

            }
            else if (stop0 == 0 && stop1 == 0 && judgearea(FishPt, 8))//鱼在下平面
            {
                std::cout << "right fish 2->1" << std::endl;
                if (FishPt.x > twodoorleft && (FishDir > 0 || FishDir < -120))
                {
                    std::cout << "右转" << std::endl;
                    m_action[A].speed = speed3;
                    m_action[A].direction = 15;
                }
                else
                {
                    //std::cout<<"right fish 2->1"<<std::endl;
                    Roundp2p(doorpt1, m_action[A], A, m_FishInfo[A]);
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
                            m_action[A].direction = 15;
                        }
                        else if (judgearea(aimpt, 18) && judgearea(FishPt, 14) && (FishDir > 130 || FishDir < -130))//球在一球门左,鱼在右
                        {
                            std::cout << "一门右卡住" << std::endl;
                            m_action[A].direction = 15;
                            m_action[A].speed = speed3;
                        }
                        else
                        {
                            deletezero(21);
                            std::cout << "向右对球的上面顶" << std::endl;
                            movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - radus, aimpt.y - 3));//对球最下面顶
                        }

                    }
                    else if (judgearea(aimpt, 19))//球在一球门右
                    {
                        //优先判断是否处于卡住状态
                        if (judgearea(FishPt, 12) && (FishDir > -60 && FishDir < 60))//鱼在第二个球门左面被卡住区域
                        {
                            std::cout << "二门左卡住" << std::endl;
                            m_action[A].direction = 15;
                        }
                        else if (judgearea(aimpt, 16) && judgearea(FishPt, 15) && (FishDir > 130 || FishDir < -130))//球在二球门左,鱼在右
                        {
                            std::cout << "二门右卡住" << std::endl;
                            m_action[A].direction = 15;
                        }
                        else if (aimpt.y > middle&&aimpt.y < middledown + 10 && aimpt.x<twodoorright&&aimpt.x>onedoorright)
                        {
                            deletezero(21);
                            std::cout << "卡球区域" << std::endl;
                            /*					if(FishPt.x<onedoorright)
                            					{
                            						Roundp2p(getpoint(onedoorright,350),m_action[A],A,m_FishInfo[A]);
                            					}*/
                            movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - 10, aimpt.y - 13));//对球最右面顶
                        }
                        else if (aimpt.y > 430)
                        {
                            deletezero(21);
                            std::cout << "球到下面" << std::endl;
                            movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - 10, aimpt.y + 10));
                        }

                        else if (aimpt.x < twodoorright&&aimpt.y<440 && aimpt.y>middledown + 10)
                        {
                            deletezero(21);
                            std::cout << "击球区域" << std::endl;
                            if (FishPt.y < aimpt.x)
                            {
                                movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - 13, aimpt.y));//对球最右面顶
                            }
                            else
                            {
                                movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, shotpt3);//对球最右面顶
                            }
                        }
                        else if (aimpt.y > doorpt3.y+(m_Channel[2].width)/2 - 7 && aimpt.x > twodoorright + 30)
                        {
                            deletezero(10);
                            //std::cout<<"从下往上击球"<<std::endl;
                            //////---------------------//////
                            if (aimpt.x > doorpt3.x-30)
                            {
                                std::cout << "从上往下击球0000" << std::endl;
                                movingshot1(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x + 5, aimpt.y - radus));//对球右上顶
                            }
                            else
                            {
                                std::cout << "从下往上击球" << std::endl;
                                //movingshot1(0,aimpt,m_action[A],A,m_FishInfo[A],doorpt,shotpt3);//对球最右面顶
                                if (FishPt.x > aimpt.x)
                                {
                                    movingshot1(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - 13, aimpt.y + 8));//对球最右面顶
                                }
                                else
                                {
                                    movingshot1(0, aimpt, m_action[A], A, m_FishInfo[A], doorpt, shotpt3);//对球最右面顶
                                }
                            }
                        }
                        else if (aimpt.y<doorpt3.y + (m_Channel[2].width) / 2 - 7 && aimpt.x>twodoorright)
                        {
                            deletezero(21);
                            std::cout << "向右击球" << std::endl;
                            if (FishPt.y < aimpt.y)
                            {
                                movingshot3(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - 15, aimpt.y + 12));//对球最右面顶
                            }
                            else
                            {
                                movingshot3(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, getpoint(aimpt.x - 15, aimpt.y + 8));//对球最右面顶
                            }
                        }
                        else if (aimpt.y > doorpt3.y + (m_Channel[2].width) / 2 - 7 && aimpt.x < twodoorright + 30)
                        {
                            deletezero(21);
                            std::cout << "向右击球" << std::endl;
                            movingshot2(1, aimpt, m_action[A], A, m_FishInfo[A], doorpt, shotpt3);//对球最右面顶
                        }
                    }
                }
            }
            if (judgeareaup(FishPt, 4))//鱼在上平面
            {
                std::cout << "left fish 1->2" << middle << std::endl;
                Roundp2p(doorpt, m_action[A], A, m_FishInfo[A]);
            }
            if (judgeareaup(FishPt, 5))//鱼在上平面
            {
                std::cout << "right fish 1->2" << std::endl;
                Roundp2p(doorpt1, m_action[A], A, m_FishInfo[A]);
            }
        }
    }
    else		//--A鱼先静止
    {
        std::cout << "A鱼静止" << std::endl;
        m_action[A].state = 0;
        m_action[A].mode = 0;
        m_action[A].speed = 0;
    }
    std::cout << "speed" << speed << std::endl << std::endl;
    return TRUE;
}

void CStrategy::Roundp2p(CPoint aimpt,CFishAction &action,int fishID,CFishInfo m_FishInfo)
{

    CPoint FishPt;// 鱼中心的位置
    double FishDir;// 鱼头的方向
    CPoint FishHeadPt;//鱼头的位置
    CPoint LastFishPt;
    CPoint FishRotatePt;//鱼的转动位置
    CPoint FishTailPt;//鱼尾巴的位置
    CPoint GoalPt1, GoalPt2;//临时目标点，GoalPt2没有用到
    CPoint Pt1, Pt2, Pt3, Pt4;
    //CPoint f_headpt;//point of fish's head
    //f_headpt=m_FishInfo[0].GetHeadPoint();

    double dir1 = 0;
    double dir0;
    double dist0;
    double dist1 = 0;
    double dir2 = 0;
    double dist2 = 0;//临时用到的方向和距离
    CPoint centerpt1, centerpt2;//作为鱼要绕的中心来用
    CPoint centerpt3, centerpt4, centerpt5, centerpt6;
    CPoint centerpt0, centerpt00;
    double dis1, dis2;
    dis1 = 0;
    dis2 = 0;
    double radius;///需要游动的半径,可以随意的设置.
    double fishdir = FishDir;
    FishPt = m_FishInfo.GetCenterPoint();
    FishDir = m_FishInfo.GetDirection();
    dist1 = Distance(FishPt, aimpt);
    dir1 = Angle(FishPt, aimpt);
    dir1 -= FishDir;
    dir1 = Checkangle(dir1) * 180 / M_PI;


    m_FishInfo.SetAction(action);
    if (dist1 > 40)
    {
        if (dir1 > -5 && dir1 < 5)
            action.direction = 7;
        else if (dir1<-5 && dir1>-10)
            action.direction = 5;
        else if (dir1<-10 && dir1>-30)
            action.direction = 4;
        else if (dir1<-30 && dir1>-50)
            action.direction = 2;
        else if (dir1<-50 && dir1>-70)
            action.direction = 1;
        else if (dir1<-70 && dir1>-90)
            action.direction = 0;
        else if (dir1 < -90)
            action.direction = 0;
        else if (dir1 > 5 && dir1 < 10)
            action.direction = 10;
        else if (dir1 > 10 && dir1 < 20)
            action.direction = 11;
        else if (dir1 > 20 && dir1 < 40)
            action.direction = 13;
        else if (dir1 > 40 && dir1 < 50)
            action.direction = 13;
        else if (dir1 > 50 && dir1 < 80)
            action.direction = 14;
        else if (dir1 > 80 && dir1 < 90)
            action.direction = 14;
        else
            action.direction = 14;
    }
    else
    {
        if (dir1 > -7 && dir1 < 7)
            action.direction = 7;
        else if (dir1<-7 && dir1>-20)
            action.direction = 4;
        else if (dir1<-20 && dir1>-40)
            action.direction = 1;
        else if (dir1<-40 && dir1>-60)
            action.direction = 0;
        else if (dir1<-60 && dir1>-70)
            action.direction = 0;
        else if (dir1<-70 && dir1>-90)
            action.direction = 0;
        else if (dir1 < -90)
            action.direction = 0;
        else if (dir1 > 7 && dir1 < 20)
            action.direction = 10;
        else if (dir1 > 20 && dir1 < 30)
            action.direction = 12;
        else if (dir1 > 30 && dir1 < 50)
            action.direction = 13;
        else if (dir1 > 50 && dir1 < 70)
            action.direction = 14;
        else if (dir1 > 70 && dir1 < 90)
            action.direction = 14;
        else
            action.direction = 14;
    }
    m_FishInfo.SetAction(action);
}