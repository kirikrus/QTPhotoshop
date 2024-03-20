#include "IMGmagic.h"
#include "displayIMG.h"

void editIMGProp(Ui::QTPhotoshopClass* ui, int index, int value, QString senderName) {
    if (senderName == "offsetX")
        ui->frame_2->layer[index].offsetX = value;
    else if (senderName == "offsetY")
        ui->frame_2->layer[index].offsetY = value;
    if (senderName == "height")
        ui->frame_2->layer[index].img = ui->frame_2->layer[index].img_save.scaled(ui->frame_2->layer[index].img_save.width(), value);
    else if (senderName == "width")
        ui->frame_2->layer[index].img = ui->frame_2->layer[index].img_save.scaled(value, ui->frame_2->layer[index].img_save.height());
    else if (senderName == "alpha") {
        ui->frame_2->layer[index].alpha = value;
        ui->frame_2->layer[index].setAlpha();
    }
    display(ui);
}