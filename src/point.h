#ifndef POINT_H
#define POINT_H

#include "./common.h"

struct CPoint : public POINT {
	constexpr CPoint() {
		x = y = 0;
	}
	constexpr CPoint(int X, int Y) {
		x = X;
		y = Y;
	}
	constexpr CPoint(const POINT &pt) {
		x = pt.x;
		y = pt.y;
	}
	constexpr CPoint operator-(const CPoint &rhs) const {
		return { x - rhs.x, y - rhs.y };
	}
	constexpr bool operator==(const CPoint &rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	constexpr bool operator!=(const CPoint &rhs) const {
		return x != rhs.x || y != rhs.y;
	}
};

#endif /*POINT_H*/