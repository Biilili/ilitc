#pragma once
#include <QVector>
#include <sstream>
#include <string>
#include <iomanip>
#include "basic.h"

#define PI 3.1416

class Geometry
{
protected:
    QVector<Point*> p;
    Color BorderColor;
    int BorderStyle;
    Color FillColor;
    int FillStyle;
    double BorderWidth;
public:
    virtual ~Geometry() { }
    QVector<Point*> getP() { return p; }//获取图形顶点容器
    Color& getBorderColor() { return BorderColor; }
    Color& getFillColor() { return FillColor; }
    double& getBorderWidth() { return BorderWidth; }
    void setP(QVector<Point*> geo) { p = geo; }
    void setBorderColor(Color c) { BorderColor = c; }
    void setFillColor(Color c) { FillColor = c; }
    void setBorderWidth(double BW) { BorderWidth = BW; }
    virtual void Build(char* buf, int size) { }
    virtual const char* getname() = 0;
    virtual BBox GetBBox() const = 0; //获取最小外包矩形
    virtual double getArea() = 0;
    virtual double getLength() = 0;
    virtual QString wkt() = 0;           //兼容自定义数据格式转wkt格式
};

class CPoint :public Geometry
{
public:
    CPoint()
    {
        BorderColor.R = 255, BorderColor.G = 0, BorderColor.B = 0;
        this->BorderWidth = 3;
        FillColor.R = 255, FillColor.G = 0, FillColor.B = 0;
    }
    CPoint(QVector<Point*> geom, int borderR = 255, int borderG = 0, int borderB = 0, int BorderWidth = 1, int FillR = 255, int FillG = 0, int FillB = 0)
    {
        p = geom;
        BorderColor.R = borderR, BorderColor.G = borderG, BorderColor.B = borderB;
        this->BorderWidth = BorderWidth;
        FillColor.R = FillR, FillColor.G = FillG, FillColor.B = FillB;
    }
    const char* getname()
    {
        return "Point";
    }
    double getArea()
    {
        return 0;
    }
    double getLength()
    {
        return 0;
    }
    BBox GetBBox() const
    {
        BBox BoundingBox;
        BoundingBox.center = p[0][0];
        BoundingBox.xmin = p[0][0].x - 10;
        BoundingBox.ymin = p[0][0].y - 10;
        BoundingBox.xmax = p[0][0].x + 10;
        BoundingBox.ymax = p[0][0].y + 10;
        return BoundingBox;
    }
    QString wkt()
    {
        std::ostringstream ofs;
        ofs << "Point(";
        //Point(3 4)
        ofs << p[0][0].x << " " << p[0][0].y;
        ofs << ")";
        return QString::fromStdString(ofs.str());
    }

    void Build(char* buf, int size)
    {
        int numPoints = *(int*)(buf + 40);
        int* Parts = (int*)(buf + 44);
        Point* pt = (Point*)(buf + 48);
        p.push_back(&pt[0]);
    }
};

class CLine :public Geometry
{
public:
    CLine()
    {
        BorderColor.R = 217, BorderColor.G = 208, BorderColor.B = 245;
        this->BorderWidth = 3;
        FillColor.R = 217, FillColor.G = 208, FillColor.B = 245;
    }
    CLine(QVector<Point*>& geom, int borderR = 217, int borderG = 208, int borderB = 245, int BorderWidth = 3,
        int FillR = 217, int FillG = 208, int FillB = 245)
    {
        p = geom;
        BorderColor.R = borderR, BorderColor.G = borderG, BorderColor.B = borderB;
        this->BorderWidth = BorderWidth;
        FillColor.R = FillR, FillColor.G = FillG, FillColor.B = FillB;
    }
    const char* getname()
    {
        return "Line";
    }
    double getArea()
    {
        return 0;
    }
    double getLength()
    {
        double len = (double)(sqrt(pow((p[1]->x - p[0]->x), 2) + pow((p[1]->y - p[0]->y), 2)));
        return len;
    }
    BBox GetBBox() const
    {
        BBox BoundingBox;
        BoundingBox.xmin = p[0]->x; BoundingBox.ymin = p[0]->y;
        BoundingBox.xmax = p[0]->x; BoundingBox.ymax = p[0]->y;
        for (int i = 0; i < p.size(); i++) {
            if (p[i]->x > BoundingBox.xmax)BoundingBox.xmax = p[i]->x;
            if (p[i]->y > BoundingBox.ymax)BoundingBox.ymax = p[i]->y;
            if (p[i]->x < BoundingBox.xmin)BoundingBox.xmin = p[i]->x;
            if (p[i]->y < BoundingBox.ymin)BoundingBox.ymin = p[i]->y;
            if (BoundingBox.xmax == BoundingBox.xmin) { BoundingBox.xmax += 1; BoundingBox.xmin -= 1; }
            if (BoundingBox.ymax == BoundingBox.ymin) { BoundingBox.ymax += 1; BoundingBox.ymin -= 1; }
        }
        BoundingBox.center.x = (BoundingBox.xmax + BoundingBox.xmin) / 2;
        BoundingBox.center.y = (BoundingBox.ymax + BoundingBox.ymin) / 2;
        return BoundingBox;
    }
    QString wkt()
    {
        std::ostringstream ofs;
        ofs << "LINESTRING(";
        //LINESTRING(3 4, 10 50, 20 25)
        for (int i = 0; i < p.size(); i++) {
            ofs << p[i]->x << " " << p[i]->y;
            if (i < p.size() - 1)
                ofs << ",";
            else
                ofs << ")";
        }
        return QString::fromStdString(ofs.str());
    }
    void Build(char* buf, int size) {
        int numPoints = *(int*)(buf + 40);
        int* Parts = (int*)(buf + 44);
        Point* pt = (Point*)(buf + 48);

        for (int i = 0; i < numPoints; i++) {
            p.push_back(&pt[i]);
        }
    }
};

class CPolygon :public Geometry
{
public:
    CPolygon()
    {
        BorderColor.R = 255, BorderColor.G = 255, BorderColor.B = 218;
        this->BorderWidth = 3;
        FillColor.R = 255, FillColor.G = 255, FillColor.B = 218;
    }
    CPolygon(QVector<Point*>& geom, int borderR = 255, int borderG = 255, int borderB = 128
        , int BorderWidth = 3, int FillR = 255, int FillG = 255, int FillB = 128)
    {
        p = geom;
        BorderColor.R = borderR, BorderColor.G = borderG, BorderColor.B = borderB;
        this->BorderWidth = BorderWidth;
        FillColor.R = FillR, FillColor.G = FillG, FillColor.B = FillB;
    }
    const char* getname()
    {
        return"Polygon";
    }
    double getArea()
    {
        //返回多边形面积
        double area = 0;
        for (int i = 1; i < p.size(); i++) {
            area += (double)(p[i]->x * p[i - 1]->y - p[i]->y * p[i - 1]->x) / 2.0;
        }
        area += (double)(p[0]->x * p[p.size() - 1]->y - p[0]->y * p[p.size() - 1]->x) / 2.0;//第1和第n个点单独处理
        if (area < 0.0) area = area;//顺时针与逆时针输入结果互为相反数
        return area;
    }
    double getLength()
    {
        //返回多边形周长
        double len = 0;
        for (int i = 1; i < p.size(); i++) {
            len += (double)(sqrt(pow((p[i]->x - p[i - 1]->x), 2) + pow((p[i]->y - p[i - 1]->y), 2)));
        }
        len += (double)(sqrt(pow((p[0]->x - p[p.size() - 1]->x), 2) + pow((p[0]->y - p[p.size() - 1]->y), 2)));//第1和第n个点单独处理
        return len;
    }
    BBox GetBBox() const
    {
        BBox BoundingBox;
        BoundingBox.xmin = p[0]->x; BoundingBox.ymin = p[0]->y;
        BoundingBox.xmax = p[0]->x; BoundingBox.ymax = p[0]->y;
        for (int i = 0; i < p.size(); i++) {
            if (p[i]->x > BoundingBox.xmax)BoundingBox.xmax = p[i]->x;
            if (p[i]->y > BoundingBox.ymax)BoundingBox.ymax = p[i]->y;
            if (p[i]->x < BoundingBox.xmin)BoundingBox.xmin = p[i]->x;
            if (p[i]->y < BoundingBox.ymin)BoundingBox.ymin = p[i]->y;
        }
        BoundingBox.center.x = (BoundingBox.xmax + BoundingBox.xmin) / 2;
        BoundingBox.center.y = (BoundingBox.ymax + BoundingBox.ymin) / 2;
        return BoundingBox;
    }
    QString wkt()
    {
        //POLYGON((1 1,5 1,5 5,1 5,1 1))
        std::ostringstream ofs;
        //ofs<<setiosflags(ios_base::scientific);
        ofs << std::setiosflags(std::ios_base::fixed);
        ofs << "Polygon((";
        for (int i = 0; i < p.size(); i++) {
            ofs << std::setprecision(10) << p[i]->x << " " << std::setprecision(10) << p[i]->y;
            if (i < p.size() - 1)
                ofs << ",";
            else
                ofs << "))";
        }
        return QString::fromStdString(ofs.str());
    }
    void Build(char* buf, int size) {
        int numParts = *(int*)(buf + 36);
        int numPoints = *(int*)(buf + 40);
        int* Parts = (int*)(buf + 44);
        Point* pt = (Point*)(buf + 44 + 4 * numParts);

        for (int i = 0; i < Parts[0]; i++) {
            p.push_back(&pt[i]);
        }
    }
};

class CRectangle : public Geometry
{
public:
    CRectangle(QVector<Point*>& geom, int borderR = 255, int borderG = 128, int borderB = 128,
        int BorderWidth = 3, int FillR = 255, int FillG = 128, int FillB = 128)
    {
        p = geom;
        BorderColor.R = borderR, BorderColor.G = borderG, BorderColor.B = borderB;
        this->BorderWidth = BorderWidth;
        FillColor.R = FillR, FillColor.G = FillG, FillColor.B = FillB;
    }
    const char* getname()
    {
        return"Rectangle";
    }
    double getArea()
    {
        // A=ab
        double edge1 = getEdge1();
        double edge2 = getEdge2();
        return edge1 * edge2;
    }
    double getLength()
    {
        // C=2(a+b)
        double edge1 = getEdge1();
        double edge2 = getEdge2();
        return 2 * (edge1 + edge2);
    }
    double getEdge1()
    {
        double edge1 = abs(p[1]->x - p[0]->x);
        return edge1;
    }
    double getEdge2()
    {
        double edge2 = abs(p[1]->y - p[0]->y);
        return edge2;
    }
    BBox GetBBox() const
    {
        BBox BoundingBox;
        if (p[0]->x > p[1]->x) { BoundingBox.xmin = p[1]->x; BoundingBox.xmax = p[0]->x; }
        else { BoundingBox.xmin = p[0]->x; BoundingBox.xmax = p[1]->x; }
        if (p[0]->y > p[1]->y) { BoundingBox.ymin = p[1]->y; BoundingBox.ymax = p[0]->y; }
        else { BoundingBox.ymin = p[0]->y; BoundingBox.ymax = p[1]->y; }
        BoundingBox.center.x = (BoundingBox.xmax + BoundingBox.xmin) / 2;
        BoundingBox.center.y = (BoundingBox.ymax + BoundingBox.ymin) / 2;
        return BoundingBox;
    }
    QString wkt()
    {
        BBox box = GetBBox();

        std::ostringstream ofs;
        //ofs<<setiosflags(ios_base::scientific);
        ofs << std::setiosflags(std::ios_base::fixed);
        ofs << "Polygon((";
        ofs << std::setprecision(10) << box.xmin << " " << std::setprecision(10) << box.ymin << ",";  //第一个点
        ofs << std::setprecision(10) << box.xmax << " " << std::setprecision(10) << box.ymin << ",";//第二个点
        ofs << std::setprecision(10) << box.xmax << " " << std::setprecision(10) << box.ymax << ",";//第三个点
        ofs << std::setprecision(10) << box.xmin << " " << std::setprecision(10) << box.ymax << "))"; //第四个点

        return QString::fromStdString(ofs.str());
    }
};

class CCircle :public Geometry

{
public:
    CCircle(QVector<Point*>& geom, int borderR = 197, int borderG = 232, int borderB = 128, int borderWidth = 3, int FillR = 197, int FillG = 232, int FillB = 128)
    {
        p = geom;
        BorderColor.R = borderR, BorderColor.G = borderG, BorderColor.B = borderB;
        this->BorderWidth = borderWidth;
        FillColor.R = FillR, FillColor.G = FillG, FillColor.B = FillB;
    }
    const char* getname()
    {
        return"Circle";
    }
    double getr() const
    {
        double r = (sqrt(pow((p[1]->x - p[0]->x), 2) + pow((p[1]->y - p[0]->y), 2)));
        return r;
    }
    double getArea()
    {
        double r = getr();
        return PI * r * r;
    }
    double getLength()
    {
        double r = getr();
        return 2 * PI * r;
    }
    BBox GetBBox() const
    {
        BBox BoundingBox;
        double r = getr();
        BoundingBox.center = *p[0];
        BoundingBox.xmin = p[0]->x - r;
        BoundingBox.ymin = p[0]->y - r;
        BoundingBox.xmax = p[0]->x + r;
        BoundingBox.ymax = p[0]->y + r;
        return BoundingBox;
    }
    QString wkt()
    {
        BBox box = GetBBox();
        double r = getr();
        double p = box.xmax - r;
        double q = box.center.y;
        std::ostringstream ofs;
        //ofs<<setiosflags(ios_base::scientific);
        ofs << std::setiosflags(std::ios_base::fixed);
        ofs << "Polygon((";
        Point temp;
        for (int i = 1; i < 100; i++) {
            temp.x = p + r * cos(2 * PI * (i - 1) / i);
            temp.y = q + r * sin(2 * PI * (i - 1) / i);
            ofs << std::setprecision(10) << temp.x << " " << std::setprecision(10) << temp.y << ",";
        }
        temp.x = p + r * cos(2 * PI * (100 - 1) / 100);
        temp.y = q + r * sin(2 * PI * (100 - 1) / 100);
        ofs << std::setprecision(10) << temp.x << " " << std::setprecision(10) << temp.y << "))";
        return QString::fromStdString(ofs.str());
    }
};
