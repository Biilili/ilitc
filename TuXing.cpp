#include "tuxing.h"
#include <QDebug>
//////#include<sql.h>
//////#include <sqltypes.h>
//////#include <sqlext.h>

QVector<Graphic*> g;
QVector<Layer*> LayerList;
View* view = nullptr;
Layer* currentLayer = nullptr;
int selectView = -1;
QVector<int> selectViewSet;
QVector<View*> selectSet;
QWidget* myMainWindow;
int num_layer = 0;
Handle* ppt;
Handle* pt;

double distance(double x,double y,double x0,double y0){
    {
        return sqrt((x - x0) * (x - x0) + (y - y0) * (y-y0)) ;
    }
}
ostream& operator<<(ostream& out, BBox& box)
{
    out << "(" << box.xmin << ",";
    out << box.xmax << ",";
    out << box.ymin << ",";
    out << box.ymax << ")";
    return out;
}
MainWindow::MainWindow(QWidget* parent):QMainWindow(parent),ui(new Ui::TuXingClass)
{
   ui->setupUi(this);
   myMainWindow= this;
   windowScaletime = 1;

   // 创建菜单栏对象
   QMenuBar* menuBar = new QMenuBar(this);

   // 创建菜单项A，并添加三个子菜单项
   QMenu* menuA = new QMenu("File");
   menuA->addAction("New");
   menuA->addAction("Open");
   menuA->addAction("Import");
   menuA->addAction("Save");
   menuA->addAction("Save as");
   menuBar->addMenu(menuA);

   // 创建菜单项B，并添加三个子菜单项
   QMenu* menuB = new QMenu("Edit");
   menuB->addAction("Start Editing");
   menuB->addAction("End and Exit");
   menuBar->addMenu(menuB);

   // 设置菜单栏为窗体的菜单栏
   setMenuBar(menuBar);

   // 创建工具栏对象
   QToolBar* toolbar = new QToolBar(this);

   // 添加工具栏图标按钮
   QAction* action1 = toolbar->addAction(QIcon(":/icons/icon1.png"), "Button 1");
   QAction* action2 = toolbar->addAction(QIcon(":/icons/icon2.png"), "Button 2");
   QAction* action3 = toolbar->addAction(QIcon(":/icons/icon3.png"), "Button 3");

   // 将工具栏添加到窗体中
   addToolBar(toolbar);
   //连接菜单项的触发信号与 openFile() 槽函数
   connect(menuA->actions().at(1), &QAction::triggered, this, &MainWindow::openFile);
   connect(menuA->actions().at(2), &QAction::triggered, this, &MainWindow::importFile);
   connect(menuA->actions().at(3), &QAction::triggered, this, &MainWindow::saveFile);
  connect(menuA->actions().at(4), &QAction::triggered, this, &MainWindow::saveSQL);
  // ConnectionSQL();
   //qDebug() << QSqlDatabase::drivers();
   
}
MainWindow::~MainWindow()
{
    //delete ui;
}
void MainWindow::getView(QVector<Graphic*> geom, int j)
{
    if (!strcmp(geom[j]->getname(), "POINT"))
        view = new PointView(*(CPoint*)geom[j]);
    else if (!strcmp(geom[j]->getname(), "linestring"))
        view = new LineView(*((CLine*)geom[j]));
    else if (!strcmp(geom[j]->getname(), "polygon"))
        view = new PolygonView(*((CPolygon*)geom[j]));
    else if (!strcmp(geom[j]->getname(), "rectangle"))
        view = new RectangleView(*((CRectangle*)geom[j]));
    else if (!strcmp(geom[j]->getname(), "circle"))
        view = new CircleView(*((CCircle*)geom[j]));
    else if (!strcmp(geom[j]->getname(), "sector"))
        view = new SectorView(*((CSector*)geom[j]));
}


void MainWindow::on_action_triggered()
{
    QStringList filename = QFileDialog::getOpenFileNames(this, "打开文件", "../Code2/", "Text Files(*.txt);;SHP Files(*.shp);;");
    //QString qstr = "D:/中国地图ARCGIS/中国地图ARCGIS/geometry_data.txt";
    //QStringList filename(qstr);

    FileData F;

    for (int j = 0; j < filename.size(); j++)
    {
        QVector<Graphic*> geom;
        Layer* pl;

        pl = new Layer(filename[j]);
        LayerList.push_back(pl);

        if (filename.back().right(3) == "txt")
           F.getTxt(filename[j].toStdString().c_str(), geom);
        else if (filename.back().right(3) == "shp")
           F.getShp(filename[j].toStdString().c_str(), geom);

        for (int i = 0; i < geom.size(); i++)
        {
            qDebug() << "The geometry" << geom[i]->getname() << "read finished" << endl;
            //getview(geom, i);
            LayerList[num_layer]->getVector().push_back(view);
        }
        QListWidgetItem* item = new QListWidgetItem(QString::fromStdString("Layer" + std::to_string(num_layer)));
       // ui->listWidget->addItem(item);
        currentLayer = LayerList[num_layer];
        num_layer++;
    }
    repaint();

}
void MainWindow::on_listWidget_itemClicked(QListWidgetItem* item)
{
    selectView = -1;
    selectSet.clear();
    currentLayer = LayerList[((QString)(item->text().back())).toInt()];
    repaint();
}
void MainWindow::on_actionBuffer_Analyze_triggered()
{
}
void MainWindow::on_actionReposition_triggered()
{
    windowTranslateAccumalation.x = 0;
    windowTranslateAccumalation.y = 0;
    windowTranslateBegin.x = 0;
    windowTranslateBegin.y = 0;
    windowTranslateEnd.x = 0;
    windowTranslateEnd.y = 0;
    windowScaletime = 1;
    repaint();
}
void MainWindow::openFile()
{
    QString filter = tr("GISLite File (*.gisk);");
    QStringList FileName = QFileDialog::getOpenFileNames(this, tr("Open File"), "", filter);
    if (FileName.isEmpty())
    {
        return;// 在此处添加将选择的文件导入项目文件中的代码
    }
    FileData F;
    for (int i = 0; i < FileName.size(); i++)
    {
      
        Layer* player;

        player = new Layer(FileName[i]);
        LayerList.push_back(player);

        if (FileName[i].right(4) == "gisk")
        {
            F.getGisk(FileName[i], g);
        }       
        for (int i = 0; i < g.size(); i++)
        {
            qDebug() << "The geometry" << g[i]->getname() << "read finished" << endl;
            getView(g, i);
            LayerList.last()->getVector().push_back(view);
        }
    }
    repaint();
}
void MainWindow::importFile()
{try{ QString filter = tr("Text Files (*.txt);;ShapeFile (*.shp);;All Files (*)");
    QStringList FileName = QFileDialog::getOpenFileNames(this, tr("Open File"), "", filter);
    if (FileName.isEmpty())
    {
        return;// 在此处添加将选择的文件导入项目文件中的代码
    }
    FileData F;
    for (int i = 0; i < FileName.size(); i++)
    {
        //QVector<Graphic*> g;
        Layer* player;
        player = new Layer(FileName[i]);
        LayerList.push_back(player);
        if (FileName[i].right(3) == "txt")
        {
            F.getTxt(FileName[i], g);
        }
        else if (FileName[i].right(3) == "shp")
        {
            F.getShp(FileName[i],g);
        }
        for (int i = 0; i < g.size(); i++)
        {
            qDebug() << "The geometry" << g[i]->getname() << "read finished" << endl;
           getView(g, i);
            LayerList.last()->getVector().push_back(view);
        }
    }
    repaint();}
catch(std::exception& e)
{
    qDebug() << "Exception caught:";
}
}
void MainWindow::saveFile()
{
    QString savePath = QFileDialog::getSaveFileName(nullptr, "Save File", "", "GISLite File (*.gisk)");

    if (savePath.isEmpty())
    {
		return;
	}
    else
    {   
        FileData F;
        F.saveData(savePath, g);

	}
}
void MainWindow::saveSQL()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "save", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QString address; FileData F;
        // 用户选择了“是”按钮，执行相关操作
       F.Databasesave(address, g);
    }
    else {
        // 用户选择了“否”按钮或关闭了对话框，不执行任何操作
    }

}
void MainWindow::weizhi(QListWidgetItem* item)
{
    selectView = -1;
    selectSet.clear();
    currentLayer = LayerList[((QString)(item->text().back())).toInt()];
    repaint();
}
void getview(QVector<Graphic*>& geom, int i)
{
    //if (geom[i]->getname() == "Point")
    if (!strcmp(geom[i]->getname(), "Point"))
        view = new PointView(*((CPoint*)geom[i]));
    else if (!strcmp(geom[i]->getname(), "Line"))
        view = new LineView(*((CLine*)geom[i]));
    else if (!strcmp(geom[i]->getname(), "Polygon"))
        view = new PolygonView(*((CPolygon*)geom[i]));
    else if (!strcmp(geom[i]->getname(), "Rectangle"))
        view = new RectangleView(*((CRectangle*)geom[i]));
    else if (!strcmp(geom[i]->getname(), "Circle"))
        view = new CircleView(*((CCircle*)geom[i]));
    else if (!strcmp(geom[i]->getname(), "Sector"))
        view = new SectorView(*((CSector*)geom[i]));
}