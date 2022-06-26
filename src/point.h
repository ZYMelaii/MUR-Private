#ifndef POINT_H
#define POINT_H

#include "./common.h"

struct CPoint : public POINT {
	CPoint() {
		x = y = 0;
	}
	CPoint(int X, int Y) {
		x = X;
		y = Y;
	}
	CPoint(const POINT &pt) {
		x = pt.x;
		y = pt.y;
	}
	CPoint operator-(const CPoint &rhs) const {
		return { x - rhs.x, y - rhs.y };
	}
	bool operator==(const CPoint &rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const CPoint &rhs) const {
		return x != rhs.x || y != rhs.y;
	}
};

#endif /*POINT_H*/