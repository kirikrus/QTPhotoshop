#include <QtWidgets/QMainWindow>
#include "ui_QTPhotoshop.h"

class QTPhotoshop : public QMainWindow{
    Q_OBJECT

public:
    QTPhotoshop(QWidget *parent = nullptr);
    ~QTPhotoshop();

    Ui::QTPhotoshopClass ui;
};
