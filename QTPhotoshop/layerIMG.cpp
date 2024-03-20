#include "layerIMG.h"

layerIMG::layerIMG(QImage img)
	: img(img), img_save(img), offsetX(0), offsetY(0), mode(Normal), alpha(100)
{}

void layerIMG::convertToARGB() {
    img_save = img = img.convertToFormat(QImage::Format_ARGB32);
}

void layerIMG::setAlpha() {
    convertToARGB();
    for (int y = 0; y < img.height(); ++y)
        for (int x = 0; x < img.width(); ++x) {
            QRgb pixel = img.pixel(x, y);
            pixel = qRgba(qRed(pixel), qGreen(pixel), qBlue(pixel), 255 * alpha / 100.);
            img.setPixel(x, y, pixel);
        }
}