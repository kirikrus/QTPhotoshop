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
    this->setStyleSheet("background-color: rgba(153, 50, 204, 100);");
}

void MYDropWidget::dragLeaveEvent(QDragLeaveEvent* event) {
    this->setStyleSheet("background: none;");
}

void MYDropWidget::dropEvent(QDropEvent* event) {
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urlList = mimeData->urls();
        for (const QUrl& url : urlList) {
            QString filePath = url.toLocalFile();
            QString fileExtension = QFileInfo(filePath).suffix().toLower();
            if (fileExtension != "jpg" && fileExtension != "jpeg" && fileExtension != "png" && fileExtension != "bmp" && fileExtension != "webp") {
                QMessageBox::critical(this, ("Oooopsy"), QString::fromUtf8("So cringe... Your trash isn't valid: JPG, JPEG, PNG, BMP."));
                return;
            }
            this->layer.push_back(layerIMG(QImage(filePath)));
        }
        event->acceptProposedAction();
        displayImages();
    }
}
