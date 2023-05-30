#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QMouseEvent>
#include <QPainter>
#include <fstream>
#include <string>
#include <sstream>
#include <QDebug>
#include <cmath>
#include <QListWidget>
#include <QFileDialog>
#include <QWheelEvent>
#include <QColorDialog>
#include <QDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QInputDialog>
#include <iomanip>
#include <QString>
#include <QString>
#include "ui_tuxing.h"

using namespace std;

const double PI = 3.141593;

QT_BEGIN_NAMESPACE
namespace Ui { class TuXingClass; }
QT_END_NAMESPACE

extern QWidget* myMainWindow;

double distance(double x, double y, double x0, double y0);
//BASIC
struct Point
{
public:
    Point() { }
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

//struct Color
//{
//public:
//    Color() {}
//    Color(int R, int G, int B) : R(R), G(G), B(B) { }
//    int R, G, B;
//};

class Graphic
{
protected:
    QVector<Point*> p;
    QColor BorderColor;
    int BorderStyle = 1;
    QColor FillColor;
    int FillStyle = 1;
    double BorderWidth = 0;
    double Radius = -1;
public:
    virtual ~Graphic() { }
    QVector<Point*> getP() { return p; }//获取图形顶点容器
    QColor& getBorderColor() { return BorderColor; }
    QColor& getFillColor() { return FillColor; }
    double& getBorderWidth() { return BorderWidth; }
    void setP(QVector<Point*> geo) { p = geo; }
    void setBorderColor(QColor c) { BorderColor = c; }
    void setFillColor(QColor c) { FillColor = c; }
    void setBorderWidth(double BW) { BorderWidth = BW; }
    virtual void Build(char* buf, int size) { }
    virtual const char* getname() = 0;
    virtual BBox GetBBox() const = 0; //获取最小外包矩形
    virtual double getArea() = 0;
    virtual double getLength() = 0;
    virtual QString wkt() = 0;           //兼容自定义数据格式转wkt格式
    virtual QString gisk() = 0;
};

extern int selectView;

class View
{
protected:
    int createStep;
    int translateStep;
    int rotateStep;
    int scaleStep;
    int activePoint;
    Point translateOrginPoint;
    Point rotateOrginPoint;
    Point scaleOrginPoint;
public:
    View() :createStep(0), translateStep(0), rotateStep(0), scaleStep(0), activePoint(-1) {}
    virtual BBox get_box() = 0;
    virtual QVector<Point*>& get_p() = 0;
    virtual QColor& get_borderColor() = 0;
    virtual QColor& get_fillColor() = 0;
    virtual int& get_borderWidth() = 0;
    virtual void show(QPainter& painter) = 0;
    virtual void create(double x, double y) = 0;
    void edit(double x, double y)
    {
        double minDistance = 30;
        int flag = 0;
        for (int i = 0; i < get_p().size(); i++)
        {
            double d = sqrt(pow((x - get_p()[i]->x), 2) + pow((y - get_p()[i]->y), 2));
            if (d < minDistance)
            {
                minDistance = d;
                activePoint = i;
                flag = 1;
            }
        }
        if (flag == 0)
            selectView = -1;
    }
    void translate(double x, double y)
    {
        if (translateStep == 0)
        {
            translateOrginPoint.x = x;
            translateOrginPoint.y = y;
            translateStep++;
        }
        else if (translateStep == 1)
        {
            for (int i = 0; i < get_p().size(); i++)
            {
                get_p()[i]->x += x - translateOrginPoint.x;
                get_p()[i]->y += y - translateOrginPoint.y;
            }
            translateOrginPoint.x = x;
            translateOrginPoint.y = y;
        }
    }
    void rotate(double x, double y)
    {
        if (rotateStep == 0)
        {
            rotateOrginPoint.x = x;
            rotateOrginPoint.y = y;
            rotateStep++;
        }
        else if (rotateStep == 1)
        {
            double temp = ((rotateOrginPoint.x - get_box().center.x) * (x - get_box().center.x) + (rotateOrginPoint.y - get_box().center.y) * (y - get_box().center.y))
                / (distance(rotateOrginPoint.x, rotateOrginPoint.y, get_box().center.x, get_box().center.y) * distance(x, y, get_box().center.x, get_box().center.y));
            double theta = acos(temp);
            for (int i = 0; i < get_p().size(); i++)
            {
                Point pointTemp;
                pointTemp.x = get_box().center.x + (get_p()[i]->x - get_box().center.x) * cos(theta) - (get_p()[i]->y - get_box().center.y) * sin(theta);
                pointTemp.y = get_box().center.y + (get_p()[i]->x - get_box().center.x) * sin(theta) + (get_p()[i]->y - get_box().center.y) * cos(theta);
                get_p()[i]->x = pointTemp.x;
                get_p()[i]->y = pointTemp.y;
            }
        }
    }
    void scale(double x, double y)
    {
        if (scaleStep == 0)
        {
            scaleOrginPoint.x = x;
            scaleOrginPoint.y = y;
            scaleStep++;
        }
        else if (scaleStep == 1)
        {
            double time;
            if ((y - get_box().center.y) / (x - get_box().center.x) > 1)
                time = (y - get_box().center.y) / (get_box().center.y - get_box().center.y);
            else
                time = (x - get_box().center.x) / (scaleOrginPoint.x - get_box().center.x);

            for (int i = 0; i < get_p().size(); i++)
            {
                get_p()[i]->x = get_p()[i]->x * time + get_box().center.x * (1 - time);
                get_p()[i]->y = get_p()[i]->y * time + get_box().center.y * (1 - time);
            }
            scaleOrginPoint.x = x;
            scaleOrginPoint.y = y;
        }
    }
    void modifyBorderColor()
    {
        QColor BorderColor = QColorDialog::getColor(Qt::yellow);
       
    }
    void modifyFillColor()
    {
        QColor fillColor = QColorDialog::getColor(Qt::yellow);
        
    }
    void modifyBorderWidth()
    {
        bool ok;
        int age = QInputDialog::getInt(myMainWindow, "Modify Border Width", "Please input border width of geometry", 3, 0, 10, 1, &ok);
        if (ok)
            get_borderWidth() = age;
    }
    void editMoveAuxiliary(double x, double y)
    {
        if (activePoint != -1)
        {
            get_p()[activePoint]->x = x;
            get_p()[activePoint]->y = y;
        }
    }
    void editReleaseAuxiliary()
    {
        activePoint = -1;
    }
    double distance(double x, double y, double x0, double y0)
    {
        return sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
    }
};

extern View* view;

extern QVector<int> selectViewSet;

extern QVector<View*> selectSet;

class Layer;

extern Layer* currentLayer;

class Layer
{
protected:
    QString LayerFileName;
    QVector<View*> pv;
    int step;
    double xmin, xmax, ymin, ymax;
    bool isIntersect(Point* p, double xmin, double ymin, double xmax, double ymax)
    {
        return (p->x > xmin && p->x < xmax && p->y > ymin && p->y < ymax) ? true : false;
    }
public:
    Layer(QString str) : step(0)
    {
        LayerFileName = str;
    }
    QVector<View*>& getVector()
    {
        return pv;
    }
    void select(double x, double y)
    {
        selectSet.clear();
        Point* temp = new Point(x, y);
        for (int i = 0; i < pv.size(); i++)
        {
            if (isIntersect(temp, pv[i]->get_box().xmin, pv[i]->get_box().ymin, pv[i]->get_box().xmax, pv[i]->get_box().ymax))
                selectView = i;
        }
        delete temp;
    }
    void boxSelect(double x, double y)
    {
        if (step == 0)
        {
            selectView = -1;
            selectSet.clear();
            xmin = x;
            xmax = x;
            ymin = y;
            ymax = y;
            step++;
        }
        else if (step == 1)
        {
            xmin = (x < xmin) ? x : xmin;
            xmax = (x > xmax) ? x : xmax;
            ymin = (y < ymin) ? y : ymin;
            ymax = (y > ymax) ? y : ymax;
            for (int i = 0; i < pv.size(); i++)
            {
                int flag = 1;
                for (int j = 0; j < pv[i]->get_p().size(); j++)
                {
                    if (!isIntersect(pv[i]->get_p()[j], xmin, ymin, xmax, ymax))
                    {
                        flag = 0;
                    }
                }
                if (flag == 1)
                    selectSet.push_back(currentLayer->getVector()[i]);
            }
            step = 0;
        }
    }
    void del()
    {
        int res = QMessageBox::warning(myMainWindow, "Delete geometry", "Are you sure you want to permanently delete this geometry?",
            QMessageBox::Ok |
            QMessageBox::Cancel,
            QMessageBox::Cancel);
        if (res == QMessageBox::Ok)
        {
            if (selectView != -1)
            {
                QVector<View*>::iterator iter = pv.begin();
                for (int i = 0; i < selectView; i++)
                {
                    iter++;
                }
                pv.erase(iter);
                selectView = -1;
            }
        }
    }
};

extern QVector<Layer*> LayerList;

//class Handle
//{
//public:
//    Handle() { }
//    virtual ~Handle() { }
//    virtual const char* getName() = 0;
//    virtual void mousePressEvent(double x, double y) = 0;
//    virtual void mouseMoveEvent(double x, double y) = 0;
//    virtual void mouseReleaseEvent(double x, double y) = 0;
//};

//extern Handle* pt;

//class Command : public QObject
//{
//public:
//    Command() { }
//    virtual const char* getName() = 0;
//    virtual void Invoke() = 0;
//};

//extern QVector<Command*> menuInfo;

class CPoint :public Graphic
{
public:
    CPoint()
    {
        BorderColor = QColor("#000000");
        FillColor = QColor("#000000");
        BorderWidth = 3; BorderStyle = 1; FillStyle = 1;
    }
    CPoint(QVector<Point*> geom, std::string bordercolor, int borderstyle)
    {
        p = geom;
        BorderColor = QColor(BorderColor);
        FillColor = QColor(BorderColor);
        BorderStyle = BorderStyle;
        FillStyle = BorderStyle;
        //        if (p.size() == 1)
        //        {
        //            cal_box();
        //        }
    }
    const char* getname()
    {
        return "POINT";
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
        BoundingBox.center = *p[0];
        BoundingBox.xmin = p[0]->x - 10;
        BoundingBox.ymin = p[0]->y - 10;
        BoundingBox.xmax = p[0]->x + 10;
        BoundingBox.ymax = p[0]->y + 10;
        return BoundingBox;
    }
    QString wkt()
    {
        std::ostringstream ofs;
        ofs << "POINT (";
        //POINT (3 4)
        ofs << p[0][0].x << " " << p[0][0].y;
        ofs << ")";
        return QString::fromStdString(ofs.str());
    }
    QString gisk()
    {
        std::ostringstream ofs;
        ofs << "POINT (";
        //POINT (3 4)
        ofs << p[0][0].x << " " << p[0][0].y;
        ofs << ")";
        ofs << ' ' << BorderColor.name().toStdString() << ' ' << BorderStyle;
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

class CLine :public Graphic
{public:
    CLine()
    {
        BorderColor = QColor("#000000");
        FillColor = QColor("#00000000");
        BorderWidth = 3; BorderStyle = 1; FillStyle = 1;
    }
    CLine(QVector<Point*>& geom, double width, std::string bordercolor, int borderstyle)
    {
        p = geom;
        BorderWidth = width;
        BorderColor = QColor(QString::fromStdString(bordercolor));
        FillColor = BorderColor;
        BorderStyle = borderstyle;
        FillStyle = BorderStyle;
    }
    const char* getname()
    {
        return "LINESTRING";
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
        ofs << "LINESTRING (";
        //LINESTRING(3 4, 10 50, 20 25)
        for (int i = 0; i < p.size(); i++)
        {
            ofs << p[i]->x << " " << p[i]->y;
            if (i < p.size() - 1)
                ofs << ",";
            else
                ofs << ")";
        }
        return QString::fromStdString(ofs.str());
    }

    QString gisk()
    {
        std::ostringstream ofs;
        ofs << "LINESTRING (";
        //LINESTRING (3 4, 5 6)
        for (int i = 0; i < p.size(); i++)
        {
            ofs << p[i]->x << " " << p[i]->y;
            if (i < p.size() - 1)
                ofs << ",";
            else
                ofs << ")";
        }
        ofs << ' ' << BorderWidth << ' ' << BorderColor.name().toStdString() << ' ' << BorderStyle;
        return QString::fromStdString(ofs.str());
    }
    void Build(char* buf, int size)
    {
        int numPoints = *(int*)(buf + 40);
        int* Parts = (int*)(buf + 44);
        Point* pt = (Point*)(buf + 48);

        for (int i = 0; i < numPoints; i++)
        {
            p.push_back(&pt[i]);
        }
    }
};

class CPolygon :public Graphic
{
public:
    CPolygon()
    {
        BorderColor = QColor("#000000");
        FillColor = QColor("#00000000");
        BorderWidth = 3; BorderStyle = 1; FillStyle = 1;
    }

    CPolygon(QVector<Point*>& geom, double width, std::string bordercolor, int borderstyle, std::string fillcolor, int fillstyle)
    {
        p = geom;
        BorderWidth = width;
        BorderColor = QColor(QString::fromStdString(bordercolor));
        FillColor = QColor(QString::fromStdString(fillcolor));
        BorderStyle = borderstyle;
        FillStyle = fillstyle;
    }

    const char* getname()
    {
        return"POLYGON";
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
        ofs << "POLYGON ((";
        for (int i = 0; i < p.size(); i++) {
            ofs << std::setprecision(10) << p[i]->x << " " << std::setprecision(10) << p[i]->y;
            if (i < p.size() - 1)
                ofs << ",";
            else
                ofs << "))";
        }
        return QString::fromStdString(ofs.str());
    }

    QString gisk()
    {
        //POLYGON((1 1,5 1,5 5,1 5,1 1))
        std::ostringstream ofs;
        //ofs<<setiosflags(ios_base::scientific);
        ofs << std::setiosflags(std::ios_base::fixed);
        ofs << "POLYGON ((";
        for (int i = 0; i < p.size(); i++) {
            ofs << std::setprecision(10) << p[i]->x << " " << std::setprecision(10) << p[i]->y;
            if (i < p.size() - 1)
                ofs << ",";
            else
                ofs << "))";
        }
        ofs << ' ' << BorderWidth << ' ' << BorderColor.name().toStdString() << ' ' << BorderStyle << ' ' << FillColor.name(QColor::HexArgb).toStdString() << ' ' << FillStyle;
        return QString::fromStdString(ofs.str());
    }

    void Build(char* buf, int size)
    {
        int numPoints = *(int*)(buf + 40);
        int* Parts = (int*)(buf + 44);
        Point* pt = (Point*)(buf + 48);

        for (int i = 0; i < numPoints; i++)
        {
            p.push_back(&pt[i]);
        }
    }
};

class CRectangle : public Graphic
{
public:
    CRectangle()
    {
        BorderColor = QColor("#000000");
        FillColor = QColor("#00000000");
        BorderWidth = 3; BorderStyle = 1; FillStyle = 1;
    }

    CRectangle(QVector<Point*>& geom, double width, std::string bordercolor, int borderstyle, std::string fillcolor, int fillstyle)
    {
        p = geom;
        BorderWidth = width;
        BorderColor = QColor(QString::fromStdString(bordercolor));
        FillColor = QColor(QString::fromStdString(fillcolor));
        BorderStyle = borderstyle;
        FillStyle = fillstyle;
    }
    const char* getname()
    {
        return"RECTANGLE";
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
        ofs << "POLYGON ((";
        ofs << std::setprecision(10) << box.xmin << " " << std::setprecision(10) << box.ymin << ",";  //第一个点
        ofs << std::setprecision(10) << box.xmax << " " << std::setprecision(10) << box.ymin << ",";//第二个点
        ofs << std::setprecision(10) << box.xmax << " " << std::setprecision(10) << box.ymax << ",";//第三个点
        ofs << std::setprecision(10) << box.xmin << " " << std::setprecision(10) << box.ymax << "))"; //第四个点
        ofs << std::setprecision(10) << box.xmin << " " << std::setprecision(10) << box.ymin << ",";  //第一个点
        return QString::fromStdString(ofs.str());
    }

    QString gisk()
    {
        BBox box = GetBBox();

        std::ostringstream ofs;
        //ofs<<setiosflags(ios_base::scientific);
        ofs << std::setiosflags(std::ios_base::fixed);
        ofs << "RECTANGLE (";
        ofs << std::setprecision(10) << box.xmin << " " << std::setprecision(10) << box.ymin << ",";  //第一个点
        ofs << std::setprecision(10) << box.xmax << " " << std::setprecision(10) << box.ymax << ")";
        ofs << ' ' << BorderWidth << ' ' << BorderColor.name().toStdString() << ' ' << BorderStyle << ' ' << FillColor.name(QColor::HexArgb).toStdString() << ' ' << FillStyle;
        return QString::fromStdString(ofs.str());
    }

};

class CCircle :public Graphic
{
public:
    CCircle()
    {
        BorderColor = QColor("#000000");
        FillColor = QColor("#00000000");
        BorderWidth = 3; BorderStyle = 1; FillStyle = 1;
        Radius = 0;
    }
    CCircle(QVector<Point*>& geom, double r, double width, std::string bordercolor, int borderstyle, std::string fillcolor, int fillstyle)
    {
        p = geom;
        BorderWidth = width;
        BorderColor = QColor(QString::fromStdString(bordercolor));
        FillColor = QColor(QString::fromStdString(fillcolor));
        BorderStyle = borderstyle;
        FillStyle = fillstyle;
        Radius = r;
    }
    const char* getname()
    {
        return"CIRCLE";
    }
    double getArea()
    {
        return PI * Radius * Radius;
    }
    double getLength()
    {
        return 2 * PI * Radius;
    }
    BBox GetBBox() const
    {
        BBox BoundingBox;
        BoundingBox.center = *p[0];
        BoundingBox.xmin = p[0]->x - Radius;
        BoundingBox.ymin = p[0]->y - Radius;
        BoundingBox.xmax = p[0]->x + Radius;
        BoundingBox.ymax = p[0]->y + Radius;
        return BoundingBox;
    }
    QString wkt()
    {
        BBox box = GetBBox();
        double p = box.xmax - Radius;
        double q = box.center.y;
        std::ostringstream ofs;
        //ofs<<setiosflags(ios_base::scientific);
        ofs << std::setiosflags(std::ios_base::fixed);
        ofs << "POLYGON ((";
        Point temp;
        for (int i = 0; i <= 100; i++)
        {
            temp.x = p + Radius * cos(2 * PI * i / 100);
            temp.y = q + Radius * sin(2 * PI * i / 100);
            ofs << std::setprecision(10) << temp.x << " " << std::setprecision(10) << temp.y << ",";
        }
        temp.x = p + Radius * cos(0);
        temp.y = q + Radius * sin(0);
        ofs << std::setprecision(10) << temp.x << " " << std::setprecision(10) << temp.y << "))";
        return QString::fromStdString(ofs.str());
    }
    QString gisk()
    {
        std::ostringstream ofs;
        //ofs<<setiosflags(ios_base::scientific);
        ofs << std::setiosflags(std::ios_base::fixed);
        ofs << "CIRCLE (";
        ofs << std::setprecision(10) << p[0]->x << " " << std::setprecision(10) << p[0]->y << ")";  //第一个点
        ofs << ' ' << Radius << ' ' << BorderWidth << ' ' << BorderColor.name().toStdString() << ' ' << BorderStyle << ' ' << FillColor.name(QColor::HexArgb).toStdString() << ' ' << FillStyle;
        return QString::fromStdString(ofs.str());
    }
};

class CSector :public Graphic
{
public:
    CSector()
    {
        BorderColor = QColor("#000000");
        FillColor = QColor("#00000000");
        BorderWidth = 3; BorderStyle = 1; FillStyle = 1;
        Radius = 0;
    }
    CSector(QVector<Point*>& geom, double r, double width, std::string bordercolor, int borderstyle, std::string fillcolor, int fillstyle)
    {
        p = geom;
        BorderWidth = width;
        BorderColor = QColor(QString::fromStdString(bordercolor));
        FillColor = QColor(QString::fromStdString(fillcolor));
        BorderStyle = borderstyle;
        FillStyle = fillstyle;
        Radius = r;
    }
    const char* getname()
    {
        return"SECTOR";
    }
    double getArea()
    {
        return PI * Radius * Radius;
    }
    double getLength()
    {
        return 2 * PI * Radius;
    }
    BBox GetBBox() const
    {
        BBox BoundingBox;
        BoundingBox.center = *p[0];
        BoundingBox.xmin = p[0]->x - Radius;
        BoundingBox.ymin = p[0]->y - Radius;
        BoundingBox.xmax = p[0]->x + Radius;
        BoundingBox.ymax = p[0]->y + Radius;
        return BoundingBox;
    }
    QString wkt()
    {
        BBox box = GetBBox();
        double x0 = box.xmax - Radius;
        double y0 = box.center.y;
        std::ostringstream ofs;
        //ofs<<setiosflags(ios_base::scientific);
        ofs << std::setiosflags(std::ios_base::fixed);
        ofs << "POLYGON ((";
        Point temp;
        ofs << std::setprecision(10) << p[0]->x << " " << std::setprecision(10) << p[0]->y << ",";
        for (double i = p[1]->x; i <= p[1]->y; i += 1)
        {
            temp.x = x0 + Radius * cos(PI * i / 180);
            temp.y = y0 + Radius * sin(PI * i / 180);
            ofs << std::setprecision(10) << temp.x << " " << std::setprecision(10) << temp.y << ",";
        }
        ofs << std::setprecision(10) << p[0]->x << " " << std::setprecision(10) << p[0]->y << "))";
        return QString::fromStdString(ofs.str());
    }

    QString gisk()
    {
        std::ostringstream ofs;
        //ofs<<setiosflags(ios_base::scientific);
        ofs << std::setiosflags(std::ios_base::fixed);
        ofs << "SECTOR (";
        ofs << std::setprecision(10) << p[0]->x << " " << std::setprecision(10) << p[0]->y << ", ";  //第一个点
        ofs << std::setprecision(10) << p[1]->x << " " << std::setprecision(10) << p[1]->y << ")";  //第一个点
        ofs << ' ' << Radius << ' ' << BorderWidth << ' ' << BorderColor.name().toStdString() << ' ' << BorderStyle << ' ' << FillColor.name(QColor::HexArgb).toStdString() << ' ' << FillStyle;
        return QString::fromStdString(ofs.str());
    }
};

class PointView : public CPoint, public View
{

};

class LineView : public CLine, public View
{
};

class PolygonView : public CPolygon, public View
{

};

class RectangleView : public CRectangle, public View
{

};

class CircleView : public CCircle, public View
{

};

class SectorView : public CSector, public View
{

};

//class SelectHandle : public Handle
//{
//public:
//    SelectHandle() { myMainWindow->setCursor(Qt::CrossCursor); }
//    ~SelectHandle() { }
//    const char* getName()
//    {
//        return "SelectHandle";
//    }
//    void mousePressEvent(double x, double y)
//    {
//        currentLayer->select(x, y);
//    }
//    void mouseMoveEvent(double x, double y)
//    {
//
//    }
//    void mouseReleaseEvent(double x, double y)
//    {
//
//    }
//};
//
//class BoxSelectHandle : public Handle
//{
//public:
//    BoxSelectHandle() { myMainWindow->setCursor(Qt::CrossCursor); }
//    ~BoxSelectHandle() { }
//    const char* getName()
//    {
//        return "BoxSelectHandle";
//    }
//    void mousePressEvent(double x, double y)
//    {
//        currentLayer->boxSelect(x, y);
//    }
//    void mouseMoveEvent(double x, double y)
//    {
//
//    }
//    void mouseReleaseEvent(double x, double y)
//    {
//        currentLayer->boxSelect(x, y);
//    }
//};
//
//class CreateHandle : public Handle
//{
//public:
//    CreateHandle() { myMainWindow->setCursor(Qt::CrossCursor); }
//    ~CreateHandle() { }
//    const char* getName()
//    {
//        return "CreateHandle";
//    }
//    void mousePressEvent(double x, double y)
//    {
//        currentLayer->getVector().first()->create(x, y);
//    }
//    void mouseMoveEvent(double x, double y)
//    {
//
//    }
//    void mouseReleaseEvent(double x, double y)
//    {
//
//    }
//};
//
//class EditHandle : public Handle
//{
//public:
//    EditHandle() { myMainWindow->setCursor(Qt::SizeAllCursor); }
//    ~EditHandle() { }
//    const char* getName()
//    {
//        return "EditHandle";
//    }
//    void mousePressEvent(double x, double y)
//    {
//        if (selectView != -1)
//        {
//            currentLayer->getVector()[selectView]->edit(x, y);
//        }
//    }
//    void mouseMoveEvent(double x, double y)
//    {
//        if (selectView != -1)
//            currentLayer->getVector()[selectView]->editMoveAuxiliary(x, y);
//    }
//    void mouseReleaseEvent(double x, double y)
//    {
//        if (selectView != -1)
//        {
//            currentLayer->getVector()[selectView]->editReleaseAuxiliary();
//        }
//    }
//};
//
//class TranslateHandle : public Handle
//{
//public:
//    TranslateHandle() { myMainWindow->setCursor(Qt::SizeAllCursor); }
//    ~TranslateHandle() { }
//    const char* getName()
//    {
//        return "TranslateHandle";
//    }
//    void mousePressEvent(double x, double y)
//    {
//        if (selectView != -1)
//        {
//            currentLayer->getVector()[selectView]->translate(x, y);
//        }
//    }
//    void mouseMoveEvent(double x, double y)
//    {
//        if (selectView != -1)
//            currentLayer->getVector()[selectView]->translate(x, y);
//    }
//    void mouseReleaseEvent(double x, double y)
//    {
//
//    }
//};
//
//class RotateHandle : public Handle
//{
//public:
//    RotateHandle() { myMainWindow->setCursor(Qt::SizeAllCursor); }
//    ~RotateHandle() { }
//    const char* getName()
//    {
//        return "RotateHandle";
//    }
//    void mousePressEvent(double x, double y)
//    {
//        if (selectView != -1)
//        {
//            currentLayer->getVector()[selectView]->rotate(x, y);
//        }
//    }
//    void mouseMoveEvent(double x, double y)
//    {
//        if (selectView != -1)
//            currentLayer->getVector()[selectView]->rotate(x, y);
//    }
//    void mouseReleaseEvent(double x, double y)
//    {
//
//    }
//};
//
//class ScaleHandle : public Handle
//{
//public:
//    ScaleHandle() { myMainWindow->setCursor(Qt::SizeAllCursor); }
//    ~ScaleHandle() { }
//    const char* getName()
//    {
//        return "ScaleHandle";
//    }
//    void mousePressEvent(double x, double y)
//    {
//        if (selectView != -1)
//        {
//            currentLayer->getVector()[selectView]->scale(x, y);
//        }
//    }
//    void mouseMoveEvent(double x, double y)
//    {
//        if (selectView != -1)
//            currentLayer->getVector()[selectView]->scale(x, y);
//    }
//    void mouseReleaseEvent(double x, double y)
//    {
//
//    }
//};
//
//class DeleteHandle : public Handle
//{
//public:
//    DeleteHandle()
//    {
//        if (selectView != -1)
//        {
//            currentLayer->del();
//        }
//    }
//    ~DeleteHandle() { }
//    const char* getName()
//    {
//        return "DeleteHandle";
//    }
//    void mousePressEvent(double x, double y)
//    {
//
//    }
//    void mouseMoveEvent(double x, double y)
//    {
//
//    }
//    void mouseReleaseEvent(double x, double y)
//    {
//
//    }
//};
//
//class ModifyBorderColorHandle : public Handle
//{
//public:
//    ModifyBorderColorHandle()
//    {
//        if (selectView != -1)
//        {
//            currentLayer->getVector()[selectView]->modifyBorderColor();
//        }
//    }
//    ~ModifyBorderColorHandle() { }
//    const char* getName()
//    {
//        return "ModifyBorderColorHandle";
//    }
//    void mousePressEvent(double x, double y)
//    {
//
//    }
//    void mouseMoveEvent(double x, double y)
//    {
//
//    }
//    void mouseReleaseEvent(double x, double y)
//    {
//
//    }
//};
//
//class ModifyFillColorHandle : public Handle
//{
//public:
//    ModifyFillColorHandle()
//    {
//        if (selectView != -1)
//        {
//            currentLayer->getVector()[selectView]->modifyFillColor();
//        }
//    }
//    ~ModifyFillColorHandle() {}
//    const char* getName()
//    {
//        return "ModifyFillColorHandle";
//    }
//    void mousePressEvent(double x, double y)
//    {
//
//    }
//    void mouseMoveEvent(double x, double y)
//    {
//
//    }
//    void mouseReleaseEvent(double x, double y)
//    {
//
//    }
//};
//
//class ModifyBorderWidthHandle : public Handle
//{
//public:
//    ModifyBorderWidthHandle()
//    {
//        if (selectView != -1)
//        {
//            currentLayer->getVector()[selectView]->modifyBorderWidth();
//        }
//    }
//    ~ModifyBorderWidthHandle() { }
//    const char* getName()
//    {
//        return "ModifyBorderWidthHandle";
//    }
//    void mousePressEvent(double x, double y)
//    {
//
//    }
//    void mouseMoveEvent(double x, double y)
//    {
//
//    }
//    void mouseReleaseEvent(double x, double y)
//    {
//
//    }
//};
//
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    QMenu* editMenu;

private:
    double windowScaletime;
    Point windowTranslateBegin;
    Point windowTranslateEnd;
    Point windowTranslateAccumalation;
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent*)
    {
        QPainter painter(this);
        painter.scale(windowScaletime, windowScaletime);
        painter.translate(windowTranslateAccumalation.x + windowTranslateEnd.x - windowTranslateBegin.x
            , windowTranslateAccumalation.y + windowTranslateEnd.y - windowTranslateBegin.y);
        if (currentLayer != nullptr)
        {
            for (int i = 0; i < currentLayer->getVector().size(); i++)
            {
                (currentLayer->getVector())[i]->show(painter);
            }
        }
        if (!selectSet.empty())
        {
            for (int i = 0; i < selectSet.size(); i++)
            {
                selectSet[i]->show(painter);
            }
        }
    }
   /* void mousePressEvent(QMouseEvent* event)
    {
        if (event->button() == Qt::LeftButton)
        {
            if (pt == NULL)
            {
                this->setCursor(Qt::OpenHandCursor);
                windowTranslateBegin.x = event->x();
                windowTranslateBegin.y = event->y();
            }
            else
            {
                pt->mousePressEvent(event->x(), event->y());
                repaint();
            }
        }
    }
    void mouseMoveEvent(QMouseEvent* event)
    {
        if (pt == NULL)
        {
            this->setCursor(Qt::ClosedHandCursor);
            windowTranslateEnd.x = event->x();
            windowTranslateEnd.y = event->y();
            repaint();
        }
        else
        {
            pt->mouseMoveEvent(event->x(), event->y());
            repaint();
        }
    }
    void mouseReleaseEvent(QMouseEvent* event)
    {
        if (pt == NULL)
        {
            this->setCursor(Qt::OpenHandCursor);
            windowTranslateAccumalation.x += windowTranslateEnd.x - windowTranslateBegin.x;
            windowTranslateAccumalation.y += windowTranslateEnd.y - windowTranslateBegin.y;
        }
        else
        {
            pt->mouseReleaseEvent(event->x(), event->y());
            repaint();
        }
    }*/
    void wheelEvent(QWheelEvent* event)
    {
        if (event->delta() > 0)
        {
            windowScaletime += 0.1;
        }
        else if (event->delta() < 0)
        {
            windowScaletime -= 0.1;
        }
        repaint();
    }
private slots:
    void on_action_triggered();

    void on_listWidget_itemClicked(QListWidgetItem* item);

    void on_actionBuffer_Analyze_triggered();

    void on_actionReposition_triggered();
    private slots:
        void importFile();
        void openFile();
        void saveFile();

private:
    Ui::TuXingClass* ui;
    int num_layer = 0;
};

//class SelectCommand : public Command
//{
//    Q_OBJECT
//public:
//    SelectCommand()
//    {
//
//    }
//    const char* getName()
//    {
//        return "Select Geometry";
//    }
//public slots:
//    void Invoke()
//    {
//        if (pt != nullptr)
//        {
//            delete pt;
//        }
//        pt = new SelectHandle;
//    }
//};
//
//class BoxSelectCommand : public Command
//{
//    Q_OBJECT
//public:
//    BoxSelectCommand()
//    {
//
//    }
//    const char* getName()
//    {
//        return "Box Select Geometry";
//    }
//public slots:
//    void Invoke()
//    {
//        if (pt != nullptr)
//        {
//            delete pt;
//        }
//        pt = new BoxSelectHandle;
//    }
//};
//
//class CreateCommand : public Command
//{
//    Q_OBJECT
//public:
//    CreateCommand()
//    {
//
//    }
//    const char* getName()
//    {
//        return "Create Geometry";
//    }
//public slots:
//    void Invoke()
//    {
//        if (pt != nullptr)
//        {
//            delete pt;
//        }
//        pt = new CreateHandle;
//    }
//};
//
//class EditCommand : public Command
//{
//    Q_OBJECT
//public:
//    EditCommand()
//    {
//
//    }
//    const char* getName()
//    {
//        return "Edit Geometry";
//    }
//public slots:
//    void Invoke()
//    {
//        if (pt != nullptr)
//        {
//            delete pt;
//        }
//        pt = new EditHandle;
//    }
//};
//
//class TranslateCommand : public Command
//{
//    Q_OBJECT
//public:
//    TranslateCommand()
//    {
//
//    }
//    const char* getName()
//    {
//        return "Translate Geometry";
//    }
//public slots:
//    void Invoke()
//    {
//        if (pt != nullptr)
//        {
//            delete pt;
//        }
//        pt = new TranslateHandle;
//    }
//};
//
//class RotateCommand : public Command
//{
//    Q_OBJECT
//public:
//    RotateCommand()
//    {
//
//    }
//    const char* getName()
//    {
//        return "Rotate Geometry";
//    }
//public slots:
//    void Invoke()
//    {
//        if (pt != nullptr)
//        {
//            delete pt;
//        }
//        pt = new RotateHandle;
//    }
//};
//
//class ScaleCommand : public Command
//{
//    Q_OBJECT
//public:
//    ScaleCommand()
//    {
//
//    }
//    const char* getName()
//    {
//        return "Scale Geometry";
//    }
//public slots:
//    void Invoke()
//    {
//        if (pt != nullptr)
//        {
//            delete pt;
//        }
//        pt = new ScaleHandle;
//    }
//};
//
//class DeleteCommand : public Command
//{
//    Q_OBJECT
//public:
//    DeleteCommand()
//    {
//
//    }
//    const char* getName()
//    {
//        return "Delete Geometry";
//    }
//public slots:
//    void Invoke()
//    {
//        if (pt != nullptr)
//        {
//            delete pt;
//        }
//        pt = new DeleteHandle;
//    }
//};
//
//class ModifyBorderColorCommand : public Command
//{
//    Q_OBJECT
//public:
//    ModifyBorderColorCommand()
//    {
//
//    }
//    const char* getName()
//    {
//        return "Modify Geometry Border Color";
//    }
//public slots:
//    void Invoke()
//    {
//        if (pt != nullptr)
//        {
//            delete pt;
//        }
//        pt = new ModifyBorderColorHandle;
//    }
//};
//
//class ModifyFillColorCommand : public Command
//{
//    Q_OBJECT
//public:
//    ModifyFillColorCommand()
//    {
//
//    }
//    const char* getName()
//    {
//        return "Modify Geometry Fill Color";
//    }
//public slots:
//    void Invoke()
//    {
//        if (pt != nullptr)
//        {
//            delete pt;
//        }
//        pt = new ModifyFillColorHandle;
//    }
//};
//
//class ModifyBorderWidthCommand : public Command
//{
//    Q_OBJECT
//public:
//    ModifyBorderWidthCommand()
//    {
//
//    }
//    const char* getName()
//    {
//        return "Modify Geometry Border Width";
//    }
//public slots:
//    void Invoke()
//    {
//        if (pt != nullptr)
//        {
//            delete pt;
//        }
//        pt = new ModifyBorderWidthHandle;
//    }
//};

class Shapefile
{
    std::ifstream ifs;
public:
    bool Open(std::string fname)
    {
        ifs.open(fname, std::ios::binary);
        if (!ifs)
            return false;

        ifs.seekg(0);
        if (GetBigInt() != 9994)
            return false;

        ifs.seekg(24);
        int size = GetBigInt() * 2;

        ifs.seekg(0, std::ios::end);
        int size0 = ifs.tellg();
        if (size != size0)
            return false;

        return true;
    }
    int  GetShapeType()
    {
        ifs.seekg(32);
        return GetLittleInt();
    }
    std::string GetShapeTypeString() { return ""; }
    bool GetBoundingBox(BBox& box)
    {
        ifs.seekg(36);
        box.xmin = GetDouble();
        ifs.seekg(44);
        box.ymin = GetDouble();

        ifs.seekg(52);
        box.xmax = GetDouble();
        ifs.seekg(60);
        box.ymax = GetDouble();
        box.center.x = (box.xmin + box.xmax) / 2;
        box.center.y = (box.ymin + box.ymax) / 2;
        return true;
    }

    class iterator {
        std::ifstream& ifs;
        int      off;
    public:
        iterator(std::ifstream& ifs, int off) :ifs(ifs), off(off) {};
        void operator++(int)
        {
            ifs.seekg(off + 4);
            int size = GetBigInt() * 2;
            off += 8;
            off += size;
        }
        Graphic* operator*()
        {
            ifs.seekg(off + 8);
            int type = GetLittleInt();
            Graphic* pt = NULL;
            if (type == 1)
                pt = new CPoint;
            else if (type == 3)
                pt = new CLine;
            else if (type == 5)
                pt = new CPolygon;
            ifs.seekg(off + 4);
            int size = GetBigInt() * 2;
            char* ptBuf = new char[size];
            ifs.seekg(off + 8);
            ifs.read(ptBuf, size);
            pt->Build(ptBuf, size);
            return pt;
        }
        bool operator!=(iterator oth)
        {
            return off != oth.off;
        }
    private:
        int GetBigInt()
        {
            //cc
            //return (i >> 24 & 0xff) | (i >> 8 & 0xff00) | (i << 8 & 0xff0000) | (i << 24);
            char tmp[4], x[4];
            ifs.read(tmp, sizeof(int));
            x[0] = tmp[3];
            x[1] = tmp[2];
            x[2] = tmp[1];
            x[3] = tmp[0];
            return *(int*)x;
        }
        int GetLittleInt()
        {
            int i;
            ifs.read((char*)&i, sizeof(int));
            return i;
        }
        double GetDouble()
        {
            double d;
            ifs.read((char*)&d, sizeof(double));
            return d;
        }
    };

    iterator begin()
    {
        return iterator(ifs, 100);
    }
    iterator end()
    {
        ifs.seekg(0, std::ios::end);
        int size0 = ifs.tellg();
        return iterator(ifs, size0);
    }

private:
    int GetBigInt()
    {
        char tmp[4], x[4];
        ifs.read(tmp, sizeof(int));
        x[0] = tmp[3];
        x[1] = tmp[2];
        x[2] = tmp[1];
        x[3] = tmp[0];
        return *(int*)x;
    }
    int GetLittleInt()
    {
        int i;
        ifs.read((char*)&i, sizeof(int));
        return i;
    }
    double GetDouble()
    {
        double d;
        ifs.read((char*)&d, sizeof(double));
        return d;
    }
};

ostream& operator<<(ostream& out, BBox& box);

class FileData
{
    std::fstream file;
public:
    bool getGisk(QString address, QVector<Graphic*>& array)
    {
        Graphic* ptr = NULL;
        file.open(address.toStdString());
        if (!file)
        {
            qDebug() << "The file open defeat!" << endl; return false;
        }
        qDebug() << "The file: " << address << "open successfully!" << endl;//文件打开成功，参数初始化定义

        //QString type;
        std::string typestring;
        std::string bordercolor, fillcolor;
        int borderstyle, fillstyle;
        double width = 1;
        std::string linestring;//按行读取文件
        while (std::getline(file, linestring, '\n'))
        {
            QVector<Point*> p;
            std::stringstream os;
            os << linestring;
            os >> typestring;//读取图形类别
            Point temp;
            char bit;
            os >> bit;

            if (typestring == "POINT")
            {
                os >> temp.x >> temp.y >> bit >> bordercolor >> borderstyle >> fillcolor >> fillstyle;//读取(x,y)
                p.push_back(new Point(temp.x, temp.y));
                ptr = new CPoint(p, bordercolor, borderstyle); //创建点对象
            }
            else if (typestring == "LINESTRING")
            {
                do
                {
                    os >> temp.x >> temp.y >> bit;//读取(x,y)
                    p.push_back(new Point(temp.x, temp.y));
                } while (bit == ',');
                os >> width >> bordercolor >> borderstyle;//读取(x,y)
                ptr = new CLine(p, width, bordercolor, borderstyle);
            }
            else if (typestring == "POLYGON")
            {
                os >> bit;
                do
                {
                    os >> temp.x >> temp.y >> bit;//读取(x,y)
                    p.push_back(new Point(temp.x, temp.y));
                } while (bit == ',');
                os >> bit >> width >> bordercolor >> borderstyle >> fillcolor >> fillstyle;//读取(x,y)
                ptr = new CPolygon(p, width, bordercolor, borderstyle, fillcolor, fillstyle);
            }
            else if (typestring == "RECTANGLE")
            {
                do
                {
                    os >> temp.x >> temp.y >> bit;//读取(x,y)
                    p.push_back(new Point(temp.x, temp.y));
                } while (bit == ',');
                os >> width >> bordercolor >> borderstyle >> fillcolor >> fillstyle;//读取(x,y)
                ptr = new CRectangle(p, width, bordercolor, borderstyle, fillcolor, fillstyle);
            }
            else if (typestring == "CIRCLE")
            {
                double r;
                os >> temp.x >> temp.y >> bit >> r >> width;//读取(x,y)
                p.push_back(new Point(temp.x, temp.y));
                os >> r >> width >> bordercolor >> borderstyle >> fillcolor >> fillstyle;//读取(x,y)
                ptr = new CCircle(p, r, width, bordercolor, borderstyle, fillcolor, fillstyle);
            }
            else if (typestring == "SECTOR")
            {
                double r;
                do
                {
                    os >> temp.x >> temp.y >> bit;//读取(x,y)
                    p.push_back(new Point(temp.x, temp.y));
                } while (bit == ',');
                os >> r >> width >> bordercolor >> borderstyle >> fillcolor >> fillstyle;//读取(x,y)
                ptr = new CSector(p, r, width, bordercolor, borderstyle, fillcolor, fillstyle);
            }
            array.push_back(ptr);
            ptr = NULL;
        }
        file.close();
        delete ptr;
        return true;
    }

    bool getShp(QString fname, QVector<Graphic*>& array)
    {
        //将shp格式转换为自定义格式
        Shapefile shp;
        if (!shp.Open(fname.toStdString()))
        {
            return false;
        }
        BBox box;
        shp.GetBoundingBox(box);

        for (Shapefile::iterator it = shp.begin(); it != shp.end(); it++)
        {
            array.push_back(*it);
        }
        return true;
    }

    bool saveData(QString address, QVector<Graphic*> array)
    {
        QFile file(address);
        QTextStream out(&file);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return false;
        }

        for (int i = 0; i < array.size(); i++)
        {
            //存储图形类别
            out << array[i]->gisk() << '\n';
        }
        file.close();
        return true;
    }
};

//void getview(QVector<Graphic*>& geom, int i);

#endif // MAINWINDOW_H
