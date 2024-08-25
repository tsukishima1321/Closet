#include "welcome.h"
#include "dbinstance.h"
#include "labeling.h"
#include "login.h"
#include "qmessagebox.h"
#include "qsqlquery.h"
#include "search.h"
#include "typeeditmenu.h"
#include "ui_welcome.h"
#include <QString>

QString imgBase = "D:/Z/Pictures/";

Welcome::Welcome(QWidget *parent) :
        QMainWindow(parent),
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
    connect(ui->buildButton, &QPushButton::clicked, this, &Welcome::buildHtmButton_clicked);
    connect(ui->logInButton, &QPushButton::clicked, this, &Welcome::logInButton_clicked);
    connect(ui->logOutButton, &QPushButton::clicked, this, &Welcome::logOutButton_clicked);
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
    std::optional<dbInstance *> instance = dbInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new login(this);
        typeEditMenu *newWindow = nullptr;
        connect(loginWindow, &login::loginRes, this, [&newWindow](QSqlDatabase &db) { newWindow = new typeEditMenu(nullptr, db); });
        auto res = loginWindow->exec();
        //exec阻塞，保证lambda被触发时newWindow未出作用域
        if (res == QDialog::Accepted) {
            newWindow->show();
        }
    } else {
        auto newWindow = new typeEditMenu(nullptr, (*instance)->db);
        newWindow->show();
    }
}

void Welcome::buildHtmButton_clicked() {
    std::optional<dbInstance *> instance = dbInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new login(this);
        connect(loginWindow, &login::loginRes, this, [this](QSqlDatabase &db) {
            buildHtm(db);
        });
        auto res = loginWindow->exec();
    } else {
        buildHtm((*instance)->db);
    }
}

void Welcome::buildHtm(QSqlDatabase &db) {
    QFile file(imgBase + "图片索引.htm");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString text = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n<html>\n<head>\n<title>图片索引</title>\n<meta name=\"GENERATOR\" content=\"WinCHM\">\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=gb2312\">\n<style>\nhtml,body { \n	font-family: Arial, Helvetica, sans-serif;\n	font-size: 11pt;\n}\n</style>\n</head>\n<body>\n";
        file.write(text.toLocal8Bit().data());
        QList<QString> typeList;
        QString sqlType = "SELECT * FROM types";
        QSqlQuery queryType(db);
        queryType.prepare(sqlType);
        queryType.exec();
        while (queryType.next()) {
            QString type = queryType.value("typename").toString();
            typeList.append(type);
        }
        //QString sql = "SELECT * FROM pictures WHERE type=:type ORDER BY date ASC,description ASC";
        QString sql = "SELECT * FROM pictures WHERE type=:type ORDER BY date ASC";
        QSqlQuery query(db);
        query.prepare(sql);

        for (const QString &type : typeList) {
            text = "<P>\n<FONT size=3><STRONG>" + type + "</STRONG></FONT>\n</P>\n<P>\n";
            file.write(text.toLocal8Bit().data());
            query.bindValue(":type", type);
            query.exec();
            while (query.next()) {
                text = "<A href=\"" + query.value("href").toString() + "\">" + query.value("description").toString() + "</A>&nbsp;——" + query.value("date").toString() + "<BR>" + "\n";
                file.write(text.toLocal8Bit().data());
            }
            text = "</P>\n";
            file.write(text.toLocal8Bit().data());
        }
        text = "</body>\n</html>";
        file.write(text.toLocal8Bit().data());
        file.close();
        QMessageBox::information(this, "生成", "生成完成", QMessageBox::Ok);
    } else {
        QMessageBox::warning(this, "错误", "文件打开失败");
    }
}

void Welcome::logInButton_clicked() {
    auto logInWindow = new login(this);
    logInWindow->exec();
}

void Welcome::logOutButton_clicked() {
    std::optional<dbInstance *> instance = dbInstance::getInstance();
    if (instance == std::nullopt) {
        QMessageBox::warning(this, "登出", "当前未以任何用户登录");
    } else {
        (*instance)->close();
        QMessageBox::information(this, "登出", "已登出用户：" + (*instance)->db.userName());
    }
}

void Welcome::lineEditUpdate() {
    imgBase = ui->lineEdit->text();
}
