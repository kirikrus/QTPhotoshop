#include <QObject>
#include <omp.h>
#include "ui_QTPhotoshop.h"

void editIMGProp(Ui::QTPhotoshopClass*, int, int, QString);
void mode(Ui::QTPhotoshopClass*);
void histo_build(QImage*,QCustomPlot*);
void applyContrastCurve(Ui::QTPhotoshopClass*, int);
void binar(Ui::QTPhotoshopClass*, int, int, int, double, double);
void setIntegralImage(layerIMG*);
void mask_filter(layerIMG*, int, int);
