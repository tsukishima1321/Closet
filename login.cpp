#include "login.h"
#include "ui_login.h"
#include "dbwidget.h"

login::login(QWidget *parent, QList<Item *> *itemList) :
    QWidget(parent),
    ui(new Ui::login)
{
    this->itemList=itemList;
    ui->setupUi(this);
    connect(ui->lineEditPwd, &QLineEdit::returnPressed, ui->pushButton, &QAbstractButton::click, Qt::UniqueConnection);
}

login::~login()
{
    delete ui;
}

void login::on_pushButton_clicked()
{
    dbWidget::Instance(nullptr,itemList,ui->lineEditName->text(),ui->lineEditPwd->text());
    this->hide();
    delete this;
}


void login::on_lineEditName_returnPressed()
{
    this->focusPreviousChild();
}

