#include "hitbox.hpp"
#include <cmath>

Circle::Circle(double x, double y, double r) : x(x), y(y), r(r) {}

void Circle::rotate(double cx, double cy, double angle) {
	angle = angle/180.0*3.1415926535;
	double s = sin(angle);
	double c = cos(angle);
	x -= cx;
	y -= cy;
	double nx = x*c - y*s;
	double ny = x*s + y*c;
	x = nx + cx;
	y = ny + cy;
}

bool Circle::intersects(const Primitive& other, double sx, double sy) const {
	return other._intersects(*this, -sx, -sy);
}

bool Circle::_intersects(const Circle& other, double sx, double sy) const {
	double dx = x-other.x + sx;
	double dy = y-other.y + sy;
	if (sqrt(dx*dx + dy*dy) < r + other.r)
		return true;
	return false;
}


