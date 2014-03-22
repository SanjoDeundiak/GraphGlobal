#define _USE_MATH_DEFINES

#include <vector>
#include <set>
#include <stack>
#include <iterator>
#include <iostream>
#include <cmath>


using std::vector;
using std::ostream;
using std::set;

const double eps = 1E-6;

class Point
{
	friend class Graph;
	double x;
	double y;

public:
	Point(double x_, double y_) : x(x_), y(y_) {};

	double norm() const { return sqrt(x*x + y*y); };	
	
	// operators
	Point operator+(const Point &other) const { return Point(x + other.x, y + other.y); };
	Point operator-(const Point &other) const { return Point(x - other.x, y - other.y); };
	Point operator*(double n) const { return Point(x*n, y*n); };
	double operator*(const Point &other) const { return x*other.x + y*other.y; };
	bool operator==(const Point &other) const { return abs(x - other.x) < eps && abs(y - other.y) < eps; };
	double operator^(const Point &other) const  
	{
		double c = (*this * other)/norm()/other.norm();
		if (c < -1)
			return -M_PI;
		if (c > 1)
			return 0;

		double angle = acos(c);
		if (x * other.y - y * other.x < 0)
			angle *= -1;

		return angle;
	};	
	friend Point operator*(double n, const Point &p) { return p*n; };

	// output operator
	friend ostream &operator<<(ostream &os, const Point &p);	
};

class Node
{	
	friend class Graph;
	friend class Compare;
	Point p; // coordinates
	vector<size_t> vi; // neighborhoods of node
	bool u;

public:
	Node(const Point &p_, const vector<size_t> &vi_) : p(p_), vi(vi_), u(false) {};
	void ChangeConn(size_t i, size_t j);
	void AddConn(size_t i);
	bool operator==(const Node &other) const { return p == other.p; };
};

class Figure
{
	friend class Graph;
	Figure(const vector<Point> &v_) : v(v_) {};
	vector<Point> v;
	friend ostream &operator<<(ostream &os, const Figure &f);
};

class Graph
{
	bool Inters(const Point &p1, const Point &p2, const Point &q1, const Point &q2, Point &r) const; // Check for intersection
	vector<Node> nodes;
	void rec(size_t i, vector<size_t> &v) const;
public:
	Graph(const vector<double> &X, const vector<double> &y);
	vector<Figure> process();
};

class Compare
{
	bool f; // true for anti-clockwise, false for clockwise
	Point p;
	const vector<Node> &nodes;
public:
	Compare(const Point &p_, const vector<Node> &nodes_, bool f_) : p(p_), f(f_), nodes(nodes_) {};
	bool operator() (size_t p1, size_t p2)
	{
		if (f)
			return ((nodes[p1].p - p) ^ (nodes[p2].p - p)) < 0;
		else
			return ((nodes[p1].p - p) ^ (nodes[p2].p - p)) > 0;
	}
};