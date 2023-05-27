#pragma once

#include <QString>

struct Point
{
public:
    Point() {}
    Point(double x, double y) :x(x), y(y) { }
    double x, y;
};


struct BBox
{
    double xmin;
    double xmax;
    double ymin;
    double ymax;
    Point center;
};


struct Color
{
public:
    Color() {}
    Color(int R, int G, int B) : R(R), G(G), B(B) { }
    int R, G, B;
};


class Layer
{
public:
    Layer() {}
    Layer(QString qs) {}
};
