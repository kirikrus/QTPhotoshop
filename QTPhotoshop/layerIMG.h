#include <qimage.h>
#include <QPainter>

enum blendingMode {
	Normal,
	Dissolve,
	Darken,
	Multiply,
	Exclusion,
	Subtract,
	Divide
};

struct layerIMG{
	QImage img, img_save;
	int offsetX, offsetY,
		mode = Normal,
		alpha;

	layerIMG(QImage);

	void convertToARGB();
	void setAlpha();
};

