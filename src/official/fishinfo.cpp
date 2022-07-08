/**
 * @note 源码已经稍做调整以适配项目
 * @author zymelaii
 * @date 2022-06-26
 */

#include "../official/fishinfo.h"
#include "../official/externrobot.h"
#include "../mathtools.h"
#include <exception>

CFishInfo::CFishInfo() :
    m_fHeaderP(nil, nil), m_fTargetPos(nil, nil), m_fInterP(nil, nil),
    m_fCenterP(nil, nil), m_fLastCenterP(nil, nil),
    m_fRotateP(nil, nil), m_fLastRotateP(nil, nil),
    m_fdirection(nil), m_flastdirection(nil), m_fvdir(nil), 
    m_ftime(0),
    m_fv(0), m_fav(0),
    m_fTargetPosDir(0) {
    // PASS
}

int& CFishInfo::fishId() {
    return m_fID;
}

const int& CFishInfo::fishId() const {
    return m_fID;
}

int& CFishInfo::teamId() {
    return m_fTeamID;
}

const int& CFishInfo::teamId() const {
    return m_fTeamID;
}

const char* CFishInfo::teamName() {
    return m_fTeamName;
}

const char* const CFishInfo::teamName() const {
    return m_fTeamName;
}

CPoint CFishInfo::centerPos() const {
    return m_fCenterP;
}

CPoint CFishInfo::lastCenterPos() const {
    return m_fLastCenterP;
}

CPoint CFishInfo::headerPos() const {
    return m_fHeaderP;
}

CPoint CFishInfo::getHeaderPosAdvance(double radius, double direction) {
    assert(radius > 0);
    m_fdirection = normalizeAngle(direction);
    return {
        static_cast<int>(radius * cos(m_fdirection)) + m_fHeaderP.x,
        static_cast<int>(radius * sin(m_fdirection)) + m_fHeaderP.y,
    };
}

CPoint CFishInfo::jointPos() const {
    return m_fRotateP;
}

double CFishInfo::currentDirection() const {
    return m_fdirection;
}

double CFishInfo::lastDirection() const {
    return m_flastdirection;
}

double CFishInfo::velocity() const {
    return m_fv;
}

double CFishInfo::velocityDirection() const {
    return m_fvdir;
}
    
double CFishInfo::angularVelocity() const {
    return m_fav;
}

bool CFishInfo::hasTarget() const {
    return m_fTargetPos != CPoint(nil, nil);
}

CPoint CFishInfo::targetPos() const {
    return m_fTargetPos;
}

CFishInfo& CFishInfo::setTarget(const CPoint &pos, double direction) {
    m_fTargetPos = pos;
    m_fTargetPosDir = direction;
    return *this;
}

double CFishInfo::directionToTarget(bool correct) const {
    /**
     * @note 40是一个看上去不那么近的距离，不知道为什么会是40
     */
    if (!hasTarget() || getDistance(m_fHeaderP, m_fTargetPos) < 40) {
        return nil;
    }

    double result = normalizeAngle(m_fdirection - getVecAngle(m_fCenterP, m_fTargetPos));

    return result + (!correct || m_fLastAction.direction == 4) ? 0 :
        (m_fLastAction.direction > 4 ? 1 : -1) * M_PI / 18;
}

/**
 * @note 不要尝试读懂下面的代码，尤其是计算相关的部分，全tm是个间断点！
 *  我就不信有什么插值函数能插出一坨间断点来。
 */
bool CFishInfo::calcInterPos(CFishAction& action, bool apply) {
    const int n = 4;
    const double distance = getDistance(m_fHeaderP, m_fTargetPos);

    if (!hasTarget() || distance < 5) {
        return false;
    }

    CPoint midpt {
        (m_fTargetPos.x + m_fHeaderP.x) / 2,
        (m_fTargetPos.y + m_fHeaderP.y) / 2,
    };

    double k1 = tan(normalizeAngle(m_fTargetPosDir + M_PI / 2));
    double k3 = tan(normalizeAngle(getVecAngle(m_fHeaderP, m_fTargetPos) + M_PI / 2));

    CPoint center;
    center.x = static_cast<int>((midpt.y - m_fTargetPos.y + k1 * m_fTargetPos.x - k3 * midpt.x) / (k1 - k3) + 0.5);
    center.y = static_cast<int>(m_fTargetPos.y + k1 * (center.x - m_fTargetPos.x) + 0.5);

    double k2 = getVecAngle(center, m_fHeaderP);
    double k4 = getVecAngle(center, m_fTargetPos);

    k3 = normalizeAngle(m_fTargetPosDir - getVecAngle(m_fTargetPos, m_fHeaderP));

    /**
     * @note 这真是一个神奇的数学公式，第一个case的情况非常好理解，
     *  但是第二个case简直莫名其妙，其数学表达式也是莫名其妙，虽然
     *  我重写了它把30行的逻辑总结成了寥寥几行，但是我水平不够，理
     *  解不了第二个式子。
     */
    double k5 = fabs(k3) > M_PI / 2 ?
        normalizeAngle(k2 + normalizeAngle(k4 - k2) / n) :
        normalizeAngle(k2 + (k4 - k2) / n + 2 * M_PI *
            (fabs(k4 - k2) > M_PI ? 0 : (k4 - k2 > 0 ? -1 : 1)));

    double r = getDistance(center, m_fHeaderP);
    m_fInterP.x = static_cast<int>(clamp(center.x + r * cos(k5) + 0.5, 0, REGBMP_WIDTH));
    m_fInterP.y = static_cast<int>(clamp(center.y + r * sin(k5) + 0.5, 0, REGBMP_HEIGHT));

    if (!apply) {
        return true;
    }

    if (m_fCurrentAction.speed) {
        double result = normalizeAngle(
            getVecAngle(m_fHeaderP, m_fInterP) - m_fdirection) / M_PI * 180;

        int nd = static_cast<int>(result / ((static_cast<int>(clamp(distance / 100, 0, 2) + 2)) * 5));
        action.direction = 7 + static_cast<int>(clamp(nd, -7, 7));
        action.speed = m_fCurrentAction.speed;

        /**
         * @brief 直行的情况
         * @note 以下代码归玄学管，没人知道它怎么得出来的，
         *  也没人知道这段代码到底有没有用。
         */
        if (action.direction == 7) {
            if (fabs(result) > 15) {
                action.speed = 15;
            } else {
                action.speed = 15 - static_cast<int>(fabs(result));
            }
        }
    } else {
        action.speed = static_cast<int>(clamp(distance / 15 + 1, 0, 15));
    }

    return true;
}

CPoint CFishInfo::interPos() const {
    return m_fInterP;
}

const CFishAction& CFishInfo::currentAction() const {
    return m_fCurrentAction;
}

bool CFishInfo::getLastAction(CFishAction& action) const {
    /**
     * @note 速度小于零表示上一次动作尚未被更新（不存在）
     */
    if (m_fLastAction.speed < 0) {
        return false;
    }
    action = m_fLastAction;
    return true;
}

CFishInfo& CFishInfo::updateAction(const CFishAction &action) {
    m_fLastAction = m_fCurrentAction;
    m_fCurrentAction = action;
    return *this;
}

CFishInfo& CFishInfo::update(CPoint pos, double direction, uint8_t duration) {
    m_fLastCenterP = m_fCenterP;
    m_fCenterP = pos;

    m_flastdirection = m_fdirection;
    m_fdirection = direction;

    m_ftime = duration;

    updateHeaderPos();
    updateJointPos();
    updateVelocities();

    return *this;
}

void CFishInfo::updateHeaderPos() {
    /**
     * @note 关于坐标系：
     *  externrobot.h中定义的鱼头到中心的坐标为水池坐标系，
     *  而当前对象所维护的坐标皆在全局视觉形成的图像坐标系下，
     *  故如下需做相应坐标变换。
     */
    const double ratio_x = REGBMP_WIDTH * 1.0 / POOL_WIDTH;
    const double ratio_y = REGBMP_HEIGHT * 1.0 / POOL_HEIGHT;
    m_fHeaderP.x = static_cast<decltype(m_fHeaderP.x)>(
        m_fCenterP.x + HEADTOCENTER_DIS * cos(m_fdirection) * ratio_x);
    m_fHeaderP.y = static_cast<decltype(m_fHeaderP.y)>(
        m_fCenterP.y + HEADTOCENTER_DIS * sin(m_fdirection) * ratio_y);
}

/**
 * @note 以下第一关节计算方法存疑，慎用！
 * @author zymelaii
 * @date 2022-06-27
 */
void CFishInfo::updateJointPos() {
    /**
     * @note 关于坐标系：
     *  externrobot.h中定义的第一关节到中心的坐标为水池坐标系，
     *  而当前对象所维护的坐标皆在全局视觉形成的图像坐标系下，
     *  故如下需做相应坐标变换。
     */
    const double ratio_x = REGBMP_WIDTH * 1.0 / POOL_WIDTH;
    const double ratio_y = REGBMP_HEIGHT * 1.0 / POOL_HEIGHT;
    m_fLastRotateP = m_fRotateP;
    m_fRotateP.x = static_cast<decltype(m_fRotateP.x)>(
        m_fCenterP.x  + ROTATETOCENTER_DIS * cos(m_fdirection) * ratio_x);
    m_fRotateP.y = static_cast<decltype(m_fRotateP.y)>(
        m_fCenterP.y  + ROTATETOCENTER_DIS * sin(m_fdirection) * ratio_y);
}

/**
 * @note 以下保留关于速度与坐标的坐标系差异问题，慎用！
 * @note 以下关于角速度的计算方法存疑，慎用！
 * @author zymelaii
 * @date 2022-06-27
 */
void CFishInfo::updateVelocities() {
    /**
     * @note 初始化阶段，速度直接置零
     */
    if (m_fLastRotateP == CPoint(nil, nil)) {
        m_fv = 0;
        m_fav = 0;
        return;
    }
    /**
     * @note 关于坐标系：速度量维护的是水池坐标系下的情形，注意坐标变换。
     */
    const double dt = m_ftime * TIMESTEP;
    CPoint displacement = m_fRotateP - m_fLastRotateP;
    displacement.x = static_cast<decltype(displacement.x)>(
        displacement.x * POOL_WIDTH * 1.0 / REGBMP_WIDTH);
    displacement.y = static_cast<decltype(displacement.y)>(
        displacement.y * POOL_HEIGHT * 1.0 / REGBMP_HEIGHT);

    m_fv = getDistance(CPoint(0, 0), displacement) / dt;
    m_fvdir = getVecAngle(m_fLastRotateP, m_fRotateP);

    /**
     * @note 假定采样时间足够短，以下将角度变换量视为同向的角度变换，
     *  不考虑摇摆的情况。
     */
    m_fav = fabs(m_fdirection - m_flastdirection) / dt;
}