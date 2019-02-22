#ifndef __HITBOX_HPP
#define __HITBOX_HPP

class Circle;
class Line;

class Primitive {
public:
	virtual bool _intersects(const Circle&, double, double) const = 0;
	//virtual bool _intersects(const Line&, double, double) const = 0;
	virtual bool intersects(const Primitive&, double, double) const = 0;
	virtual void rotate(double, double, double) = 0;
};

class Circle : public Primitive {
public:
	double x, y, r;
	Circle(double, double, double);
	bool _intersects(const Circle&, double, double) const override;
	//bool _intersects(const Line&, double, double) const override;
	bool intersects(const Primitive&, double, double) const override;
	void rotate(double, double, double) override;
};

class Line : public Primitive {
	double ax, ay, bx, by;
	Line(double, double, double, double);
};

#endif
