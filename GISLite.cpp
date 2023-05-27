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

    // �����˵�������
    QMenuBar* menuBar = new QMenuBar(this);

    // �����˵���A������������Ӳ˵���
    QMenu* menuA = new QMenu("File");
    menuA->addAction("New");
    menuA->addAction("Open");
    menuA->addAction("Import");
    menuA->addAction("Save");
    menuA->addAction("Save as");
    menuBar->addMenu(menuA);

    // �����˵���B������������Ӳ˵���
    QMenu* menuB = new QMenu("Edit");
    menuB->addAction("Start Editing");
    menuB->addAction("End and Exit");
    menuBar->addMenu(menuB);

    // ���ò˵���Ϊ����Ĳ˵���
    setMenuBar(menuBar);

    // ��������������
    QToolBar* toolbar = new QToolBar(this);

    // ��ӹ�����ͼ�갴ť
    QAction* action1 = toolbar->addAction(QIcon(":/icons/icon1.png"), "Button 1");
    QAction* action2 = toolbar->addAction(QIcon(":/icons/icon2.png"), "Button 2");
    QAction* action3 = toolbar->addAction(QIcon(":/icons/icon3.png"), "Button 3");

    // ����������ӵ�������
    addToolBar(toolbar);
    //���Ӳ˵���Ĵ����ź��� openFile() �ۺ���
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
        return;// �ڴ˴���ӽ�ѡ����ļ�������Ŀ�ļ��еĴ���
    }
}

void GISLite::importFile()
{
    QString filter = tr("Text Files (*.txt);;ShapeFile (*.shp);;All Files (*)");
    QStringList FileName = QFileDialog::getOpenFileNames(this, tr("Open File"), "", filter);
    if (FileName.isEmpty())
    {
        return;// �ڴ˴���ӽ�ѡ����ļ�������Ŀ�ļ��еĴ���
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

