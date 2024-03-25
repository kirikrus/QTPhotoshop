#include <QObject>
#include "ui_QTPhotoshop.h"

void editIMGProp(Ui::QTPhotoshopClass*, int, int, QString);
void mode(Ui::QTPhotoshopClass*);
void histo_build(QImage*,QCustomPlot*);
void applyContrastCurve(Ui::QTPhotoshopClass* ui, int index);