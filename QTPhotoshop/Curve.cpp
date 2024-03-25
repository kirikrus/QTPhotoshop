#include "curve.h"
#include <QPainter>
#include <QPointF>
#include <QVector>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QApplication>

CurveItem::CurveItem(QVector<QPointF>* inp_points,QGraphicsItem* parent) 
    : QGraphicsItem(parent), selectedIndex(-1), controlPoints(inp_points) {}

QRectF CurveItem::boundingRect() const{
    if (controlPoints->isEmpty())
        return QRectF();
    else
        return QRectF(controlPoints->first(), controlPoints->last());
}

void CurveItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget){
    painter->setPen(QPen(QColor(153, 50, 204), 2));
    drawCurve(painter);
    qreal pointSize = 10;
    for (const QPointF& point : *controlPoints) {
        qreal x = point.x();
        qreal y = point.y();
        painter->drawEllipse(QRectF(x - pointSize / 2, y - pointSize / 2, pointSize, pointSize));
    }
}

void CurveItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    QPointF clickPos = event->pos();
    if (event->button() == Qt::RightButton) {
        for (int i = 0; i < controlPoints->size(); ++i) {
            QPointF diff = clickPos - (*controlPoints)[i];
            if (diff.manhattanLength() < 10) {
                controlPoints->remove(i);
                update();
                return;
            }
        }
    }
    else {
        for (int i = 1; i < controlPoints->size() - 1; ++i) {
            QPointF diff = clickPos - (*controlPoints)[i];
            if (diff.manhattanLength() < 10) { // оадиус активации точки
                selectedIndex = i;
                QCursor cursor(Qt::ClosedHandCursor);
                QApplication::setOverrideCursor(cursor);
                return;
            }
        }
        controlPoints->insert(controlPoints->size() - 1, clickPos);
        selectedIndex = controlPoints->size() - 2;
        QCursor cursor(Qt::ClosedHandCursor);
        QApplication::setOverrideCursor(cursor);
        update();
    }
}

void CurveItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (selectedIndex >= 0) {
        QPointF newPos = event->pos();
        qreal newX = qBound(0., newPos.x(), 175.);
        qreal newY = qBound(0., newPos.y(), 175.);
        (*controlPoints)[selectedIndex] = QPointF(newX, newY);
        update();
    }
}

void CurveItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    selectedIndex = -1;
    QCursor cursor(Qt::ArrowCursor);
    QApplication::setOverrideCursor(cursor);
    emit curveChanged();
}

void CurveItem::drawCurve(QPainter* painter) {//кубический сплайн безье
    if (controlPoints->size() < 2) return;

    QPainterPath path;
    path.moveTo((*controlPoints)[0]);

    for (int i = 0; i < controlPoints->size() - 1; ++i) {
        QPointF p0 = (i == 0) ? (*controlPoints)[0] : (*controlPoints)[i - 1];
        QPointF p1 = (*controlPoints)[i];
        QPointF p2 = (*controlPoints)[i + 1];
        QPointF p3 = (i == controlPoints->size() - 2) ? (*controlPoints)[i + 1] : (*controlPoints)[i + 2];

        QPointF controlPoint1 = p1 + (p2 - p0) / 6.0;
        QPointF controlPoint2 = p2 - (p3 - p1) / 6.0;

        controlPoint1 = QPointF(qBound(0., controlPoint1.x(), 175.), qBound(0., controlPoint1.y(), 175.));
        controlPoint2 = QPointF(qBound(0., controlPoint2.x(), 175.), qBound(0., controlPoint2.y(), 175.));

        path.cubicTo(controlPoint1, controlPoint2, p2);
    }

    painter->drawPath(path);
}
