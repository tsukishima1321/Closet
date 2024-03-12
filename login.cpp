#include "login.h"
#include "ui_login.h"
#include "dbwidget.h"

login::login(QWidget *parent, QList<Item *> *itemList) :
    QWidget(parent),
    ui(new Ui::login)
{
    this->itemList=itemList;
    ui->setupUi(this);
}

login::~login()
{
    delete ui;
}

void login::on_pushButton_clicked()
{
    dbWidget* dbw=new dbWidget(nullptr,itemList,ui->lineEdit->text(),ui->lineEdit_2->text());
    delete this;
}

