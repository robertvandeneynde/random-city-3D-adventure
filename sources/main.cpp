#include <ctime>
#include <QApplication>
#include <QDebug>
#include <algorithm>
#include "MainWindow.h"
#include "ListeChainee.h"
#include "Element.h"
#include "Tas.h"
#include <QColorDialog>

void test()
{

}

int main(int argc, char *argv[])
{
    test();
    srand(time(0));
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
