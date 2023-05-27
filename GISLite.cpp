#include "GISLite.h"

GISLite::GISLite(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
}

GISLite::~GISLite()
{}
GISLite::GISLite(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

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
    connect(menuA->actions().at(1), &QAction::triggered, this, &GISLite::openFile);
    connect(menuA->actions().at(2), &QAction::triggered, this, &GISLite::importFile);
}

GISLite::~GISLite()
{}

void GISLite::openFile()
{
    QString filter = tr("GISLite Files (*.gisk)");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", filter);
    if (fileName.isEmpty())
    {
        return;// 在此处添加将选择的文件导入项目文件中的代码
    }
}

void GISLite::importFile()
{
    QString filter = tr("Text Files (*.txt);;ShapeFile (*.shp);;All Files (*)");
    QStringList FileName = QFileDialog::getOpenFileNames(this, tr("Open File"), "", filter);
    if (FileName.isEmpty())
    {
        return;// 在此处添加将选择的文件导入项目文件中的代码
    }
    FileData F;
    for (int i = 0; i < FileName.size(); i++)
    {
        QVector<Geometry*> g;
        Layer* player;

        player = new Layer(FileName[i]);
        LayerList.push_back(player);

        if (FileName[i].right(3) == "txt")
        {
            F.getTxt(FileName[i], g);
        }
        else if (FileName[i].right(3) == "shp")
        {
            //F.getShp(FileName[i],g);
        }
        for (int j = 0; j < g.size(); j++)
        {
            //getView(g,i);
            LayerList[num_layer];
            num_layer++;
        }
    }
    repaint();
}

