#include "paintthreed.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PaintThreeD w;
    w.show();

    return a.exec();
}
