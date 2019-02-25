#include <vector>

class Polygon {
	using vd = vector<pair<double,double>>;
private:
	vd points;
public:
	Polygon(const vd& points) : points(points) {}
	Polygon(vd&& points) : points(move(points)) {}

}
