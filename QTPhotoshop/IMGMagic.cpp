#include "IMGmagic.h"
#include "displayIMG.h"
#include <execution>

void mode(Ui::QTPhotoshopClass*);

void editIMGProp(Ui::QTPhotoshopClass* ui, int index, int value, QString senderName) {
    if (senderName == "offsetX") ui->frame_2->layer[index].offsetX = value;
    else if (senderName == "offsetY") ui->frame_2->layer[index].offsetY = value;
    if (senderName == "height") {
        ui->frame_2->layer[index].h = value;
    }
    else if (senderName == "width") {
        ui->frame_2->layer[index].w = value;
    }
    else if (senderName == "alpha") {
        ui->frame_2->layer[index].alpha = value;
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

    void to_Grey() {
        r = g = b = 0.2125 * r + 0.7154 * g + 0.0721 * b;
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
    bool operator<= (int inp) { return mid <= inp; }
};

void mode(Ui::QTPhotoshopClass* ui) {
    for (int index{ 0 }; index <= ui->frame_2->layer.size() - 1; index++) {
        QImage img_copy = ui->frame_2->layer[index].img_save.copy();
        ui->frame_2->layer[index].img = img_copy.scaled(ui->frame_2->layer[index].w, ui->frame_2->layer[index].h);

        ui->frame_2->layer[index].setAlpha();

        if (ui->frame_2->layer[index].mode == Normal || index == 0) {
            if (ui->frame_2->layer[index].mode != Normal)
                ui->frame_2->layer[index].mode = Normal;
            ui->frame_2->layer[index].img = ui->frame_2->layer[index].img_save.scaled(ui->frame_2->layer[index].w, ui->frame_2->layer[index].h).copy();
            ui->frame_2->layer[index].img_mix_save = ui->frame_2->layer[index].img.copy();
            if(ui->frame_2->layer[index].controlPoints.size() > 2)
                applyContrastCurve(ui,index);
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
                int targetX = x + offsetX; //  оордината X с учетом смещени€
                int targetY = y + offsetY; //  оордината Y с учетом смещени€

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
                    case Invert://инверси€
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
        layer_up->img_mix_save = layer_up->img;
    }
}

void histo_build(QImage* img ,QCustomPlot* histo) {
    QVector<double> brightValues(256, 0);

    for (int y = 0; y < img->height(); ++y)
        for (int x = 0; x < img->width(); ++x)
            brightValues[(int)(RGBA(img->pixel(x, y)).mid)]++;

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
            QColor color(ui->frame_2->layer[index].img_mix_save.pixel(x, y));

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

void setIntegralImage(layerIMG* layer) {//интегральна€ матрица
    int width = layer->img.width();
    int height = layer->img.height();

    layer->integralImage.resize(height);
    for (int y = 0; y < height; ++y) {
        layer->integralImage[y].resize(width);
        for (int x = 0; x < width; ++x) {
            int pixelValue = RGBA(layer->img.pixel(x, y)).mid;
            layer->integralImage[y][x] = pixelValue;
            if (y > 0) layer->integralImage[y][x] += layer->integralImage[y - 1][x];
            if (x > 0) layer->integralImage[y][x] += layer->integralImage[y][x - 1];
            if (y > 0 && x > 0) layer->integralImage[y][x] -= layer->integralImage[y - 1][x - 1];
        }
    }
}

#define border(x) min##x - 1 < 0 ? 0 : 

void binar(Ui::QTPhotoshopClass* ui, int index, int method, int mask_size, double k, double a) {
    QImage* img = &ui->frame_2->layer[index].img;

    for (int y = 0; y < img->height(); ++y)
        for (int x = 0; x < img->width(); ++x) {
            RGBA rgb = img->pixel(x, y);
            rgb.to_Grey();
            img->setPixel(x, y, qRgb(rgb.r, rgb.g, rgb.b));
        }

    int threshold{ 0 };

    if (method == 0) {//гавр---------------------------
        threshold = 0;
        for (int y = 0; y < img->height(); ++y)
            for (int x = 0; x < img->width(); ++x)
                threshold += RGBA(img->pixel(x, y)).mid;
        threshold /= img->height() * img->width();
        for (int y = 0; y < img->height(); ++y)
            for (int x = 0; x < img->width(); ++x)
                img->setPixel(x, y, RGBA(img->pixel(x, y)) <= threshold ? qRgb(0, 0, 0) : qRgb(255, 255, 255));
    }
    else if (method == 1) {//отсу----------------------------------
        threshold = 0;
        int nSumPix[256]{ 0 };
        float nProDis[256]{ 0 };
        for (int y = 0; y < img->height(); ++y)
            for (int x = 0; x < img->width(); ++x)
                nSumPix[(int)(RGBA(img->pixel(x, y)).mid)]++;

        for (int i = 0; i < 256; i++)
            nProDis[i] = (float)nSumPix[i] / (img->height() * img->width());

        float w0, w1, u0_temp, u1_temp, u0, u1, delta_temp;
        double delta_max = 0;
        for (int i = 0; i < 256; i++) {
            w0 = w1 = u0 = u1 = u0_temp = u1_temp = delta_temp = 0;
            for (int j = 0; j < 256; j++)
                if (j <= i) {
                    w0 += nProDis[j];
                    u0_temp += j * nProDis[j];
                }
                else {
                    w1 += nProDis[j];
                    u1_temp += j * nProDis[j];
                }
            u0 = u0_temp / w0;
            u1 = u1_temp / w1;
            delta_temp = (float)(w0 * w1 * (u0 - u1) * (u0 - u1));
            if (delta_temp > delta_max) {
                delta_max = delta_temp;
                threshold = i;
            }
        }
        for (int y = 0; y < img->height(); ++y)
            for (int x = 0; x < img->width(); ++x)
                img->setPixel(x, y, RGBA(img->pixel(x, y)) <= threshold ? qRgb(0, 0, 0) : qRgb(255, 255, 255));

    }
    else if (method == 2) { //ниблек-------------------------
        int halfSize = mask_size / 2;
        int width = img->width();
        int height = img->height();

        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x) {
                int minX = qMax(0, x - halfSize);
                int minY = qMax(0, y - halfSize);
                int maxX = qMin(width - 1, x + halfSize);
                int maxY = qMin(height - 1, y + halfSize);
                int areaSize = (maxX - minX + 1) * (maxY - minY + 1);
                int sum = ui->frame_2->layer[index].integralImage[maxY][maxX]
                    - (border(X) ui->frame_2->layer[index].integralImage[maxY][minX - 1])
                    - (border(Y) ui->frame_2->layer[index].integralImage[minY - 1][maxX])
                    + (border(X) (border(Y) ui->frame_2->layer[index].integralImage[minY - 1][minX - 1]));
                double MX = (double)sum / areaSize;
                double DX = 0;

                for (int i = minY; i <= maxY; ++i)
                    for (int j = minX; j <= maxX; ++j) {
                        int pixelValue = RGBA(img->pixel(j, i)).mid;
                        DX += pow(pixelValue - MX, 2);
                    }
                DX /= areaSize;

                double sigma = sqrt(DX);
                double threshold = MX + k * sigma;
                img->setPixel(x, y, RGBA(img->pixel(x, y)) <= threshold ? qRgb(0, 0, 0) : qRgb(255, 255, 255));
            }
    }
    else if (method == 3) {//—аув-------------------------
        int halfSize = mask_size / 2;
        int width = img->width();
        int height = img->height();

        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x) {
                int minX = qMax(0, x - halfSize);
                int minY = qMax(0, y - halfSize);
                int maxX = qMin(width - 1, x + halfSize);
                int maxY = qMin(height - 1, y + halfSize);
                int areaSize = (maxX - minX + 1) * (maxY - minY + 1);
                int sum = ui->frame_2->layer[index].integralImage[maxY][maxX]
                    - (border(X) ui->frame_2->layer[index].integralImage[maxY][minX - 1])
                    - (border(Y) ui->frame_2->layer[index].integralImage[minY - 1][maxX])
                    + (border(X) (border(Y) ui->frame_2->layer[index].integralImage[minY - 1][minX - 1]));
                double MX = (double)sum / areaSize;
                double DX = 0;

                for (int i = minY; i <= maxY; ++i)
                    for (int j = minX; j <= maxX; ++j) {
                        auto a = img->pixel(j, i);
                        int pixelValue = RGBA(img->pixel(j, i)).mid;
                        DX += pow(pixelValue - MX, 2);
                    }
                DX /= areaSize;

                double sigma = sqrt(DX);
                double threshold = MX * (1 + k * (sigma / 128. - 1));
                img->setPixel(x, y, RGBA(img->pixel(x, y)) <= threshold ? qRgb(0, 0, 0) : qRgb(255, 255, 255));
            }
    }
    else if (method == 4) {//¬ульф------------------------
        int halfSize = mask_size / 2;
        int width = img->width();
        int height = img->height();
        int min_pix{ 255 };
        int max_sigma{ -1 };
        QVector<QVector<double>> sigma_matrix(height, QVector<double>(width, 0)),
            MX_matrix(height, QVector<double>(width, 0));

        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x) {
                int minX = qMax(0, x - halfSize);
                int minY = qMax(0, y - halfSize);
                int maxX = qMin(width - 1, x + halfSize);
                int maxY = qMin(height - 1, y + halfSize);
                int areaSize = (maxX - minX + 1) * (maxY - minY + 1);
                int sum = ui->frame_2->layer[index].integralImage[maxY][maxX]
                    - (border(X) ui->frame_2->layer[index].integralImage[maxY][minX - 1])
                    - (border(Y) ui->frame_2->layer[index].integralImage[minY - 1][maxX])
                    + (border(X) (border(Y) ui->frame_2->layer[index].integralImage[minY - 1][minX - 1]));
                double MX = (double)sum / areaSize;
                MX_matrix[y][x] = MX;
                double DX = 0;

                for (int i = minY; i <= maxY; ++i)
                    for (int j = minX; j <= maxX; ++j) {
                        int pixelValue = RGBA(img->pixel(j, i)).mid;
                        min_pix = (min_pix > pixelValue ? pixelValue : min_pix);
                        DX += pow(pixelValue - MX, 2);
                    }
                DX /= areaSize;

                double sigma = sqrt(DX);
                sigma_matrix[y][x] = sigma;
                max_sigma = (max_sigma < sigma ? sigma : max_sigma);
            }
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x) {
                double threshold = (1 - a) * MX_matrix[y][x] + a * min_pix + a * sigma_matrix[y][x] / max_sigma * (MX_matrix[y][x] - min_pix);
                img->setPixel(x, y, RGBA(img->pixel(x, y)) <= threshold ? qRgb(0, 0, 0) : qRgb(255, 255, 255));
            }
    }
    else if (method == 5) {//–от---------------------------
        int halfSize = mask_size / 2;
        int width = img->width();
        int height = img->height();
        for (int y = 0; y < height; ++y)
            for (int x = 0; x < width; ++x) {
                int minX = qMax(0, x - halfSize);
                int minY = qMax(0, y - halfSize);
                int maxX = qMin(width - 1, x + halfSize);
                int maxY = qMin(height - 1, y + halfSize);
                int areaSize = (maxX - minX + 1) * (maxY - minY + 1);
                int sum = ui->frame_2->layer[index].integralImage[maxY][maxX]
                    - (border(X) ui->frame_2->layer[index].integralImage[maxY][minX - 1])
                    - (border(Y) ui->frame_2->layer[index].integralImage[minY - 1][maxX])
                    + (border(X) (border(Y) ui->frame_2->layer[index].integralImage[minY - 1][minX - 1]));
                bool condition = (RGBA(img->pixel(x, y)) * areaSize <= sum * (1 - k));
                img->setPixel(x, y, condition ? qRgb(0, 0, 0) : qRgb(255, 255, 255));
            }
    }

    display(ui);
}

void mask_filter(layerIMG* layer, int mh, int mw) {
    QByteArray byteArrayCopy(layer->byteImg);
    uchar* byteArray = reinterpret_cast<uchar*>(layer->byteImg.data());

    int imageWidth = layer->img.width();
    int imageHeight = layer->img.height();

    for (int i = 0; i < imageHeight * imageWidth; ++i) {
        int y = i / imageWidth;
        int x = i % imageWidth;

        int sum_r = 0, sum_g = 0, sum_b = 0;
            for (int dy = -mh / 2; dy <= mh / 2; ++dy)
                for (int dx = -mw / 2; dx <= mw / 2; ++dx) {
                    int nx = (x + dx < 0) ? 0 : ((x + dx >= imageWidth) ? imageWidth - 1 : x + dx);
                    int ny = (y + dy < 0) ? 0 : ((y + dy >= imageHeight) ? imageHeight - 1 : y + dy);
                    int pixelIndex = (ny * imageWidth + nx) * 4; // индекс начала пиксел€ в массиве байтов

                    sum_r += byteArray[pixelIndex];
                    sum_g += byteArray[pixelIndex + 1];
                    sum_b += byteArray[pixelIndex + 2];
                }
        int average_r = sum_r * (1. / (double)(mh * mw));
        int average_g = sum_g * (1. / (double)(mh * mw));
        int average_b = sum_b * (1. / (double)(mh * mw));

        byteArrayCopy[i * 4] = average_r;
        byteArrayCopy[i * 4 + 1] = average_g;
        byteArrayCopy[i * 4 + 2] = average_b;
    }

    layer->byteArrayToImage(byteArrayCopy);
}

int partition(QVector<int>& arr, int left, int right) {
    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; ++j)
        if (arr[j] <= pivot) {
            ++i;
            std::swap(arr[i], arr[j]);
        }

    std::swap(arr[i + 1], arr[right]);
    return i + 1;
}

int quickSelect(QVector<int>& arr, int left, int right, int k) {
    int pivotIndex = partition(arr, left, right); 
    int pivotRank = pivotIndex - left + 1; 

    if (k == pivotRank) return arr[pivotIndex];
    else if (k < pivotRank) return quickSelect(arr, left, pivotIndex - 1, k);
    else return quickSelect(arr, pivotIndex + 1, right, k - pivotRank);
}

void median_filter(layerIMG* layer, int mh, int mw) {
    QByteArray byteArrayCopy(layer->byteImg);
    uchar* byteArray = reinterpret_cast<uchar*>(layer->byteImg.data());

    int imageWidth = layer->img.width();
    int imageHeight = layer->img.height();

    for (int i = 0; i < imageHeight * imageWidth; ++i) {
        int y = i / imageWidth;
        int x = i % imageWidth;

        QVector<int> sort_maskR(mh * mw);
        QVector<int> sort_maskG(mh * mw);
        QVector<int> sort_maskB(mh * mw);
        int index{ 0 };

        for (int dy = -mh / 2; dy <= mh / 2; ++dy)
            for (int dx = -mw / 2; dx <= mw / 2; ++dx) {
                int nx = (x + dx < 0) ? 0 : ((x + dx >= imageWidth) ? imageWidth - 1 : x + dx);
                int ny = (y + dy < 0) ? 0 : ((y + dy >= imageHeight) ? imageHeight - 1 : y + dy);
                int pixelIndex = (ny * imageWidth + nx) * 4; // индекс начала пиксел€ в массиве байтов
                
                sort_maskR[index] = byteArray[pixelIndex];
                sort_maskG[index] = byteArray[pixelIndex + 1];
                sort_maskB[index] = byteArray[pixelIndex + 2];

                index++;
            }
        
        int index_mid = mh * mw / 2. + 1;

        byteArrayCopy[i * 4] = quickSelect(sort_maskR, 0, sort_maskR.size() - 1, index_mid);
        byteArrayCopy[i * 4 + 1] = quickSelect(sort_maskG, 0, sort_maskR.size() - 1, index_mid);
        byteArrayCopy[i * 4 + 2] = quickSelect(sort_maskB, 0, sort_maskR.size() - 1, index_mid);
    }

    layer->byteArrayToImage(byteArrayCopy);
}

QVector<QVector<double>> GaussianCore(int size, double sigma) {
    QVector<QVector<double>> core(size, QVector<double>(size));

    double sum = 0.;
    int halfSize = size / 2;

    for (int x = -halfSize; x <= halfSize; ++x)
        for (int y = -halfSize; y <= halfSize; ++y) {
            double exponent = -(x * x + y * y) / (2 * sigma * sigma);
            core[x + halfSize][y + halfSize] = exp(exponent) / (2 * M_PI * sigma * sigma);
            sum += core[x + halfSize][y + halfSize];
        }

    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            core[i][j] /= sum;

    return core;
}

void gaussFilter(layerIMG* layer) {
    QVector<QVector<double>>& core = layer->gaussCore;
    QByteArray byteArrayCopy(layer->byteImg);
    uchar* byteArray = reinterpret_cast<uchar*>(layer->byteImg.data());

    int imageWidth = layer->img.width();
    int imageHeight = layer->img.height();

    int mh = core.size(), mw = mh;

    for (int i = 0; i < imageHeight * imageWidth; ++i) {
        int y = i / imageWidth;
        int x = i % imageWidth;

        int sum_r = 0, sum_g = 0, sum_b = 0;
        for (int dy = -mh / 2; dy <= mh / 2; ++dy)
            for (int dx = -mw / 2; dx <= mw / 2; ++dx) {
                int nx = (x + dx < 0) ? 0 : ((x + dx >= imageWidth) ? imageWidth - 1 : x + dx);
                int ny = (y + dy < 0) ? 0 : ((y + dy >= imageHeight) ? imageHeight - 1 : y + dy);
                int pixelIndex = (ny * imageWidth + nx) * 4; // индекс начала пиксел€ в массиве байтов

                sum_r += byteArray[pixelIndex]     * core[dx + mh / 2][dy + mh / 2];
                sum_g += byteArray[pixelIndex + 1] * core[dx + mh / 2][dy + mh / 2];
                sum_b += byteArray[pixelIndex + 2] * core[dx + mh / 2][dy + mh / 2];
            }

        byteArrayCopy[i * 4] = sum_r;
        byteArrayCopy[i * 4 + 1] = sum_g;
        byteArrayCopy[i * 4 + 2] = sum_b;
    }

    layer->byteArrayToImage(byteArrayCopy);
}