#include "QTPhotoshop.h"
#include <QFrame>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class MYDropWidget : public QFrame {
public:
    QVector<QImage> img;

    MYDropWidget(QWidget*);
    void displayImages();

protected:
    void dragEnterEvent(QDragEnterEvent*);
    void dragOutEvent(QDragLeaveEvent*);
    void dropEvent(QDropEvent*);
};
