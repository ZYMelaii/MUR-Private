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
 * @author:        ZhangX，XiongML
 * @date:          14-04-04 16:55
 */

#ifndef MUR_STRUCT_H
#define MUR_STRUCT_H

#include "../common.h"

/**
 * @brief: 机器鱼动作执行所需信息
 */
typedef struct tagFishAction
{
    int id;                 /// 机器鱼id号
    int speed;	            /// 速度档次（0—15），0为静止
    int direction;	        /// 转弯方向及档次，0—6左转，7为中间值，8—14右转
    int state;              /// 状态，是否发送命令，0发送其他数不发送
    int display;            /// 显示在信息显示栏，状态处
					        /// 当前使用含义：
                            /// display=其他值;正在执行Point2PointClose函数，表其中的各个区域。
                            /// display=20;正在执行Point2PointNear函数。
                            /// display=30;正在执行Point2PointFar函数。
    BYTE mode;              /// 模态，0为通用转弯方式；1为原始转弯方式（转一下立刻回来）
} FISHACTION;

/**
 * @brief: 水球状态信息
 */
typedef struct tagBallInfo
{
    CPoint centerPt;	     /// 当前水球重心
    CPoint last_centerPt;	 /// 前一状态水球重心
    double v_ball;           /// 水球速度，单位cm/s 
    double v_direction;      /// 水球方向，单位弧度-PI~PI 
    float  radius;           /// 水球半径
    BYTE   time;	         /// 水球识别时间间隔
} BALLINFO;

/**
 * @brief: 临时点通道信息
 */
typedef struct tagChannel
{
    CPoint center;
    int    width;
    float  angle;         /// 临时点通道选择角度方向
} CHANNEL;

/**
 * @brief: 矩形障碍物信息
 */
typedef struct tagObStraRect
{
    CPoint center;        /// 矩形障碍物中心
    int    width;         /// 矩形障碍物宽
    int    height;        /// 矩形障碍物高
    float  angle;         /// 矩形障碍物旋转角度
} OBSTARECT;

/**
 * @brief: 圆形障碍物信息
 */
typedef struct tagObStraRound
{
    CPoint  center;        /// 圆形障碍物中心
    float   radius;        /// 圆形障碍物半径
} OBSTAROUND;

/**
 * @brief: 不规则障碍物信息
 */
typedef struct tagObstaInfo {
    int    nfringeP;       /// 边界点数
    CPoint center;         /// 边界中心点
    CPoint fringeP[100];   /// 边界点信息数组
} OBSTAINFO;

/**
 * @brief: 图像预处理校正参数有效标志
 */
typedef struct tagImgProcessSet
{
    BOOL bDistort;          /// 图像失真处理标志
    BOOL bSubstrate;        /// 图像饱和度衬底处理标志
    BOOL bLighten;          /// 图像亮度处理标志
    BOOL bSmoothing;        /// 图像平滑处理标志
} IMGPROCESSSET;

/**
 * @brief: 水池图像信息参数设置
 */
typedef struct tagImgDisplayPara
{
    int startX;             /// X起点
    int startY;             /// Y起点
    int imgWidth;           /// 水池图像宽度
    int imgHeight;          /// 水池图像高度
    int imgDistortPara;     /// 图像失真系数
    int imgLightPara;       /// 图像亮暗系数
    int imgBrightness;      /// 图像亮度系数
    int imgContrast;        /// 图像对比度系数
    int imgHue;             /// 图像色调系数
    int imgSaturation;      /// 图像饱和度系数
} IMGDISPLAYPARA;

/**
 * @brief: 信息参数显示有效标志
 */
typedef struct tagInfoDisplay
{
    BOOL bDisplayFishpos;           /// 显示鱼的位置
    BOOL bDisplayFishspeed;         /// 显示鱼的速度
    BOOL bDisplayFishspeeddir;      /// 显示鱼的速度方向
    BOOL bDisplayFishdir;           /// 显示鱼的运动方向
    BOOL bDisplayCommandsent;       /// 显示发送的命令
    BOOL bDisplayCommandrec;        /// 显示反馈的命令
    BOOL bDisplayNormal;            /// 显示当前状态是否正常
    BOOL bDisplayBallpos;           /// 显示水球的位置
    BOOL bDisplayBallspeed;         /// 显示水球的速度
    BOOL bDisplayBallspeeddir;      /// 显示水球的速度方向
    BOOL bDisplayObstapos;          /// 显示障碍物的位置
    BOOL bDisplayObstaspeed;        /// 显示障碍物的速度
    BOOL bDisplayObstaspeeddir;     /// 显示障碍物的速度方向
} INFODISPLAY;

/**
 * @brief: 信息参数保存有效标志
 */
typedef struct tagInfoSave
{
    BOOL bSaveFishpos;              /// 保存鱼的位置
    BOOL bSaveFishspeed;            /// 保存鱼的速度
    BOOL bSaveFishspeeddir;         /// 保存鱼的速度方向
    BOOL bSaveFishdir;              /// 保存鱼的运动方向
    BOOL bSaveCommandsent;          /// 保存发送的命令
    BOOL bSaveCommandrec;           /// 保存反馈的命令
    BOOL bSaveNormal;               /// 保存当前状态是否正常
    BOOL bSaveBallpos;              /// 保存水球的位置
    BOOL bSaveBallspeed;            /// 保存水球的速度
    BOOL bSaveBallspeeddir;         /// 保存水球的速度方向
    BOOL bSaveObstapos;             /// 保存障碍物的位置
    BOOL bSaveObstaspeed;           /// 保存障碍物的速度
    BOOL bSaveObstaspeeddir;        /// 保存障碍物的速度方向
} INFOSAVE;

/**
 * @brief: 目标位置与方位信息结构体
 */
typedef struct tagTargetPos {
	int x;                          /// 位置坐标x
	int y;                          /// 位置坐标y
	float Azimuth;                  /// 方位角:目标与X轴的夹角
} TARGETPOS;

/**
 * @brief: 与机器鱼模板相关的线结构体，在CTargetFish类内使用
 */
typedef struct tagTLINE {
    float x0;	                   /// 线起点坐标x
    float y0;                      /// 线起点坐标y
    float x1;                      /// 线终点坐标x
    float y1;                      /// 线终点坐标y
} TLINE;

/**
 * @brief: 与机器鱼平台正常初始化运行所需的结构参数结构体
 */
#if 0
typedef struct tagRobotPara {
    int sBaud;                     /// 机器鱼串口通信频率
    IMGPROCESSSET img;             /// 水池图像参数
    int nFish;                     /// 机器鱼数目
    int* idFish;                   /// 机器鱼id编号
    int nBall;                     /// 水球数目
    int* idBall;                   /// 水球id
    int* radiusBall;               /// 水球半径
    int* diamBall;                 /// 水球直径
    COLORREF* colorBall;           /// 水球颜色值
    char* imgBackName;             /// 背景图像完整路径与名字
} ROBOTPARA;
#endif

/**
 * @brief: 鼠标操作选球选鱼删球删鱼的结构参数结构体
 */
typedef struct tagTargetOPPara {
    int id;                       /// 目标id编号
    int operator_Flag;            /// 操作标志(选球/删球/选鱼/删鱼等)
    int mouse_Event;              /// 鼠标事件
    int x;                        /// 鼠标位置x
    int y;                        /// 鼠标位置y
} TARGETBALLPARA, TARGETFISHPARA;

/**
 * @brief: 图像头信息的结构参数结构体
 */
typedef struct tagIMGHeaderInfoPara {
    int width;                   /// 图像宽度
    int height;                  /// 图像高度
    int depth;                   /// 颜色深度(8)
    int nchanel;                 /// 信号通道数(3)
} IMGHEADERINFOPARA;

/**
 * @brief: NoneColorFish文件的结构参数结构体
 */
typedef struct tagNoneColorFishFilePara {
    int ball_radius;            /// 球半径
    int front_threshold;        /// 前景阈值
    double fish_triSize;        /// 鱼的大小
    double fish_triAlpha;       /// 鱼的长宽比
    int bshow_model;            /// 显示模板
    int bshow_front;            /// 显示前景
} NONECOLORFISHFILEPARA;

/**
 * @brief: 串口参数的结构参数结构体
 */
typedef struct tagSerialCOMPara {
    char port[32];              /// 串口端口号
    int BaudRate;               /// 串口波特率
    int ByteSize;               /// 数据位(7/8)
    int StopBits;               /// 停止位(1/1.5/2)
    int Parity;                 /// 效验位 (NOPARITY/ODDPARITY/EVENPARITY/MARKPARITY)
    int bOpenComFlag;           /// 是否打开串口标志
} SERIALCOMPARA;

#endif /*MUR_STRUCT_H*/