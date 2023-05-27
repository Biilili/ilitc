#include "GISLite.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GISLite w;
    w.show();
    return a.exec();
}
