#ifndef __PRIMITIVES_HPP
#define __PRIMITIVES_HPP

#include <memory>

//render() function is used for debugging mode
//it can be removed (with this include) to make a standalone library
#include <SFML/Graphics.hpp>

class Circle;
class Line;

//Abstract primitive, allows "dynamic type" usage of other primitives intersection
//and general primitive interface
class Primitive {
public:
	//Double-dispatch functions providing actual calculations for function 'intersects'
	virtual bool _intersects(const Circle&, double, double) const = 0;
	virtual bool _intersects(const Line&, double, double) const = 0;

	//detects intersection between two Primitives
	//intersects(prim, sx, sy) answers the question:
	//	does *this collide with prim if you shift prim (add to its position) by sx, sy?
	virtual bool intersects(const Primitive&, double, double) const = 0;

	virtual void rotate(double, double, double) = 0;
	
	//shift(sx, sy) moves the primitive to its original position + (sx, sy)
	virtual void shift(double, double) = 0;

	virtual void scale(double) = 0;

	//copies *this and returns a unique_ptr pointing to the location of the copy
	virtual std::unique_ptr<Primitive> clone() const = 0;
	virtual void render(sf::RenderWindow&, double, double) const = 0;
};

class Circle : public Primitive {
public:
	double x, y, r;
	Circle(double, double, double);
	bool _intersects(const Circle&, double, double) const override;
	bool _intersects(const Line&, double, double) const override;
	bool intersects(const Primitive&, double, double) const override;
	void rotate(double, double, double) override;
	void scale(double ratio) override { x *= ratio; y *= ratio; r *= ratio; }
	void shift(double dx, double dy) override { x += dx; y += dy; }
	std::unique_ptr<Primitive> clone() const override;
	void render(sf::RenderWindow&, double, double) const override;
};

//Line segment
class Line : public Primitive {
public:
	double ax, ay, bx, by;

	//wine(ax, ay, bx, by) creates a line segment through two points (ax, ay) and (bx,by)
	Line(double, double, double, double);

	bool _intersects(const Circle&, double, double) const override;
	bool _intersects(const Line&, double, double) const override;
	bool intersects(const Primitive&, double, double) const override;
	void rotate(double, double, double) override;
	void scale(double ratio) override { ax *= ratio; ay *= ratio; bx *= ratio; by *= ratio; }
	void shift(double dx, double dy) override { ax += dx; ay += dy; bx += dx; by += dy; }
	std::unique_ptr<Primitive> clone() const override;
	void render(sf::RenderWindow&, double, double) const override;

	//general form equation: a()*x + b()*y + c() = 0
	double a() const;
	double b() const;
	double c() const;
};

#endif
