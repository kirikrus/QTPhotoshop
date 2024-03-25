#include <QGraphicsItem>

class CurveItem : public QObject, public QGraphicsItem {
    Q_OBJECT
public:
    CurveItem(QVector<QPointF>*,QGraphicsItem* parent = nullptr);
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* ) ;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* ) ;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* ) ;

private:
    QVector<QPointF>* controlPoints;
    int selectedIndex;

    void drawCurve(QPainter*);

signals:
    void curveChanged();
};
