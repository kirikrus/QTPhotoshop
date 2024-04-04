#include <qimage.h>
#include <QPainter>
#include <qbuffer.h>
#include "curve.h"

enum blendingMode {
	Normal,
	Darken,
	Multiply,
	Exclusion,
	Subtract,
	Divide,
	Screen,
	Invert,
	Addition
};

struct layerIMG{
	QImage img, img_save, img_mix_save;
	int offsetX, offsetY,
		mode = Normal,
		alpha,
		h, w;
	QVector<QPointF> controlPoints;
	QVector<QVector<int>> integralImage;
	QByteArray byteImg;

	layerIMG(QImage);

	void imageToByteArray();
	void byteArrayToImage(QByteArray);
	void convertToARGB();
	void setAlpha();
};

