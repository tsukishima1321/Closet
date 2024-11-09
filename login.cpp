#include "login.h"
#include "dbinstance.h"
#include "iconresources.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
        QDialog(parent),
        ui(new Ui::Login) {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    ui->toolButton->setIcon(IconResources::getIcons()["square-x"]);
    ui->lineEditName->setFocus();
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());
    setAttribute(Qt::WA_TranslucentBackground);
    ui->pushButton->setAutoDefault(false);
    ui->pushButton->setDefault(false);
    connect(ui->lineEditPwd, &QLineEdit::returnPressed, this, &Login::pushButton_clicked);
    connect(ui->lineEditName, &QLineEdit::returnPressed, this, &Login::lineEditName_returnPressed);
    connect(ui->pushButton, &QPushButton::clicked, this, &Login::pushButton_clicked);
    connect(ui->toolButton, &QPushButton::clicked, this, [this]() { this->done(QDialog::Rejected); });
}

Login::~Login() {
    delete ui;
}

void Login::pushButton_clicked() {
    std::optional<DBInstance *> instance = DBInstance::getInstance(ui->lineEditName->text(), ui->lineEditPwd->text());
    if (!(*instance)->isOpen()) {
        return;
    }
    submit((*instance)->db);
}

void Login::lineEditName_returnPressed() {
    this->focusNextChild();
    std::optional<DBInstance *> instance = DBInstance::getInstanceByName(ui->lineEditName->text());
    if (instance == std::nullopt) {
        return;
    }
    if (!(*instance)->isOpen()) {
        return;
    }
    submit((*instance)->db);
}

void Login::submit(QSqlDatabase &db) {
    emit loginRes(db);
    done(QDialog::Accepted);
}
