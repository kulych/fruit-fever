#include "primitives.hpp"
#include <cmath>
#include <utility>
#include <memory>

//SMAZAT INCLUDE
#include <iostream>
#include <SFML/Graphics.hpp>


//#OTAZKA, chtel bych mit v "knihovne" funkci, kterou pouziva vice funkci v ruznych tridach (napr _rotate_point, _distance)
//Zaroven asi nechci, aby se vtirala vsude, kam si nekdo knihovnu includne, kam ji mam dat? Zatim je tu takhle globalne.
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
	auto res = _rotate_point(x,y,cx,cy,angle);
	x = res.first;
	y = res.second;
}

bool Circle::intersects(const Primitive& other, double sx, double sy) const {
	return other._intersects(*this, -sx, -sy);
}

bool Circle::_intersects(const Circle& other, double sx, double sy) const {
	//Two circles intersect iff the distance between their centers is at most the
	//sum of their diameters
	double dx = x - (other.x + sx);
	double dy = y - (other.y + sy);
	if (sqrt(dx*dx + dy*dy) <= r + other.r)
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
	//Circle center in *this line coordinate system
	double cx = other.x + sx;
	double cy = other.y + sy;

	//my line: a()* x + b() * y + c = 0
	//perp line through cx, cy: b()*x - a()*y + k = 0; k = a*cy - b*cx
	//their intersection (= projection of cx, cy to *this line) is 
	double k = a() * cy - b() * cx;
	double px = -(b()*k + a()*c())/(a()*a() + b()*b());
	double py = (k*a()-c()*b())/(a()*a() + b()*b());

	double distance = abs(a() * cx + b() * cy + c())/(sqrt(a()*a() + b()*b()));

	//If projection is on the line segment, use that as the circle-line distance measuring point, otherwise use the 
	//closer end of line segment
	if (px >= std::min(ax, bx) && px <= std::max(ax,bx) && py >= std::min(ay,by) && py <= std::max(ay,by))
		return distance <= other.r;
	else{
		return std::min(_distance(cx, cy, ax, ay), _distance(cx, cy, bx, by)) <= other.r;
	}
}
	
std::unique_ptr<Primitive> Line::clone() const {
	return std::make_unique<Line>(*this);
}

void Line::render(sf::RenderWindow& window, double px, double py) const {
	//SFML cannot draw bold lines - make a quad with the other two points
	//moved in perpendicular direction
	sf::Vector2f perp(a(), b());
	perp = 2.0f * perp / (float)sqrt(a()*a()+b()*b());

	sf::VertexArray line(sf::Quads, 4);
	line[0].position = sf::Vector2f(ax+px,ay+py);
	line[1].position = sf::Vector2f(ax+px,ay+py) + perp;
	line[2].position = sf::Vector2f(bx+px,by+py) + perp;
	line[3].position = sf::Vector2f(bx+px,by+py);
	line[0].color = sf::Color::Red;
	line[1].color = sf::Color::Red;
	line[2].color = sf::Color::Red;
	line[3].color = sf::Color::Red;

	window.draw(line);
}
