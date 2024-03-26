#include "QTPhotoshop.h"

QTPhotoshop::QTPhotoshop(QWidget *parent) : QMainWindow(parent){
    ui.setupUi(this);
    connect(ui.frame_2, &MYDropWidget::imgLoad, this, &QTPhotoshop::update);
    connect(ui.scalerInc, &QPushButton::clicked, this, &QTPhotoshop::update);
    connect(ui.scalerDec, &QPushButton::clicked, this, &QTPhotoshop::update);
}

QTPhotoshop::~QTPhotoshop(){}

void QTPhotoshop::update() {
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
        if (clickedButton == ui.scalerInc) {
            scale(&ui,.1);
        } else if (clickedButton == ui.scalerDec) {
            scale(&ui, -.1);
        }
        else{
            display(&ui);
            displayProp(&ui, ui.frame_2->layer.size()-1);
        }
}