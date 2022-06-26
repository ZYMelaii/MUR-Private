/**
 * @note 源码已经稍做调整以适配项目
 * @author zymelaii
 * @date 2022-06-26
 */

#include "../official/fishinfo.h"
#include "../official/externrobot.h"
#include "../mathtools.h"

////////////////////////////////////////////////////////////////

CFishInfo::CFishInfo()
{
    m_fTargetPos = m_fCenterP = m_fLastCenterP = m_fInterP = m_fHeaderP = m_fRotateP = m_fLastRotateP = CPoint(-1,-1);
    m_fdirection = m_flastdirection = m_fvdir=-10.0;
    m_ftime = 0;
    m_fv = m_fav = 0.0;
    m_fTargetPosDir = 0.0;
}

 /**判断action是否跟目前鱼执行的动作一样*/
BOOL CFishInfo::IsSameAction(CFishAction action)
{
    return m_fCurrentAction == action;
}

/**将上一次动作赋值给action*/
BOOL CFishInfo::GetLastAction(CFishAction& action)
{
    if(m_fLastAction.speed<0)
        return FALSE;

    action=m_fLastAction;
    return TRUE;
}

/**返回当前的控制速度档*/
int CFishInfo::GetCurrentSpeed()
{
    return m_fCurrentAction.speed; 
}

/**返回当前的控制方向档*/
int CFishInfo::GetCurrentDirection()
{
    return m_fCurrentAction.direction; 
}

/**返回当前的控制模式*/
int CFishInfo::GetCurrentMode()
{
    return m_fCurrentAction.mode;
}

/**返回当前的控制状态*/
int CFishInfo::GetCurrentState()
{
    return m_fCurrentAction.state;
}

/**返回当前的调试状态位Display*/
int CFishInfo::GetCurrentDisplay()
{
    return m_fCurrentAction.display;
}

/**返回鱼的ID*/
int CFishInfo::GetFishID()
{
    return m_fID;
}

/**设置鱼的ID*/
int CFishInfo::SetFishID(int id)
{
    m_fID = id;
    return 0;
}

/**返回鱼所属的Team ID*/
int CFishInfo::GetFishTeamID()
{
    return m_fTeamID;
}

/**设置鱼所属的Team ID*/
int CFishInfo::SetFishTeamID(int id)
{
    m_fTeamID = id;
    return 0;
}

/**返回鱼所属的Team名*/
char* CFishInfo::GetFishTeamName()
{
    return m_fTeamName;
}

/**设置鱼所属的Team名*/
int CFishInfo::SetFishTeamName(const char* fteamName)
{
    strcpy(m_fTeamName, fteamName);
    return 0;
}



/**获得当前的动作状态*/
void CFishInfo::GetCurrentAction(CFishAction& action)
{
    action = m_fCurrentAction; 
}

/**记录机器鱼的动作*/
void CFishInfo::SetAction(CFishAction& action)
{
    m_fLastAction=m_fCurrentAction;
    m_fCurrentAction = action;
}

/**返回鱼头尖端坐标*/
CPoint CFishInfo::GetHeaderPoint()
{
    return m_fHeaderP;
}

/**返回距离鱼r，和鱼方向夹角为dir度（顺时针为正，逆为负,注意单位不是弧度）的点*/
CPoint CFishInfo::GetHeaderPoint(int r,double dir)
{
    int x,y;
    m_fdirection+=(dir*M_PI/180);
    if (m_fdirection>M_PI)
        m_fdirection-=2*M_PI;
    else if (m_fdirection<-M_PI)
        m_fdirection+=2*M_PI;
    double temp=r*cos(m_fdirection);
    temp=temp>0?temp+0.5:temp-0.5;
    x=(int)temp+m_fCenterP.x;
    temp=r*sin(m_fdirection);
    temp=temp>0?temp+0.5:temp-0.5;
    y=(int)temp+m_fCenterP.y;

    return CPoint(x,y);
}

/**计算鱼头尖端坐标*/
void CFishInfo::CalHeaderPoint()
{
    CPoint pt;
    double h=REGBMP_WIDTH/POOL_WIDTH*HEADTOCENTER_DIS;        /**待修改，目前是宏定义，缺少灵活性*/
    pt.x=m_fCenterP.x+h*cos(m_fdirection)+0.5;
    pt.y=m_fCenterP.y+h*sin(m_fdirection)+0.5;
    m_fHeaderP=pt;
}

/**返回第一关节转动轴坐标*/
CPoint CFishInfo::GetRotatePoint()
{
    return m_fRotateP;
}

/**计算第一关节转动轴坐标*/
void   CFishInfo::CalRotatePoint()
{
    CPoint pt;
    double h=REGBMP_WIDTH/POOL_WIDTH*HEADTOCENTER_DIS;        /**待修改，目前是宏定义，缺少灵活性*/
    pt.x=m_fCenterP.x-h*cos(m_fdirection)+40;//0.5;           /**数值40 和 0.5待修改为宏定义,具体表示的含义待定*/
    pt.y=m_fCenterP.y-h*sin(m_fdirection)+40;//0.5;           /**数值40 和 0.5待修改为宏定义,具体表示的含义待定*/
    m_fLastRotateP = m_fRotateP;
    m_fRotateP = pt;
}

/**返回速度，单位cm/s */
double CFishInfo::GetV(void)
{
    return m_fv;
}

/**返回角速度，单位弧度/s */
double CFishInfo::GetAV(void)
{
    return m_fav;
}

/**返回速度方向，单位弧度*/
double CFishInfo::GetVdirection(void)
{
    return m_fvdir;
}

/**返回上一次鱼运动方向，单位弧度*/
double CFishInfo::GetLastDiredtion(void)
{
    return m_flastdirection;
}

/**计算速度、角速度、速度方向*/
void   CFishInfo::CalSpeedPara(void)
{
    
    if (m_fLastRotateP.x==-1)
    {
        m_fv=0.0;
        m_fav=0.0;
        return;
    }
    double time=1.0*TIMESTEP;       //取的时间的采样点   /**原始为double time=0.04;  2014.01*/
    CPoint cpt;

    cpt=m_fRotateP-m_fLastRotateP;
    m_fvdir=atan2((double)cpt.y,(double)cpt.x);
    double x,y;
    x=cpt.x*POOL_WIDTH/REGBMP_WIDTH;
    y=cpt.y*POOL_HEIGHT/REGBMP_HEIGHT;
    m_fv=sqrt(x*x+y*y)/time;
    //------以上是计算第一个舵机的速度的方法。
    m_fav=m_fdirection-m_flastdirection;

    if (fabs(m_fav)>M_PI)
    {
        if (m_fav>0)  m_fav=fabs(m_fav)-M_PI;
        else  m_fav=-(fabs(m_fav)-M_PI);
    }
    m_fav=m_fav/time;
    //----以上是计算角速度的方法。
}

/**根据最新的图像识别信息，更新设置鱼的信息，在图像识别算法中用*/
void   CFishInfo::SetInfo(CPoint pt,double dir,int time)
{
    m_fLastCenterP=m_fCenterP;  
    m_fCenterP=pt;

    m_flastdirection=m_fdirection;
    m_fdirection=dir;

    this->m_ftime=time;
    CalHeaderPoint();
    CalRotatePoint();
    CalSpeedPara();
}

/**返回中心点坐标*/
CPoint CFishInfo::GetCenterPoint()
{
    return m_fCenterP;
}

/**返回上个周期鱼中心点的位置*/
CPoint CFishInfo::GetLastCenterPoint()
{
    return m_fLastCenterP;
}

/**返回鱼头方向*/
double CFishInfo::GetDirection( )
{
    return m_fdirection;
}

/**设置目标点坐标及到目标点后期望的方向，dir弧度*/  
void   CFishInfo::SetToPos(CPoint pt,double dir)
{
    m_fTargetPos = pt;
    m_fTargetPosDir = dir;
}

 /**返回目标点坐标*/
CPoint CFishInfo::GetToPoint()
{
    return m_fTargetPos;
}

/**返回鱼鱼头尖端到设定目标点m_toPos的距离，单位为坐标单位，如果目标点没设定则返回-1*/
double CFishInfo::GetHeadToPosDistance()
{
    if(m_fTargetPos == CPoint(-1,-1))
        return -1;
    CPoint pt=m_fHeaderP-m_fTargetPos;
    return sqrt((double)pt.x*pt.x+pt.y*pt.y);
}
/**返回鱼中心点到设定目标点m_toPos的距离，单位为坐标单位，如果目标点没设定则返回-1*/
double CFishInfo::GetToPosDistance()
{
    if(m_fTargetPos == CPoint(-1,-1))
        return -1;
    CPoint pt=m_fTargetPos-m_fCenterP;
    return sqrt((double)pt.x*pt.x+pt.y*pt.y);
}

/**获得鱼中心点到设定目标点m_toPos的方向（-M_PI—M_PI），未设定目的点时返回-10,over为真则考虑过冲，现在一般不用考虑了*/
double CFishInfo::GetToPosDirection(BOOL over)
{
    double distance=GetToPosDistance();
    if(distance==-1 || distance<40 )
        return -10;

    CPoint pt=m_fTargetPos-m_fCenterP;
    double result=atan2((double)pt.y,(double)pt.x);

    result=m_fdirection-result;

    if(result>M_PI)
        result-=2*M_PI;
    else if(result<-M_PI)
        result+=2*M_PI;

    if (over) 
        return result+(m_fLastAction.direction==4?0.0:m_fLastAction.direction>4?M_PI/18:-M_PI/18);//待定，为什么以第4档来分情况
    else   
        return result;

    return 0;
}

/**返回鱼中心点到pos点的距离，单位为坐标单位，如果dir为真，则返回结果是距离+角度*/
double CFishInfo::GetToPosDistance(CPoint pos,BOOL dir)
{
    CPoint pt=pos-m_fCenterP;
    double result=atan2((double)pt.y,(double)pt.x);
    result=m_fdirection-result;
    if(result>M_PI)
        result-=2*M_PI;
    else if(result<-M_PI)
        result+=2*M_PI;

    return sqrt((double)pt.x*pt.x+pt.y*pt.y)+(dir?1.0:0.0)*fabs(result)/M_PI*180;
}

/**通过路径规划，确定PTP的临时插值目标点，并得到目前到达临时目标点对应的action，待定*/
BOOL   CFishInfo::GetPTPAction(CFishAction& action)
{
    CPoint midpt;
    midpt.x=(m_fTargetPos.x+m_fHeaderP.x)/2;
    midpt.y=(m_fTargetPos.y+m_fHeaderP.y)/2;


    double k1=m_fTargetPosDir+M_PI/2.0;
    if(k1>M_PI)
        k1-=2*M_PI;
    else if(k1<-M_PI)
        k1+=2*M_PI;
    k1=tan(k1);


    CPoint pt=m_fTargetPos-m_fHeaderP;
    double k3=atan2((double)pt.y,(double)pt.x);
    k3=k3+M_PI/2.0;
    if(k3>M_PI)
        k3-=2*M_PI;
    else if(k3<-M_PI)
        k3+=2*M_PI;
    k3=tan(k3);


    CPoint center;
    double x,y;
    x=(midpt.y-m_fTargetPos.y+k1*m_fTargetPos.x-k3*midpt.x)/(k1-k3);
    y=m_fTargetPos.y+k1*(x-m_fTargetPos.x);
    center.x=x+0.5;
    center.y=y+0.5;

    double k2=atan2(m_fHeaderP.y-y,m_fHeaderP.x-x);
    double k4=atan2(m_fTargetPos.y-y,m_fTargetPos.x-x);

    pt=m_fHeaderP-m_fTargetPos;
    double dis=sqrt(double(pt.x*pt.x+pt.y*pt.y));
    BYTE n=4;
    k3=atan2((double)pt.y,(double)pt.x);
    k3=m_fTargetPosDir-k3;
    if(k3>M_PI)
        k3-=2*M_PI;
    else if(k3<-M_PI)
        k3+=2*M_PI;
    double k5;
    if (fabs(k3)>M_PI/2)
    {
        if (k4-k2>M_PI)
        {
            k5=k2-(2*M_PI-(k4-k2))/n;
            if (k5<-M_PI)
                k5=k5+2*M_PI;
        }
        else if (k4-k2<-M_PI)
        {
            k5=k2+(2*M_PI+k4-k2)/n;
            if (k5>M_PI)
                k5=k5-2*M_PI;
        }
        else
            k5=k2+(k4-k2)/n;

    }
    else
    {
        if (k4-k2>M_PI||k4-k2<-M_PI)
        {
            k5=k2+(k4-k2)/n;
        }
        else if (k4-k2<0)
        {
            k5=k2+(2*M_PI+k4-k2)/n;
            if (k5>M_PI)
                k5=k5-2*M_PI;
        }
        else 
        {
            k5=k2-(2*M_PI-k4+k2)/n;
            if (k5<-M_PI)
                k5=k5+2*M_PI;
        }

    }
    pt=m_fHeaderP-center;
    double r=sqrt(double(pt.x*pt.x+pt.y*pt.y));

    m_fInterP.x=center.x+r*cos(k5)+0.5;
    m_fInterP.y=center.y+r*sin(k5)+0.5;

    if (m_fInterP.x<0) m_fInterP.x=0;
    else if (m_fInterP.x>REGBMP_WIDTH) m_fInterP.x=REGBMP_WIDTH;

    if (m_fInterP.y<0) m_fInterP.x=0;
    else if (m_fInterP.y>REGBMP_HEIGHT) m_fInterP.y=REGBMP_HEIGHT;


    pt=m_fHeaderP-m_fTargetPos;
    dis=sqrt(double(pt.x*pt.x+pt.y*pt.y));
    CPoint pt1=m_fInterP-m_fHeaderP;
    double dis1=sqrt(double(pt1.x*pt1.x+pt1.y*pt1.y));
    if (m_fCurrentAction.speed)
    {
        double result=atan2((double)pt1.y,(double)pt1.x);
        result=result-m_fdirection;
        if(result>M_PI)
            result-=2*M_PI;
        else if(result<-M_PI)
            result+=2*M_PI;

        result=result*180/M_PI;

        int nd;
        if (dis>200) nd=result/20;
        else if (dis>100) nd=result/15;
        else nd=result/10;



        if (nd>7) nd=7;
        else if (nd<-7) nd=-7;

        action.direction=7+nd;

        action.speed=m_fCurrentAction.speed;
        if (nd==0) 
        {   
            if (fabs(result)>15) action.speed=15;
            else
                action.speed=15-(int)(fabs(result));

        }
        return (GetHeadToPosDistance()<5);
    }
    else
    {       
        int ns=dis/15+1;
        if (ns>15) ns=15;
        action.speed=ns;
        return (GetHeadToPosDistance()<5);
    }
    return TRUE;
}

/**返回由GetPTPAction确定的插值目标点坐标，待定*/
CPoint CFishInfo::GetInterPoint()
{
    return m_fInterP;
}

////////////////////////////////////////////////////////////////