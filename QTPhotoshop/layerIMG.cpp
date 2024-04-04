#include "layerIMG.h"

layerIMG::layerIMG(QImage img)
	: img(img), offsetX(0), offsetY(0),
    mode(Normal), alpha(100), h(img.height()), w(img.width()),
    controlPoints({ QPointF(0.,0.),  QPointF(175.,175.) })
{
    img_save = img.copy();
    img_mix_save = img.copy();
}

void layerIMG::imageToByteArray() {byteImg = QByteArray::fromRawData((const char*)img.bits(), img.sizeInBytes());}
void layerIMG::byteArrayToImage(QByteArray byteImg) {
    const char* data = byteImg.constData();
    memcpy(img.bits(), data, img.sizeInBytes());
}

void layerIMG::convertToARGB() {
    img = img.convertToFormat(QImage::Format_ARGB32);
    img_save = img_save.convertToFormat(QImage::Format_ARGB32);
    img_mix_save = img_mix_save.convertToFormat(QImage::Format_ARGB32);
}

void layerIMG::setAlpha() {
    convertToARGB();
    for (int y = 0; y < img.height(); ++y)
        for (int x = 0; x < img.width(); ++x) {
            QRgb pixel = img.pixel(x, y);
            QRgb pixel2 = img_mix_save.pixel(x, y);
            pixel = qRgba(qRed(pixel), qGreen(pixel), qBlue(pixel), 2.55 * alpha);
            pixel2 = qRgba(qRed(pixel2), qGreen(pixel2), qBlue(pixel2), 2.55 * alpha);
            img.setPixel(x, y, pixel);
            img_mix_save.setPixel(x, y, pixel2);
        }
}