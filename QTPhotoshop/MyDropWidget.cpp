#include "mydropwidget.h"

MYDropWidget::MYDropWidget(QWidget* parent)
    : QFrame(parent) {
    setAcceptDrops(true);
}

void MYDropWidget::displayImages() {
    this->setStyleSheet("");
    emit imgLoad();
}

QGraphicsScene* MYDropWidget::scene() {return &m_scene;}

void MYDropWidget::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
    this->setStyleSheet("background-color: rgba(170, 255, 127, 100);");
}

void MYDropWidget::dragLeaveEvent(QDragLeaveEvent* event) {
    this->setStyleSheet("");
}

void MYDropWidget::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        for (const QUrl& url : urlList)
            this->layer.push_back(layerIMG(QImage(url.toLocalFile())));
        event->acceptProposedAction();
        displayImages();
    }
}
