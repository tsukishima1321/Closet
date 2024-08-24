#include "login.h"
#include "dbinstance.h"
#include "ui_login.h"

login::login(QWidget *parent) : QDialog(parent),
                                ui(new Ui::login) {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    ui->pushButton->setAutoDefault(false);
    ui->pushButton->setDefault(false);
    connect(ui->lineEditPwd, &QLineEdit::returnPressed, this, &login::pushButton_clicked);
    connect(ui->lineEditName, &QLineEdit::returnPressed, this, &login::lineEditName_returnPressed);
    connect(ui->pushButton, &QPushButton::clicked, this, &login::pushButton_clicked);
}

login::~login() {
    delete ui;
}

void login::pushButton_clicked() {
    dbInstance *instance = dbInstance::getInstance(ui->lineEditName->text(), ui->lineEditPwd->text());
    if (!instance->isOpen) {
        return;
    }
    submit(instance->db);
}

void login::lineEditName_returnPressed() {
    dbInstance *instance = dbInstance::getInstanceByName(ui->lineEditName->text());
    if (instance == nullptr) {
        this->focusPreviousChild();
        return;
    }
    if (!instance->isOpen) {
        this->focusPreviousChild();
        return;
    }
    submit(instance->db);
}

void login::submit(QSqlDatabase &db) {
    emit loginRes(db);
    done(QDialog::Accepted);
}
