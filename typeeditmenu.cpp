#include "typeeditmenu.h"
#include "ui_typeeditmenu.h"

typeEditMenu::typeEditMenu(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::typeEditMenu)
{
    ui->setupUi(this);
}

typeEditMenu::~typeEditMenu()
{
    delete ui;
}
