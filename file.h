#pragma once
#include <fstream>
#include <string>
#include <QDebug>
#include <iostream>

#include "basic.h"
#include "geometry.h"

class FileData
{
    std::fstream file;
public:
    bool getTxt(QString address, QVector<Geometry*>& array)
    {
        Geometry* ptr = NULL;
        file.open(address.toStdString());
        if (!file)
        {
            qDebug() << "The file open defeat!" << endl; return false;
        }
        qDebug() << "The file: " << address << "open successfully!" << endl;//�ļ��򿪳ɹ���������ʼ������

        //QString type;
        std::string typestring;
        int num;
        int R1 = 0, G1 = 0, B1 = 0, R2 = 0, G2 = 0, B2 = 0;
        double width = 1;
        std::string linestring;
        //���ж�ȡ�ļ�
        while (std::getline(file, linestring, '\n'))
        {
            QVector<Point*> p;
            std::stringstream os;
            os << linestring;
            os >> typestring;//��ȡͼ�����
            Point temp;
            char bit;
            os >> bit;
            do
            {
                os >> temp.x >> temp.y >> bit;//��ȡ(x,y)
                p.push_back(new Point(temp.x, temp.y));
            } while (bit == ',');

            //os >> R1 >> G1 >> B1 >> width >> R2 >> G2 >> B2;
            //����ͼ����𴴽�ͼ�ζ���
            if (typestring == "Point") { ptr = new CPoint(p, R1, G1, B1, width, R2, G2, B2); }
            else if (typestring == "Line") { ptr = new CLine(p, R1, G1, B1, width, R2, G2, B2); }
            else if (typestring == "Polygon") { ptr = new CPolygon(p, R1, G1, B1, width, R2, G2, B2); }
            else if (typestring == "Rectangle") { ptr = new CRectangle(p, R1, G1, B1, width, R2, G2, B2); }
            else if (typestring == "Circle") { ptr = new CCircle(p, R1, G1, B1, width, R2, G2, B2); }
            //else if (type == "Sector") { ptr = new CSector(p, R1, G1, B1, width, R2, G2, B2); }
            array.push_back(ptr);
            ptr = NULL;
        }
        file.close();
        delete ptr;
        return true;
    }


};