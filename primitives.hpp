#ifndef __PRIMITIVES_HPP
#define __PRIMITIVES_HPP

#include <memory>

//SMAZAT INCLUDE PO DODEBUGOVANI
#include <SFML/Graphics.hpp>

class Circle;
class Line;

class Primitive {
public:
	virtual bool _intersects(const Circle&, double, double) const = 0;
	virtual bool _intersects(const Line&, double, double) const = 0;
	virtual bool intersects(const Primitive&, double, double) const = 0;
	virtual void rotate(double, double, double) = 0;
	virtual void shift(double, double) = 0;
	virtual void scale(double) = 0;
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

class Line : public Primitive {
public:
	double ax, ay, bx, by;
	Line(double, double, double, double);
	bool _intersects(const Circle&, double, double) const override;
	bool _intersects(const Line&, double, double) const override;
	bool intersects(const Primitive&, double, double) const override;
	void rotate(double, double, double) override;
	void scale(double ratio) override { ax *= ratio; ay *= ratio; bx *= ratio; by *= ratio; }
	void shift(double dx, double dy) override { ax += dx; ay += dy; bx += dx; by += dy; }
	std::unique_ptr<Primitive> clone() const override;
	void render(sf::RenderWindow&, double, double) const override;
};

#endif
