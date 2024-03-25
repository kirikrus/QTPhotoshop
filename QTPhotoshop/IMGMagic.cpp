#include "IMGmagic.h"
#include "displayIMG.h"

void mode(Ui::QTPhotoshopClass*, int);

void editIMGProp(Ui::QTPhotoshopClass* ui, int index, int value, QString senderName) {
    if (senderName == "offsetX") ui->frame_2->layer[index].offsetX = value;
    else if (senderName == "offsetY") ui->frame_2->layer[index].offsetY = value;
    if (senderName == "height") {
        ui->frame_2->layer[index].h = value;
        ui->frame_2->layer[index].img = ui->frame_2->layer[index].img_save.scaled(ui->frame_2->layer[index].w, value);
    }
    else if (senderName == "width") {
        ui->frame_2->layer[index].w = value;
        ui->frame_2->layer[index].img = ui->frame_2->layer[index].img_save.scaled(value, ui->frame_2->layer[index].h);
    }
    else if (senderName == "alpha") {//ПРОБЛЕМА, КАКАЯ ХЗ, НО ПРОБЛЕМА
        ui->frame_2->layer[index].alpha = value;
        ui->frame_2->layer[index].setAlpha();
    }
    mode(ui);
    display(ui);
}

struct RGBA {
    float r, g, b, a, mid;

    RGBA();
    RGBA(float r, float g, float b, float a):r(r), g(g), b(b), a(a) {
        mid = (r + g + b) / 3.;
    }
    RGBA(QRgb inp):r(qRed(inp)), g(qGreen(inp)), b(qBlue(inp)), a(qAlpha(inp)) {
        mid = (r + g + b) / 3.;
    }

    void normalize() {
        r /= 255.;
        g /= 255.;
        b /= 255.;
        a /= 255.;
    }

    RGBA operator+ (RGBA inp) { 
        return RGBA(r + inp.r > 1?1: r + inp.r, g + inp.g > 1 ? 1 : g + inp.g, b + inp.b > 1 ? 1 : b + inp.b, a);
    }
    RGBA operator- (RGBA inp) { 
        return RGBA(r - inp.r < 0 ? 0 : r - inp.r, g - inp.g < 0 ? 0 : g - inp.g, b - inp.b < 0 ? 0 : b - inp.b, a);
    }
    friend RGBA operator- ( int i, RGBA obj) {return RGBA(i - obj.r, i - obj.g, i - obj.b, obj.a);}
    RGBA operator* (RGBA inp) { return RGBA(r * inp.r, g * inp.g, b * inp.b,a); }
    RGBA operator/ (RGBA inp) { return RGBA(r / inp.r, g / inp.g, b / inp.b, a); }
};

void mode(Ui::QTPhotoshopClass* ui) {
    for (int index{ 0 }; index <= ui->frame_2->layer.size() - 1; index++) {
        ui->frame_2->layer[index].img = ui->frame_2->layer[index].img_save;
        ui->frame_2->layer[index].img = ui->frame_2->layer[index].img_save.scaled(ui->frame_2->layer[index].w, ui->frame_2->layer[index].h);
        ui->frame_2->layer[index].setAlpha();

        if (ui->frame_2->layer[index].mode == Normal || index == 0) {
            if (ui->frame_2->layer[index].mode != Normal)
                ui->frame_2->layer[index].mode = Normal;
            continue;
        }

        layerIMG* layer_up = &ui->frame_2->layer[index];
        layerIMG* layer_down = &ui->frame_2->layer[index - 1];

        int minW = qMin(layer_up->img.width(), layer_down->img.width());
        int minH = qMin(layer_up->img.height(), layer_down->img.height());

        int offsetX = layer_up->offsetX - layer_down->offsetX;
        int offsetY = layer_up->offsetY - layer_down->offsetY;

        for (int y = 0; y < minH - offsetY; ++y) {
            for (int x = 0; x < minW - offsetX; ++x) {
                int targetX = x + offsetX; // Координата X с учетом смещения
                int targetY = y + offsetY; // Координата Y с учетом смещения

                if (targetX >= 0 && targetX < layer_down->img.width() &&
                    targetY >= 0 && targetY < layer_down->img.height()) {

                    RGBA rgba_up = layer_up->img.pixel(x, y);
                    RGBA rgba_down = layer_down->img.pixel(targetX, targetY);

                    rgba_up.normalize();
                    rgba_down.normalize();

                    switch (layer_up->mode) {//https://en.wikipedia.org/wiki/Blend_modes
                    case Darken://затеимнение
                        rgba_up = rgba_down.mid < rgba_up.mid ? rgba_up : rgba_down;
                        break;
                    case Multiply://умножение
                        rgba_up = rgba_up * rgba_down;
                        break;
                    case Exclusion://исключение
                        rgba_up = (rgba_up + rgba_down) - (rgba_up * rgba_down * 2.);
                        break;
                    case Subtract://вычитание
                        rgba_up = rgba_up - rgba_down;;
                        break;
                    case Divide://разделение
                        rgba_up = rgba_up / rgba_down;
                        break;
                    case Screen://экран
                        rgba_up = 1 - (1 - rgba_up) * (1 - rgba_down);
                        break;
                    case Invert://инверсия
                        rgba_up = 1 - rgba_up;
                        break;
                    case Addition://дополнение
                        rgba_up = rgba_up + rgba_down;
                        break;
                    }

                    QRgb out_pixel_up = qRgba(rgba_up.r * 255., rgba_up.g * 255., rgba_up.b * 255., rgba_up.a * 255.);
                    layer_up->img.setPixel(x, y, out_pixel_up);
                }
            }
        }
    }
}

void histo_build(QImage* img ,QCustomPlot* histo) {
    QVector<double> brightValues(256, 0);

    for (int y = 0; y < img->height(); ++y)
        for (int x = 0; x < img->width(); ++x) {
            QColor color(img->pixel(x, y));
            int mid = (color.red() + color.blue() + color.green()) / 3.;
            brightValues[mid]++;
        }
    int max1{ 0 }, max2{ 0 };
    for (auto& elem : brightValues) max1 = max1 < elem ? elem : max1;
    for (auto& elem : brightValues) max2 = (max2 < elem && elem != max1) ? elem : max2;

    histo->clearPlottables();

    QCPBars* histogram = new QCPBars(histo->xAxis, histo->yAxis);

    QVector<double> xAxis(256);
    for (int i = 0; i < 256; ++i) xAxis[i] = i;

    histo->xAxis->setTickLabels(false);
    histo->yAxis->setTickLabels(false);
    histo->xAxis->setRange(0, 255);
    histo->yAxis->setRange(0, max2);

    histo->setBackground(QColor(60, 60, 60));
    histogram->setBrush(QColor(153, 50, 204));
    histogram->setPen(QColor(153, 50, 204));
    histo->xAxis->setVisible(false);
    histo->yAxis->setVisible(false);
    histo->axisRect()->setAutoMargins(QCP::msNone);
    histo->axisRect()->setMargins(QMargins(0, 0, 0, 0));

    histogram->setData(xAxis,brightValues);
    histo->replot();
}

double interpolateCurve(QPointF point, QVector<QPointF> controlPoints) {
    if (controlPoints.size() < 2) return 0.0;

    // ищем  ближайшие точки на кривой к заданной точке
    QPointF p1, p2;
    int index = 0;
    while (index < controlPoints.size() - 1 && point.x() >controlPoints[index + 1].x())
        ++index;

    p1 = controlPoints[index];
    p2 = controlPoints[index + 1];

    //интерполируем значение y на кривой в заданной точке
    double t = (point.x() - p1.x()) / (p2.x() - p1.x());
    double y = (1 - t) * p1.y() + t * p2.y();

    return y / 175.;
}


void applyContrastCurve(Ui::QTPhotoshopClass* ui, int index) {
    QVector<QPointF> controlPoints = ui->frame_2->layer[index].controlPoints;

    for (int y = 0; y < ui->frame_2->layer[index].img.height(); ++y)
        for (int x = 0; x < ui->frame_2->layer[index].img.width(); ++x) {
            QColor color(ui->frame_2->layer[index].img_save.pixel(x, y));

            double originalRed = color.redF();
            double originalGreen = color.greenF();
            double originalBlue = color.blueF();

            double newRed = interpolateCurve(QPointF(175. * originalRed, 175. * originalRed), controlPoints);
            double newGreen = interpolateCurve(QPointF(175. * originalGreen, 175. * originalGreen), controlPoints);
            double newBlue = interpolateCurve(QPointF(175. * originalBlue, 175. * originalBlue), controlPoints);

            color.setRgbF(newRed, newGreen, newBlue);
            ui->frame_2->layer[index].img.setPixel(x, y, color.rgb());
        }
}
