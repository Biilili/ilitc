#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_GISLite.h"

#include <QMainWindow>
#include <QMenuBar>
#include <QAction>
#include <QFile>
#include <QFileDialog>
#include <QVector>
#include <sstream>
#include <QString>

#include "basic.h"
#include "geometry.h"
#include "file.h"


#define PI 3.1416

namespace Ui { class MainWindow; }

class GISLite : public QMainWindow
{
    Q_OBJECT

public:
    GISLite(QWidget* parent = nullptr);
    ~GISLite();

private:
    Ui::GISLiteClass ui;

private slots:
    void importFile();
    void openFile();

public:
    int num_layer = 0;
    QVector<Layer*> LayerList;
};
