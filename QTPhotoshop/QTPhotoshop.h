#include <QtWidgets/QMainWindow>
#include "displayIMG.h"

class QTPhotoshop : public QMainWindow{
    Q_OBJECT

public:
    QTPhotoshop(QWidget *parent = nullptr);
    ~QTPhotoshop();

    Ui::QTPhotoshopClass ui;

private slots:
    void update();
};
