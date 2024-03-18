#include "mydropwidget.h"

MYDropWidget::MYDropWidget(QWidget* parent = nullptr)
    : QFrame(parent) {
    setAcceptDrops(true);
}

void MYDropWidget::displayImages() {
    QGraphicsScene* scene = new QGraphicsScene(QTPhotoshop().ui.photo_zone);
    for (const QImage& image : img)
        scene->addItem(new QGraphicsPixmapItem(QPixmap::fromImage(image)));
    QTPhotoshop().ui.photo_zone->setScene(scene);
}

void MYDropWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
    this->setStyleSheet("background-color: rgba(170, 255, 127, 100);");
}

void MYDropWidget::dragOutEvent(QDragLeaveEvent* event) {
    this->setStyleSheet("");
}

void MYDropWidget::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        for (const QUrl& url : urlList)
            this->img.push_back(QImage(url.toLocalFile()));
        event->acceptProposedAction();
        displayImages();
    }
}