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
	double dx = x - (other.x + sx);
	double dy = y - (other.y + sy);
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
	std::cout << "__MISSING CODE :-)__" << std::endl;
	return true;
}

double Line::a() const {
	return by-ay;
}
double Line::b() const {
	return -(bx-ax);
}
double Line::c() const {
	return bx*ay - ax*by;
}

bool Line::_intersects(const Circle& other, double sx, double sy) const {
/*	std::cout << "line: " << ax  << " " << ay << " " << bx << " " << by << std::endl;
	std::cout << "circ: " << other.x << " " << other.y<< " " << other.r << std::endl;
	std::cout << "line circ " << sx << " " << sy << std::endl;
//	std::cout << other.x << " " << other.y << std::endl;
	std::cout << "_____" << std::endl;*/
	double cx = other.x + sx;
	double cy = other.y + sy;
	//my line: ax + by + c = 0
	//perp line through cx, cy: bx - ay + k = 0; k = a*cy - b*cx
	//their intersection (= projection of sx, sy to my line) is 

	double k = a() * cy - b() * cx;
	double px = -(b()*k + a()*c())/(a()*a() + b()*b());
	double py = (k*a()-c()*b())/(a()*a() + b()*b());

	double distance = abs(a() * cx + b() * cy + c())/(sqrt(a()*a() + b()*b()));

	if (px >= std::min(ax, bx) && px <= std::max(ax,bx) && py >= std::min(ay,by) && py <= std::max(ay,by))
		return distance <= other.r;
	else{
		return std::min(_distance(cx, cy, ax, ay), _distance(cx, cy, bx, by)) <= other.r;
	}
//	double distance2 = sqrt((px-cx)*(px-cx) + (py-cy)*(py-cy));

	//std::cout << "DIS: " << distance << std::endl;
	//double distance = abs((by - ay)*(other.x+sx) - (bx-ax)*(other.y+sy) + bx*ay - by*ax)/(sqrt(dx*dx+dy*dy));
	//double edistance = std::min(_distance(other.x + sx, other.y + sy, ax, ay), _distance(other.x + sx, other.y + sy, bx, by));
	//distance = std::max(distance, edistance);
}
	
std::unique_ptr<Primitive> Line::clone() const {
	return std::make_unique<Line>(*this);
}

void Line::render(sf::RenderWindow& window, double px, double py) const {
	sf::Vector2f slope(a(), b());
	slope = 2.0f*slope / (float)sqrt(a()*a()+b()*b());
	sf::VertexArray line(sf::Quads, 4);
	line[0].position = sf::Vector2f(ax+px,ay+py);
	line[1].position = sf::Vector2f(ax+px,ay+py) + slope;
	line[2].position = sf::Vector2f(bx+px,by+py) + slope;
	line[3].position = sf::Vector2f(bx+px,by+py);
	line[0].color = sf::Color::Red;
	line[1].color = sf::Color::Red;
	line[2].color = sf::Color::Red;
	line[3].color = sf::Color::Red;
	window.draw(line);
	//TODO :)
}
