#include "mathtools.h"

double clamp(double value, double min, double max) {
	if (value < min) {
		return min;
	} else if (value > max) {
		return max;
	} else {
		return value;
	}
}

double normalizeAngle(double angle, bool is_radian) {
	const double unit = is_radian ? 2 * M_PI : 360.0;
	if (angle < 0) {
		return angle + (int)(-angle / unit) * unit;
	} else {
		return angle - (int)(angle / unit) * unit;
	}
}

double getDistance(const CPoint &a, const CPoint &b) {
	const double dx = a.x - b.x, dy = a.y - b.y;
	return sqrt(dx * dx + dy * dy);
}
#include <iostream>
double getVecAngle(const CPoint &origin, const CPoint &dest) {
	CPoint vec(dest.x - origin.x, dest.y - origin.y);
	return atan2(vec.y, vec.x);
}