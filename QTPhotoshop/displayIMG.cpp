#include "displayIMG.h"

void display(Ui::QTPhotoshopClass* ui) {
    QGraphicsScene* scene = new QGraphicsScene(ui->frame_2);
    for (int i{ 0 };i < ui->frame_2->img.size();i++) {
        QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(ui->frame_2->img[i]));
        pixmapItem->setZValue(i);
        scene->addItem(pixmapItem);
    }

    ui->photo_zone->setScene(scene);
}

void scale(Ui::QTPhotoshopClass* ui, float inc) {
    qreal currentScale = ui->photo_zone->transform().m11();
    qreal newScale = currentScale + inc;
    ui->photo_zone->setTransform(QTransform().scale(newScale, newScale));
}