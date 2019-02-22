#include "primitives.hpp"
#include <cmath>
#include <utility>
#include <memory>

//SMAZAT INCLUDE
#include <iostream>
#include <SFML/Graphics.hpp>

std::pair<double, double> _rotate_point(double x, double y, double cx, double cy, double angle) {
	angle = angle/180.0*3.1415926535;
	double s = sin(angle);
	double c = cos(angle);

	x -= cx;
	y -= cy;
	double nx = x*c - y*s;
	double ny = x*s + y*c;
	return {nx + cx, ny + cy};
}

double _distance(double ax, double ay, double bx, double by) {
	double dx = bx-ax;
	double dy = by-ay;
	return sqrt(dx*dx+dy*dy);
}

Circle::Circle(double x, double y, double r) : x(x), y(y), r(r) {}

void Circle::rotate(double cx, double cy, double angle) {
	/*angle = angle/180.0*3.1415926535;
	double s = sin(angle);
	double c = cos(angle);
	x -= cx;
	y -= cy;
	double nx = x*c - y*s;
	double ny = x*s + y*c;
	x = nx + cx;
	y = ny + cy;*/
	auto res = _rotate_point(x,y,cx,cy,angle);
	x = res.first;
	y = res.second;
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

bool Circle::_intersects(const Line& other, double sx, double sy) const {
	return other._intersects(*this, -sx, -sy);
}

std::unique_ptr<Primitive> Circle::clone() const {
	return std::make_unique<Circle>(*this);
}

void Circle::render(sf::RenderWindow& window, double px, double py) const {
	sf::CircleShape myc(r);
	myc.setPosition(px + x - r, py + y - r);
	myc.setFillColor(sf::Color::Transparent);
	myc.setOutlineThickness(-3);
	myc.setOutlineColor(sf::Color::Red);
	window.draw(myc);
}

Line::Line(double ax, double ay, double bx, double by) : ax(ax), ay(ay), bx(bx), by(by) {}

void Line::rotate(double cx, double cy, double angle) {
	auto res1 = _rotate_point(ax,ay,cx,cy,angle);
	auto res2 = _rotate_point(bx,by,cx,cy,angle);

	ax = res1.first;
	ay = res1.second;
	bx = res2.first;
	by = res2.second;
}

bool Line::intersects(const Primitive& other, double sx, double sy) const {
	return other._intersects(*this, -sx, -sy);
}

bool Line::_intersects(const Line& other, double sx, double sy) const {
	return true;
}

//Currently calculates line (not line SEGMENT as intended) - point distance
bool Line::_intersects(const Circle& other, double sx, double sy) const {
	double dy = by-ay;
	double dx = bx-ax;
	//distance is either perpendicular distance or distance to the close endpoint
	double distance = abs((by - ay)*(other.x+sx) - (bx-ax)*(other.y+sy) + bx*ay - by*ax)/(sqrt(dx*dx+dy*dy));
	//double edistance = std::min(_distance(other.x + sx, other.y + sy, ax, ay), _distance(other.x + sx, other.y + sy, bx, by));
	//distance = std::max(distance, edistance);
	return distance <= other.r;
}
	
std::unique_ptr<Primitive> Line::clone() const {
	return std::make_unique<Line>(*this);
}

void Line::render(sf::RenderWindow& window, double px, double py) const {
	sf::Vertex line[2];
	line[0].position = sf::Vector2f(ax+px,ay+py);
	line[1].position = sf::Vector2f(bx+px,by+py);
	window.draw(line, 2, sf::Lines);
	//TODO :)
}
