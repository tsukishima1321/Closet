#include "welcome.h"
#include "dbinstance.h"
#include "iconresources.h"
#include "labeling.h"
#include "login.h"
#include "qsystemtrayicon.h"
#include "search.h"
#include "typeeditmenu.h"
#include "ui_welcome.h"
#include <QMessageBox>
#include <QSettings>
#include <QSqlQuery>
#include <QString>

QString imgBase = "D:/Z/Pictures/";

Welcome::Welcome(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::Welcome) {
    ui->setupUi(this);
    QPixmap *pixmap = new QPixmap(":/pic/backgrounds/back.jpg");
    pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->label->setScaledContents(true);
    ui->label->setPixmap(*pixmap);
    if (!QFileInfo("config.ini").isFile()) {
        QFile file("config.ini");
        file.open(QIODevice::Append);
        file.close();
        QSettings setting("config.ini", QSettings::IniFormat);
        setting.setValue("resource/imgBase", imgBase);
    }
    QSettings setting("config.ini", QSettings::IniFormat);
    imgBase = setting.value("resource/imgBase").toString();
    //ui->lineEdit->setText(imgBase);
    //ui->lineEdit->hide();
    connect(ui->labelingButton, &QPushButton::clicked, this, &Welcome::labelingButton_clicked);
    connect(ui->searchButton, &QPushButton::clicked, this, &Welcome::searchButton_clicked);
    connect(ui->typeEditButton, &QPushButton::clicked, this, &Welcome::typeEditButton_clicked);
    connect(ui->buildButton, &QPushButton::clicked, this, &Welcome::buildHtmButton_clicked);
    connect(ui->logInButton, &QPushButton::clicked, this, &Welcome::logInButton_clicked);
    connect(ui->logOutButton, &QPushButton::clicked, this, &Welcome::logOutButton_clicked);
    connect(ui->lineEdit, &QLineEdit::editingFinished, this, &Welcome::lineEditUpdate);

    QMenu *menu = new QMenu(this);

    QAction *min = new QAction("最小化", this);
    connect(min, &QAction::triggered, this, &QMainWindow::hide);
    QAction *restore = new QAction("恢复", this);
    connect(restore, &QAction::triggered, this, [this]() {show();raise();activateWindow(); });
    QAction *quit = new QAction("退出", this);
    connect(quit, &QAction::triggered, QCoreApplication::instance(), &QApplication::quit);

    menu->addAction(min);
    menu->addAction(restore);
    menu->addSeparator();
    menu->addAction(quit);

    tray = new QSystemTrayIcon(this);
    tray->setContextMenu(menu);
    tray->setIcon(IconResources::getIcons()["main-icon"]);
    tray->setToolTip("静寂的壁橱");
    connect(tray, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {if(reason==QSystemTrayIcon::DoubleClick){show();raise();activateWindow();} });
    tray->show();
}

void Welcome::closeEvent(QCloseEvent *event) {
    QMessageBox box(QMessageBox::Information, "关闭", "是否最小化到托盘");
    QAbstractButton *yesButton = box.addButton("最小化", QMessageBox::AcceptRole);
    box.addButton("退出", QMessageBox::RejectRole);
    box.exec();
    if (box.clickedButton() == yesButton) {
        this->hide();
        tray->showMessage("静寂的壁橱", "已最小化到托盘");
        event->ignore();
    } else {
        event->accept();
    }
}

Welcome::~Welcome() {
    delete ui;
}

void Welcome::labelingButton_clicked() {
    auto newWindow = new LabelWindow;
    newWindow->show();
}

void Welcome::searchButton_clicked() {
    std::optional<dbInstance *> instance = dbInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new login(this);
        connect(loginWindow, &login::loginRes, this, [this](QSqlDatabase &db) {
            Search *newWindow = new Search(this, db);
            newWindow->show();
        });
        loginWindow->exec();
    } else {
        auto newWindow = new Search(nullptr, (*instance)->db);
        newWindow->show();
    }
}

void Welcome::typeEditButton_clicked() {
    std::optional<dbInstance *> instance = dbInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new login(this);
        connect(loginWindow, &login::loginRes, this, [this](QSqlDatabase &db) {
            typeEditMenu *newWindow = new typeEditMenu(this, db);
            QMainWindow *window = new QMainWindow(this);
            window->setCentralWidget(newWindow);
            window->setWindowTitle("编辑分类");
            window->show();
        });
        loginWindow->exec();
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
        loginWindow->exec();
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
    auto res = logInWindow->exec();
    if (res == QDialog::Accepted) {
        ui->lineEdit->setText(imgBase);
    }
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
    std::optional<dbInstance *> instance = dbInstance::getInstance();
    if (instance == std::nullopt) {
        QMessageBox::warning(this, "拒绝访问", "请先登录");
    } else {
        imgBase = ui->lineEdit->text();
        QSettings setting("config.ini", QSettings::IniFormat);
        setting.setValue("resource/imgBase", imgBase);
        QMessageBox::information(this, "修改", "已将图片库地址修改为：" + imgBase);
    }
}
