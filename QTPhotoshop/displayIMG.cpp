#include "displayIMG.h"
#include <QObject>

//тут мозг отрисовки панелек

void layChange(Ui::QTPhotoshopClass* ui, int index, bool up) {//меняем порядок массива слоев
    if (!up && index < ui->frame_2->layer.size() - 1) qSwap(ui->frame_2->layer[index], ui->frame_2->layer[index + 1]);
    if (up && index > 0) qSwap(ui->frame_2->layer[index], ui->frame_2->layer[index - 1]);
    display(ui);
}

void style(Ui::QTPhotoshopClass* ui) {
    ui->photo_zone->setStyleSheet(PURPLE_SCROLL);
    ui->scalerInc->setStyleSheet(PURPLE_BUTTON);
    ui->scalerDec->setStyleSheet(PURPLE_BUTTON);
    ui->scrollArea->setStyleSheet(PURPLE_SCROLL);
    ui->settingsArea->setStyleSheet(PURPLEE_LINE);
}

void display(Ui::QTPhotoshopClass* ui){

    style(ui);

    QLayoutItem* item;
    while ((item = ui->verticalLayout->takeAt(0)) != nullptr) {//чистим панель от старых виджеты 
        delete item->widget();
        delete item;
    }

    if(ui->photo_zone->scene())
        delete ui->photo_zone->scene();
    
    QGraphicsScene* scene = new QGraphicsScene(ui->photo_zone);
    for (int i{ 0 };i < ui->frame_2->layer.size();i++) {
        QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(QPixmap::fromImage(ui->frame_2->layer[i].img));
        pixmapItem->setZValue(i);
        pixmapItem->setOffset(ui->frame_2->layer[i].offsetX, ui->frame_2->layer[i].offsetY);
        scene->addItem(pixmapItem);

        QWidget* miniPanelIMG;
        QGridLayout* gridLayout;
        QPushButton* layDown;
        QPushButton* layUp;
        QLabel* label;
        QLabel* miniImg;

#pragma region отрисовка панелек слоев

        miniPanelIMG = new QWidget(ui->scrollAreaWidgetContents);
        miniPanelIMG->setMinimumSize(QSize(0, 100));
        miniPanelIMG->setMaximumSize(QSize(16777215, 100));
        miniPanelIMG->installEventFilter(new clickedPanel(ui, i, miniPanelIMG));//тут ивенты панелек
        gridLayout = new QGridLayout(miniPanelIMG);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        layDown = new QPushButton(miniPanelIMG);
        layDown->setMinimumSize(QSize(30, 0));
        layDown->setMaximumSize(QSize(30, 16777215));
        layDown->setText("▼");
        QObject::connect(layDown, &QPushButton::clicked, [=]() {//фу, лямбда
            layChange(ui, i, 0);
            });
        gridLayout->addWidget(layDown, 1, 2, 1, 1);
        layUp = new QPushButton(miniPanelIMG);
        layUp->setMinimumSize(QSize(0, 0));
        layUp->setMaximumSize(QSize(30, 16777215));
        layUp->setText("▲");
        QObject::connect(layUp, &QPushButton::clicked, [=]() {//  \(0 Д  0)/
            layChange(ui, i, 1);
            });
        gridLayout->addWidget(layUp, 1, 1, 1, 1);
        label = new QLabel(miniPanelIMG);
        label->setScaledContents(false);
        label->setAlignment(Qt::AlignCenter);
        label->setText("Слой " + QString::number(i));
        label->setMouseTracking(true);
        label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        gridLayout->addWidget(label, 0, 1, 1, 2);
        miniImg = new QLabel(miniPanelIMG);
        miniImg->setAlignment(Qt::AlignCenter);
        miniImg->setPixmap(QPixmap::fromImage(ui->frame_2->layer[i].img).scaled(80, 80, Qt::KeepAspectRatio));
        label->setMouseTracking(true);
        label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        gridLayout->addWidget(miniImg, 0, 0, 2, 1);

#pragma endregion

        ui->verticalLayout->addWidget(miniPanelIMG);
    }

    ui->photo_zone->setScene(scene);
    ui->frame_2->setStyleSheet("background-color: rgba(255, 255, 255, 0);");
}

void scale(Ui::QTPhotoshopClass* ui, float inc) {//скейл фото зоны
    qreal currentScale = ui->photo_zone->transform().m11();
    qreal newScale = currentScale + inc;
    ui->photo_zone->setTransform(QTransform().scale(newScale, newScale));
}

void displayProp(Ui::QTPhotoshopClass* ui, int index) {//отрисовка панельки с начтройками индивидуально для каждого слоя
    
    delete ui->scrollAreaWidgetContents_2;

    QLineEdit* offsetY;
    QLineEdit* offsetX;
    QLabel* label_2;
    QLabel* label_3;
    QComboBox* modes;
    QLabel* label_4;
    QLineEdit* height;
    QLabel* label_5;
    QLineEdit* width;
    QLabel* label_6;
    QLabel* label_7;
    QLineEdit* alpha;
    QWidget* scrollAreaWidgetContents_2;

#define propConnect(elem) QObject::connect(elem, &QLineEdit::textChanged, [=]() { \
    editIMGProp(ui, index, elem->text().toInt(), #elem);\
    });

#pragma region отрисовка панели настройки 💀💀💀

    scrollAreaWidgetContents_2 = new QWidget();
    scrollAreaWidgetContents_2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_2"));
    scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 189, 579));
    offsetY = new QLineEdit(scrollAreaWidgetContents_2);
    offsetY->setGeometry(QRect(110, 40, 41, 21));
    offsetY->setInputMethodHints(Qt::ImhDigitsOnly);
    offsetY->setText(QString::number(ui->frame_2->layer[index].offsetY));
    propConnect(offsetY);

    offsetX = new QLineEdit(scrollAreaWidgetContents_2);
    offsetX->setGeometry(QRect(40, 40, 41, 21));
    offsetX->setInputMethodHints(Qt::ImhDigitsOnly);
    offsetX->setText(QString::number(ui->frame_2->layer[index].offsetX));
    propConnect(offsetX);

    label_2 = new QLabel(scrollAreaWidgetContents_2);
    label_2->setGeometry(QRect(40, 20, 40, 16));
    label_2->setAlignment(Qt::AlignCenter);
    label_3 = new QLabel(scrollAreaWidgetContents_2);
    label_3->setGeometry(QRect(110, 20, 40, 16));
    label_3->setAlignment(Qt::AlignCenter);

    modes = new QComboBox(scrollAreaWidgetContents_2);
    modes->addItem(QString("Нормальное"));
    modes->addItem(QString("Затухание"));
    modes->addItem(QString("Затемнение"));
    modes->addItem(QString("Умножение"));
    modes->addItem(QString("Исключение"));
    modes->addItem(QString("Вычитание"));
    modes->addItem(QString("Разделение"));
    modes->setGeometry(QRect(40, 160, 111, 22));
    modes->setCurrentIndex(ui->frame_2->layer[index].mode);
    modes->setStyleSheet(PURPLE_LIST);

    label_4 = new QLabel(scrollAreaWidgetContents_2);
    label_4->setGeometry(QRect(30, 80, 60, 16));
    label_4->setAlignment(Qt::AlignCenter);

    height = new QLineEdit(scrollAreaWidgetContents_2);
    height->setGeometry(QRect(110, 100, 41, 21));
    height->setInputMethodHints(Qt::ImhDigitsOnly);
    height->setText(QString::number(ui->frame_2->layer[index].img.height()));
    propConnect(height);

    label_5 = new QLabel(scrollAreaWidgetContents_2);
    label_5->setGeometry(QRect(100, 80, 60, 16));
    label_5->setAlignment(Qt::AlignCenter);

    width = new QLineEdit(scrollAreaWidgetContents_2);
    width->setGeometry(QRect(40, 100, 41, 21));
    width->setInputMethodHints(Qt::ImhDigitsOnly);
    width->setText(QString::number(ui->frame_2->layer[index].img.width()));
    propConnect(width);

    label_6 = new QLabel(scrollAreaWidgetContents_2);
    label_6->setGeometry(QRect(40, 140, 110, 16));
    label_6->setAlignment(Qt::AlignCenter);

    label_7 = new QLabel(scrollAreaWidgetContents_2);
    label_7->setGeometry(QRect(40, 200, 110, 16));
    label_7->setAlignment(Qt::AlignCenter);

    alpha = new QLineEdit(scrollAreaWidgetContents_2);
    alpha->setGeometry(QRect(70, 220, 50, 21));
    alpha->setInputMethodHints(Qt::ImhDigitsOnly);
    alpha->setText(QString::number(ui->frame_2->layer[index].alpha));
    propConnect(alpha);

    ui->settingsArea->setWidget(scrollAreaWidgetContents_2);
    ui->scrollAreaWidgetContents_2 = scrollAreaWidgetContents_2;

#pragma region скример НЕ ОТКРЫВАТЬ
    label_2->setText(QCoreApplication::translate("QTPhotoshopClass", "offsetX", nullptr));
    label_3->setText(QCoreApplication::translate("QTPhotoshopClass", "offsetY", nullptr));
    modes->setItemText(0, QCoreApplication::translate("QTPhotoshopClass", "\320\235\320\276\321\200\320\274\320\260\320\273\321\214\320\275\320\276\320\265", nullptr));
    modes->setItemText(1, QCoreApplication::translate("QTPhotoshopClass", "\320\227\320\260\321\202\321\203\321\205\320\260\320\275\320\270\320\265", nullptr));
    modes->setItemText(2, QCoreApplication::translate("QTPhotoshopClass", "\320\227\320\260\321\202\320\265\320\274\320\275\320\265\320\275\320\270\320\265", nullptr));
    modes->setItemText(3, QCoreApplication::translate("QTPhotoshopClass", "\320\243\320\274\320\275\320\276\320\266\320\265\320\275\320\270\320\265", nullptr));
    modes->setItemText(4, QCoreApplication::translate("QTPhotoshopClass", "\320\230\321\201\320\272\320\273\321\216\321\207\320\265\320\275\320\270\320\265", nullptr));
    modes->setItemText(5, QCoreApplication::translate("QTPhotoshopClass", "\320\222\321\213\321\207\320\270\321\202\320\260\320\275\320\270\320\265", nullptr));
    modes->setItemText(6, QCoreApplication::translate("QTPhotoshopClass", "\320\240\320\260\320\267\320\264\320\265\320\273\320\265\320\275\320\270\320\265", nullptr));

    modes->setPlaceholderText(QCoreApplication::translate("QTPhotoshopClass", "\320\235\320\276\321\200\320\274\320\260\320\273\321\214\320\275\320\276\320\265", nullptr));
    label_4->setText(QCoreApplication::translate("QTPhotoshopClass", "\320\250\320\270\321\200\320\270\320\275\320\260", nullptr));
    label_5->setText(QCoreApplication::translate("QTPhotoshopClass", "\320\222\321\213\321\201\320\276\321\202\320\260", nullptr));
    label_6->setText(QCoreApplication::translate("QTPhotoshopClass", "\320\240\320\265\320\266\320\270\320\274 \320\275\320\260\320\273\320\276\320\266\320\265\320\275\320\270\321\217", nullptr));
    label_7->setText(QCoreApplication::translate("QTPhotoshopClass", "\320\237\321\200\320\276\320\267\321\200\320\260\321\207\320\275\320\276\321\201\321\202\321\214 %", nullptr));
#pragma endregion


#pragma endregion

}