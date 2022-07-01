/**
 * @note 源码已经稍做调整以适配项目
 * @author zymelaii
 * @date 2022-06-26
 */

#include "../official/ballinfo.h"
#include "../official/externrobot.h"
#include "../mathtools.h"

CBallInfo::CBallInfo() {
	last_centerPt = {-1, -1};
	centerPt = {-1, -1};
	time = 0;
}

CPoint CBallInfo::getCenter() const {
	return centerPt;
}

CPoint CBallInfo::getPrevCenter() const {
	return last_centerPt;
}

double CBallInfo::getSpeed() const {
	return v_ball;
}

double CBallInfo::getDirection() const {
	return v_direction;
}

double CBallInfo::getRadius() const {
	return radius;
}

CPoint CBallInfo::calcHitPoint(const CPoint &goal) const {
	double direction = normalizeAngle(getVecAngle(centerPt, goal));
	return {
		static_cast<int>(centerPt.x - radius * cos(direction)),
		static_cast<int>(centerPt.y - radius * sin(direction)),
	};
}

CBallInfo& CBallInfo::updateVelocity() {
	if (centerPt.x == -1 && centerPt.y == -1) {
		v_ball = v_direction = 0;
	} else {
		/**
		 * @note 水球的坐标信息是通过全局视觉获取得到的图像坐标，
		 *  需做等比变换将图像坐标系下的坐标变换到水池坐标系。
		 */
		double duration = time * TIMESTEP;
		double dx = (centerPt.x - last_centerPt.x) * POOL_WIDTH / REGBMP_WIDTH;
		double dy = (centerPt.y - last_centerPt.y) * POOL_HEIGHT / REGBMP_HEIGHT;
		v_direction = getVecAngle(centerPt, last_centerPt);
		v_ball = sqrt(dx * dx + dy * dy) / duration;
	}
	return *this;
}

CBallInfo& CBallInfo::setProperty(CPoint pos, uint8_t duration, double r) {
	last_centerPt = centerPt;
	centerPt = pos;
	time = duration;
	radius = r;
	return updateVelocity();
}

CBallInfo& CBallInfo::getProperty(BALLINFO &info) {
	info.centerPt = centerPt;
	info.last_centerPt = last_centerPt;
	info.v_ball = v_ball;
	info.v_direction = v_direction;
	info.radius = radius;
	info.time = time;
	return *this;
}

CBallInfo& CBallInfo::operator=(const BALLINFO &other) {
	centerPt = other.centerPt;
	last_centerPt = other.last_centerPt;
	v_ball = other.v_ball;
	v_direction = other.v_direction;
	radius = other.radius;
	time = other.time;
	return *this;
}