#include <QFrame>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>

class MYDropWidget : public QFrame {
    Q_OBJECT
signals:
    void imgLoad();

private:
    QGraphicsScene m_scene;

public:
    QVector<QImage> img;

    MYDropWidget(QWidget* parent = nullptr);
    void displayImages();
    QGraphicsScene* scene();

protected:
    void dragLeaveEvent(QDragLeaveEvent*);
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);
};