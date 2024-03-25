#include "layerIMG.h"

layerIMG::layerIMG(QImage img)
	: img(img), img_save(img), offsetX(0), offsetY(0), mode(Normal), alpha(100), h(img.height()), w(img.width())
{}

void layerIMG::convertToARGB() {
    img_save = img = img.convertToFormat(QImage::Format_ARGB32);
}

void layerIMG::setAlpha() {
    convertToARGB();
    for (int y = 0; y < img.height(); ++y)
        for (int x = 0; x < img.width(); ++x) {
            QRgb pixel = img.pixel(x, y);
            pixel = qRgba(qRed(pixel), qGreen(pixel), qBlue(pixel), 2.55 * alpha);
            img.setPixel(x, y, pixel);
        }
}