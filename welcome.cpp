#include "welcome.h"
#include "labeling.h"
#include "login.h"
#include "search.h"
#include "typeeditmenu.h"
#include "ui_welcome.h"
#include <QString>

QString imgBase = "D:/Z/Pictures/";

Welcome::Welcome(QWidget *parent) : QMainWindow(parent),
                                    ui(new Ui::Welcome) {
    ui->setupUi(this);
    QPixmap *pixmap = new QPixmap(":/pic/back.jpg");
    pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->label->setScaledContents(true);
    ui->label->setPixmap(*pixmap);
    ui->lineEdit->setText(imgBase);
    connect(ui->labelingButton, &QPushButton::clicked, this, &Welcome::labelingButton_clicked);
    connect(ui->searchButton, &QPushButton::clicked, this, &Welcome::searchButton_clicked);
    connect(ui->typeEditButton, &QPushButton::clicked, this, &Welcome::typeEditButton_clicked);
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, &Welcome::lineEditUpdate);
}

Welcome::~Welcome() {
    delete ui;
}

void Welcome::labelingButton_clicked() {
    auto newWindow = new LabelWindow;
    newWindow->show();
}

void Welcome::searchButton_clicked() {
    auto newWindow = new Search;
    newWindow->show();
}

void Welcome::typeEditButton_clicked() {
    auto loginWindow = new login(this);
    typeEditMenu *newWindow = nullptr;
    connect(loginWindow, &login::loginRes, this, [&newWindow](QSqlDatabase &db) {
        newWindow = new typeEditMenu(nullptr, db);
    });
    auto res = loginWindow->exec();
    if (res == QDialog::Accepted) {
        newWindow->show();
    }
}

void Welcome::lineEditUpdate() {
    imgBase = ui->lineEdit->text();
}
