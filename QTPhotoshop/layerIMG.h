#include <qimage.h>

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
	QImage img;
	int offsetX, offsetY,
	 mode = Normal;

	layerIMG(QImage);
};

