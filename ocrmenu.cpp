#include "ocrmenu.h"
#include "ui_ocrmenu.h"

ocrMenu::ocrMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ocrMenu)
{
    ui->setupUi(this);
}

ocrMenu::~ocrMenu()
{
    delete ui;
}
