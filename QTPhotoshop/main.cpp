#include "QTPhotoshop.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]){
    QApplication a(argc, argv);
    QTPhotoshop w;
    w.show();
    return a.exec();
}
