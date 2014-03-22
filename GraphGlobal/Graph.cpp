#include "Graph.h"
#include <algorithm>

void Node::ChangeConn(size_t i, size_t j)
{
	for (auto it = vi.begin(); it != vi.end(); it++)
	if (*it == i)
	{
		vi.erase(it);
		break;
	}
	
	AddConn(j);
}

void Node::AddConn(size_t i)
{	
	bool b = false;
	for (auto it = vi.begin(); it != vi.end(); it++)
	if (*it == i)
	{
		b = true;
		break;
	}
	
	if (!b)
		vi.push_back(i);
}


bool Graph::Inters(const Point &start1, const Point &end1, const Point &start2, const Point &end2, Point &r) const
{
	Point dir1 = end1 - start1;
	Point dir2 = end2 - start2;

	//считаем уравнения прямых проходящих через отрезки
	double a1 = -dir1.y;
	double b1 = +dir1.x;
	double d1 = -(a1*start1.x + b1*start1.	y);

	double a2 = -dir2.y;
	double b2 = +dir2.x;
	double d2 = -(a2*start2.x + b2*start2.y);

	//подставляем концы отрезков, для выяснения в каких полуплоскотях они
	double seg1_line2_start = a2*start1.x + b2*start1.y + d2;
	double seg1_line2_end = a2*end1.x + b2*end1.y + d2;

	double seg2_line1_start = a1*start2.x + b1*start2.y + d1;
	double seg2_line1_end = a1*end2.x + b1*end2.y + d1;

	//если концы одного отрезка имеют один знак, значит он в одной полуплоскости и пересечения нет.
	if (seg1_line2_start * seg1_line2_end >= 0 || seg2_line1_start * seg2_line1_end >= 0)
		return false;

	double u = seg1_line2_start / (seg1_line2_start - seg1_line2_end);
	r = start1 + u*dir1;

	return true;
}

Graph::Graph(const vector<double> &X, const vector<double> &Y)
{
	size_t n = std::min(X.size(), Y.size());
	
	for (size_t i = 0; i < n; i++) // Add nodes
	{		
		nodes.push_back(Node(Point(X[i], Y[i]), {}));		

		// Add connections
		if (i > 0)
			nodes[i].AddConn(nodes.size() - 2);
		if (i < n - 1)
			nodes[i].AddConn(nodes.size());
	}
	nodes.back().AddConn(0);
	nodes.front().AddConn(nodes.size() - 1);

	n = nodes.size(); // new size without repitions
	Point r(0,0);
	for (size_t i = 0; i < n - 1; i++) // Check for intersections
	for (size_t j = i + 2; j < n - 1; j++)
	if (Inters(nodes[i].p, nodes[i + 1].p, nodes[j].p, nodes[j + 1].p, r))
	{
		// Check wheather we're adding new node, or changing existing node
		size_t k;
		for (k = 0; k < nodes.size(); k++)		
		if ((r - nodes[k].p).norm() < eps)
				break;
		
		// Disconnect old nodes and connect them with new one
		nodes[i].ChangeConn(i + 1, k);
		nodes[i + 1].ChangeConn(i, k);
		nodes[j].ChangeConn(j + 1, k);
		nodes[j + 1].ChangeConn(j, k);


		if (k == nodes.size()) // adding new node
		{
			nodes.push_back(Node(Point(r), {i, i + 1, j, j + 1}));
		}
		else // connecting existing node with others
		{
			nodes[k].AddConn(i);
			nodes[k].AddConn(i + 1);
			nodes[k].AddConn(j);
			nodes[k].AddConn(j + 1);
		}
	}	

	for (size_t i = 0; i < n - 1; i++)
	if (Inters(nodes[i].p, nodes[i + 1].p, nodes[n - 1].p, nodes[0].p, r)) // Working with last segment
	{
		// Check wheather we're adding new node, or changing existing node
		size_t k;
		for (k = 0; k < nodes.size(); k++)
		if ((r - nodes[k].p).norm() < eps)
			break;

		// Disconnect old nodes and connect them with new one
		nodes[i].ChangeConn(i + 1, k);
		nodes[i + 1].ChangeConn(i, k);
		nodes[n - 1].ChangeConn(0, k);
		nodes[0].ChangeConn(n - 1, k);


		if (k == nodes.size()) // adding new node
		{
			nodes.push_back(Node(Point(r), { i, i + 1, n - 1, 0 }));
		}
		else // connecting existing node with others
		{
			nodes[k].AddConn(i);
			nodes[k].AddConn(i + 1);
			nodes[k].AddConn(n - 1);
			nodes[k].AddConn(0);
		}
	}
}

ostream &operator<<(ostream &os, const Point &p)
{		
	return os << "( " << p.x << " , " << p.y << " )";
}

ostream &operator<<(ostream &os, const Figure &f)
{
	os << f.v.size() - 1 << "-point figure:\n";
	for (size_t i = 0; i < f.v.size() - 1; i++)
	{
		os << f.v[i] << std::endl;
	}
	return os;
}

vector<Figure> Graph::process()
{
	vector<Figure> v;
	vector<Point> f;
	vector<size_t> path;
	std::set<size_t> u;

	for (size_t i = 0; i < nodes.size(); i++)
	{
		// search for minimum unused x
		size_t min = 0;
		while (u.find(min) != u.end())
			min++;
		double xmin = nodes[min].p.x;
		for (size_t j = min + 1; j < nodes.size(); j++)
		if (nodes[j].p.x < xmin && u.find(j) == u.end())
		{
			xmin = nodes[j].p.x;
			min = j;
		}
		u.insert(min);

		// form all figures starting with this x
		Compare comp(nodes[min].p, nodes, true);
		std::sort(nodes[min].vi.begin(), nodes[min].vi.end(), comp);
		for (size_t j = 0; j < nodes[min].vi.size(); j++)
		{
			path.clear();
			path.push_back(min);			
			rec(nodes[min].vi[j], path);
			if (path.front() == path.back() && path.size() > 1)
			{
				f.clear();
				for (size_t k = 0; k < path.size(); k++)
					f.push_back(nodes[path[k]].p);
				v.push_back(Figure(f));
			}
		}
	}

	return v;
}

void Graph::rec(size_t i, vector<size_t> &v) const
{
	if (v[0] == i)
	{
		v.push_back(i);
		return;
	}
	if (nodes[i].p.x > nodes[v.front()].p.x)
		return;		
	if (nodes[i].p.x == nodes[v.front()].p.x && nodes[i].p.y < nodes[v.front()].p.y)
		return;
	// search for the vertex with minimum angle
	Point rel = nodes[i].p - nodes[v.back()].p;
	size_t min = 0;
	double xmin = M_PI;
	for (size_t j = 0; j < nodes[i].vi.size(); j++)
	{
		double angle = rel ^ (nodes[nodes[i].vi[j]].p - nodes[i].p);
		if (angle < xmin && nodes[i].vi[j] != v.back())
		{
			xmin = angle;
			min = nodes[i].vi[j];
		}
	}
	if (xmin > 0) // we can't go clockwise
		return;
	// pushing current vertex to stack
	v.push_back(i);
	rec(min, v);	
}