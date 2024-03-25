#include "QTPhotoshop.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[]){
    QApplication a(argc, argv);
    QTPhotoshop w;
    w.show();
    //w.ui.scrollAreaWidgetContents_2->hide();
    w.ui.miniPanelIMG->hide();
    return a.exec();
}
