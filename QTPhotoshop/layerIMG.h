#include <qimage.h>
#include <QPainter>
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
	QImage img, img_save;
	int offsetX, offsetY,
		mode = Normal,
		alpha,
		h, w;
	QVector<QPointF> controlPoints{ QPointF(0.,0.),  QPointF(175.,175.) };

	layerIMG(QImage);

	void convertToARGB();
	void setAlpha();
};

