#include "dialog.h"
#include <QApplication>
#include <thread>
#include <QTimer>
#include <QObject>
#include <QDebug>



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;

    w.show();

    return a.exec();
}
