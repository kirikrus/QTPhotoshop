#include <QObject>
#include <qevent.h>
#include "ui_QTPhotoshop.h"

void displayProp(Ui::QTPhotoshopClass*, int);

class clickedPanel : public QObject {
    Q_OBJECT
public:
    clickedPanel(Ui::QTPhotoshopClass* ui, int index, QWidget* parent = nullptr)
        : parent(parent), ui(ui), index(index) {}

protected:
    bool eventFilter(QObject* obj, QEvent* event) {
        if (event->type() == QEvent::Enter) parent->setStyleSheet("background-color: rgb(136, 136, 136);");
        if (event->type() == QEvent::Leave) parent->setStyleSheet("");
        if (event->type() == QEvent::MouseButtonPress) {
            ui->scrollAreaWidgetContents_2->show();
            displayProp(ui, index);
        }
        return 0;
    }

private:
    QWidget* parent;
    Ui::QTPhotoshopClass* ui;
    int index;
};

