#include "displayIMG.h"
#include <QObject>
#include <QLineEdit>
#include <qcombobox.h>
//тут мозг отрисовки панелек

void layChange(Ui::QTPhotoshopClass* ui, int index, bool up) {//меняем порядок массива слоев
    if (!up && index < ui->frame_2->layer.size() - 1)
        qSwap(ui->frame_2->layer[index], ui->frame_2->layer[index + 1]);
    if (up && index > 0)
        qSwap(ui->frame_2->layer[index], ui->frame_2->layer[index - 1]);
    displayProp(ui, index);
}

void style(Ui::QTPhotoshopClass* ui) {
    ui->photo_zone->setStyleSheet(PURPLE_SCROLL);
    ui->scalerInc->setStyleSheet(PURPLE_BUTTON);
    ui->scalerDec->setStyleSheet(PURPLE_BUTTON);
    ui->scrollArea->setStyleSheet(PURPLE_SCROLL);
    ui->settingsArea->setStyleSheet(PURPLEE_LINE);

    ui->prop1->setStyleSheet(PURPLE_BUTTON);
    ui->prop2->setStyleSheet(PURPLE_BUTTON);
    ui->prop3->setStyleSheet(PURPLE_BUTTON);
    ui->prop4->setStyleSheet(PURPLE_BUTTON);

    ui->binarGavr->setStyleSheet(PURPLE_BUTTON);
    ui->binarOtsu->setStyleSheet(PURPLE_BUTTON);
    ui->binarNibl->setStyleSheet(PURPLE_BUTTON);
    ui->binarSauv->setStyleSheet(PURPLE_BUTTON);
    ui->binarVulf->setStyleSheet(PURPLE_BUTTON);
    ui->binarBrad->setStyleSheet(PURPLE_BUTTON);

    ui->sensitivity->setStyleSheet(PURPLEE_LINE);
    ui->maskSize->setStyleSheet(PURPLEE_LINE);
    ui->vulf_a->setStyleSheet(PURPLEE_LINE);

    ui->lineFilter->setStyleSheet(PURPLE_BUTTON);
    ui->medianFilter->setStyleSheet(PURPLE_BUTTON);
    ui->maskSizeM->setStyleSheet(PURPLEE_LINE);
    ui->maskSizeN->setStyleSheet(PURPLEE_LINE);
    ui->GaussCore->setStyleSheet(PURPLE_BUTTON);
    ui->gaussFilter->setStyleSheet(PURPLE_BUTTON);
    ui->sigma->setStyleSheet(PURPLEE_LINE);
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
    
    ui->svgLoad->hide();

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
            display(ui);
            });
        gridLayout->addWidget(layDown, 1, 2, 1, 1);
        layUp = new QPushButton(miniPanelIMG);
        layUp->setMinimumSize(QSize(0, 0));
        layUp->setMaximumSize(QSize(30, 16777215));
        layUp->setText("▲");
        QObject::connect(layUp, &QPushButton::clicked, [=]() {//  \(0 Д  0)/
            layChange(ui, i, 1);
            display(ui);
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
    
    mode(ui);

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
    QPushButton* pushButton;
    QCustomPlot* histo;
    QGraphicsView* curve;

#define propConnect(elem) QObject::connect(elem, &QLineEdit::textChanged, [=]() { \
    editIMGProp(ui, index, elem->text().toInt(), #elem);\
    });

#pragma region отрисовка панели настройки 💀💀💀

    scrollAreaWidgetContents_2 = new QWidget();
    scrollAreaWidgetContents_2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_2"));
    scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 189, 579));

    histo = new QCustomPlot(scrollAreaWidgetContents_2);
    histo->setGeometry(QRect(10, 450, 181, 80));
    histo_build(&ui->frame_2->layer[index].img, histo);//гистограмма

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
    modes->addItem(QString("Затемнение"));
    modes->addItem(QString("Умножение"));
    modes->addItem(QString("Исключение"));
    modes->addItem(QString("Вычитание"));
    modes->addItem(QString("Разделение"));
    modes->addItem(QString("Экран"));
    modes->addItem(QString("Инверсия"));
    modes->addItem(QString("Дополнение"));
    modes->setGeometry(QRect(40, 160, 111, 22));
    modes->setCurrentIndex(ui->frame_2->layer[index].mode);
    modes->setStyleSheet(PURPLE_LIST);
    QObject::connect(modes, QOverload<int>::of(&QComboBox::activated), [=]() {//  \(0 Д  0)/
        ui->frame_2->layer[index].mode = modes->currentIndex();
        mode(ui);
        applyContrastCurve(ui, index);
        histo_build(&ui->frame_2->layer[index].img, histo);
        display(ui);
        });

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

    pushButton = new QPushButton(scrollAreaWidgetContents_2);
    pushButton->setObjectName(QString::fromUtf8("pushButton"));
    pushButton->setGeometry(QRect(50, 540, 90, 30));
    QFont font;
    font.setBold(true);
    pushButton->setFont(font);
    pushButton->setText("Удалить слой");
    pushButton->setStyleSheet(QString::fromUtf8("border-radius: 10px;""background: rgb(255, 85, 127);"));
    QObject::connect(pushButton, &QPushButton::clicked, [=]() {
        ui->frame_2->layer.erase(ui->frame_2->layer.begin() + index);
        if (index == 0) ui->scrollAreaWidgetContents_2->hide();
        mode(ui);
        display(ui);
        });

    curve = new QGraphicsView(scrollAreaWidgetContents_2);
    curve->setGeometry(QRect(10, 270, 181, 181));
    curve->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    curve->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    curve->setTransform(QTransform::fromScale(1, -1));
    CurveItem* curveItem = new CurveItem(&ui->frame_2->layer[index].controlPoints);
    QGraphicsScene* scene = new QGraphicsScene(curve);
    scene->addItem(curveItem);
    curve->setScene(scene);
    QObject::connect(curveItem, &CurveItem::curveChanged, [=]() {
        applyContrastCurve(ui, index);
        histo_build(&ui->frame_2->layer[index].img,histo);
        display(ui);
        });

    if (ui->frame_2->layer[index].img == ui->frame_2->layer[index].img_mix_save && ui->frame_2->layer[index].controlPoints.size() > 2) {
        applyContrastCurve(ui, index);
        histo_build(&ui->frame_2->layer[index].img, histo);
    }


    ui->settingsArea->setWidget(scrollAreaWidgetContents_2);
    ui->scrollAreaWidgetContents_2 = scrollAreaWidgetContents_2;

    //нижние переключатели вкладок
    QObject::connect(ui->prop1, &QPushButton::pressed, [=]() {ui->tabWidget->setCurrentIndex(0);});
    QObject::connect(ui->prop2, &QPushButton::pressed, [=]() {
        setIntegralImage(&(ui->frame_2->layer[index]));
        ui->tabWidget->setCurrentIndex(3); });
    QObject::connect(ui->prop3, &QPushButton::pressed, [=]() {
        ui->frame_2->layer[index].imageToByteArray();
        ui->tabWidget->setCurrentIndex(1); });
    QObject::connect(ui->prop4, &QPushButton::pressed, [=]() {ui->tabWidget->setCurrentIndex(2); });

    //бинарные коннекты
    {
#define scan_param int mask_size = ui->maskSize->text().toInt();\
    mask_size = (mask_size <= 0 ? 3 : mask_size);\
    double sensitivity = ui->sensitivity->text().toDouble();\
    double a = ui->vulf_a->text().toDouble();

        QObject::connect(ui->binarGavr, &QPushButton::pressed, [=]() {binar(ui, index, 0, 0, 0, 0); });
        QObject::connect(ui->binarOtsu, &QPushButton::pressed, [=]() {binar(ui, index, 1, 0, 0, 0); });
        QObject::connect(ui->binarNibl, &QPushButton::pressed, [=]() {
            scan_param
                binar(ui, index, 2, mask_size, sensitivity, 0); });
        QObject::connect(ui->binarSauv, &QPushButton::pressed, [=]() {
            scan_param
                binar(ui, index, 3, mask_size, sensitivity, 0); });
        QObject::connect(ui->binarVulf, &QPushButton::pressed, [=]() {
            scan_param
                binar(ui, index, 4, mask_size, sensitivity, a); });
        QObject::connect(ui->binarBrad, &QPushButton::pressed, [=]() {
            scan_param
                binar(ui, index, 5, mask_size, sensitivity, 0); });
    }
    {
        //прост фильтрац коннекты
#define scan_param int mask_sizeN = ui->maskSizeN->text().toInt();\
    int mask_sizeM = ui->maskSizeM->text().toInt();

        QObject::connect(ui->lineFilter, &QPushButton::pressed, [=]() {
            scan_param
                mask_filter(&(ui->frame_2->layer[index]), mask_sizeN, mask_sizeM);
            display(ui);
            });
        QObject::connect(ui->medianFilter, &QPushButton::pressed, [=]() {
            scan_param
                median_filter(&(ui->frame_2->layer[index]), mask_sizeN, mask_sizeM);
            display(ui);
            });
        QObject::connect(ui->GaussCore, &QPushButton::pressed, [=]() {
            ui->frame_2->layer[index].gaussCore = GaussianCore(ui->maskSizeN->text().toInt(),ui->sigma->text().toInt());
            display(ui);
            });
        QObject::connect(ui->gaussFilter, &QPushButton::pressed, [=]() {
            gaussFilter(&(ui->frame_2->layer[index]));
            display(ui);
            });
    }
#pragma region скример НЕ ОТКРЫВАТЬ
    label_2->setText(QCoreApplication::translate("QTPhotoshopClass", "offsetX", nullptr));
    label_3->setText(QCoreApplication::translate("QTPhotoshopClass", "offsetY", nullptr));

    modes->setPlaceholderText(QCoreApplication::translate("QTPhotoshopClass", "\320\235\320\276\321\200\320\274\320\260\320\273\321\214\320\275\320\276\320\265", nullptr));
    label_4->setText(QCoreApplication::translate("QTPhotoshopClass", "\320\250\320\270\321\200\320\270\320\275\320\260", nullptr));
    label_5->setText(QCoreApplication::translate("QTPhotoshopClass", "\320\222\321\213\321\201\320\276\321\202\320\260", nullptr));
    label_6->setText(QCoreApplication::translate("QTPhotoshopClass", "\320\240\320\265\320\266\320\270\320\274 \320\275\320\260\320\273\320\276\320\266\320\265\320\275\320\270\321\217", nullptr));
    label_7->setText(QCoreApplication::translate("QTPhotoshopClass", "\320\237\321\200\320\276\320\267\321\200\320\260\321\207\320\275\320\276\321\201\321\202\321\214 %", nullptr));
#pragma endregion


#pragma endregion

}