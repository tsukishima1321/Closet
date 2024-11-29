#include "welcome.h"
#include "ImgSearch.h"
#include "TextSearch.h"
#include "dbinstance.h"
#include "iconresources.h"
#include "labeling.h"
#include "login.h"
#include "textdetailview.h"
#include "typeeditmenu.h"
#include "ui_welcome.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QSettings>
#include <QSqlQuery>
#include <QString>
#include <QSystemTrayIcon>

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

    // init menubar
    QAction *actionExportHtm = new QAction("导出图片信息为htm", this);
    QAction *actionExportDB = new QAction("导出数据库", this);
    QAction *actionSetBaseDir = new QAction("设置图片库地址", this);
    QAction *actionEditTypes = new QAction("编辑分类", this);
    connect(actionExportHtm, &QAction::triggered, this, &Welcome::exportHtm);
    connect(actionExportDB, &QAction::triggered, this, &Welcome::exportDB);
    connect(actionSetBaseDir, &QAction::triggered, this, &Welcome::setBaseDir);
    connect(actionEditTypes, &QAction::triggered, this, &Welcome::typeEdit);
    QMenu *configMenu = new QMenu("设置", this);
    configMenu->addAction(actionSetBaseDir);
    configMenu->addAction(actionEditTypes);
    QMenu *exportMenu = new QMenu("导出", this);
    exportMenu->addAction(actionExportHtm);
    exportMenu->addAction(actionExportDB);
    ui->menubar->addMenu(configMenu);
    ui->menubar->addMenu(exportMenu);

    connect(ui->labelingButton, &QPushButton::clicked, this, &Welcome::labelingButton_clicked);
    connect(ui->searchButton, &QPushButton::clicked, this, &Welcome::searchButton_clicked);
    connect(ui->textButton, &QPushButton::clicked, this, &Welcome::textButton_clicked);
    connect(ui->logInButton, &QPushButton::clicked, this, &Welcome::logInButton_clicked);
    connect(ui->logOutButton, &QPushButton::clicked, this, &Welcome::logOutButton_clicked);

    QMenu *menu = new QMenu(this);

    QAction *quickWrite = new QAction("随手记", this);
    connect(quickWrite, &QAction::triggered, this, [this]() {
        std::optional<DBInstance *> instance = DBInstance::getInstanceByName("root");
        if (instance == std::nullopt) {
            auto loginWindow = new Login(this);
            connect(loginWindow, &Login::loginRes, this, [this](QSqlDatabase &db) {
                auto newWindow = new TextDetailView(this, db);
                newWindow->setDate(QDate::currentDate());
                newWindow->show();
            });
            loginWindow->exec();
        } else {
            auto newWindow = new TextDetailView(nullptr, (*instance)->db);
            newWindow->setDate(QDate::currentDate());
            newWindow->show();
        }
    });
    QAction *min = new QAction("最小化", this);
    connect(min, &QAction::triggered, this, &QMainWindow::hide);
    QAction *restore = new QAction("恢复", this);
    connect(restore, &QAction::triggered, this, [this]() {show();raise();activateWindow(); });
    QAction *quit = new QAction("退出", this);
    connect(quit, &QAction::triggered, QCoreApplication::instance(), &QApplication::quit);

    menu->addAction(quickWrite);
    menu->addSeparator();
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
        QApplication::instance()->quit();
        return QMainWindow::closeEvent(event);
    }
}

void Welcome::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    // test database connection
    std::optional<DBInstance *> instance = DBInstance::getInstance();
    if (instance == std::nullopt) {
        return;
    } else {
        if (!(*instance)->db.isOpen()) {
            QMessageBox::warning(this, "", "数据库连接超时，请重新登录");
            return;
        } else {
            QSqlQuery query((*instance)->db);
            query.exec("SELECT * FROM pictures LIMIT 1");
            if (!query.next()) {
                QMessageBox::warning(this, "", "数据库连接超时，请重新登录");
                return;
            }
        }
    }
    // QMessageBox::information(this, "欢迎", "欢迎使用静寂的壁橱");
}

Welcome::~Welcome() {
    delete ui;
}

void Welcome::labelingButton_clicked() {
    auto newWindow = new LabelWindow;
    newWindow->show();
}

void Welcome::searchButton_clicked() {
    std::optional<DBInstance *> instance = DBInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new Login(this);
        connect(loginWindow, &Login::loginRes, this, [this](QSqlDatabase &db) {
            ImgSearch *newWindow = new ImgSearch(this, db);
            newWindow->show();
        });
        loginWindow->exec();
    } else {
        auto newWindow = new ImgSearch(nullptr, (*instance)->db);
        newWindow->show();
    }
}

void Welcome::textButton_clicked() {
    std::optional<DBInstance *> instance = DBInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new Login(this);
        connect(loginWindow, &Login::loginRes, this, [this](QSqlDatabase &db) {
            TextSearch *newWindow = new TextSearch(this, db);
            newWindow->show();
        });
        loginWindow->exec();
    } else {
        auto newWindow = new TextSearch(nullptr, (*instance)->db);
        newWindow->show();
    }
}

void Welcome::typeEdit() {
    std::optional<DBInstance *> instance = DBInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new Login(this);
        connect(loginWindow, &Login::loginRes, this, [this](QSqlDatabase &db) {
            QMainWindow *window = new QMainWindow(this);
            TypeEditMenu *newWindow = new TypeEditMenu(window, db);
            window->setCentralWidget(newWindow);
            window->setWindowTitle("编辑分类");
            window->show();
        });
        loginWindow->exec();
    } else {
        auto newWindow = new TypeEditMenu(nullptr, (*instance)->db);
        newWindow->show();
    }
}

void Welcome::exportHtm() {
    std::optional<DBInstance *> instance = DBInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new Login(this);
        connect(loginWindow, &Login::loginRes, this, [this](QSqlDatabase &db) {
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
        // QString sql = "SELECT * FROM pictures WHERE type=:type ORDER BY date ASC,description ASC";
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

void Welcome::exportDB() {
    std::optional<DBInstance *> instance = DBInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new Login(this);
        connect(loginWindow, &Login::loginRes, this, [this](QSqlDatabase &db) {
            sqlDump(db);
        });
        loginWindow->exec();
    } else {
        sqlDump((*instance)->db);
    }
}

void Welcome::sqlDump(QSqlDatabase &db) {
    if (db.userName() != "root") {
        QMessageBox::warning(this, "拒绝访问", "请使用root用户登录");
        return;
    }
    QString path = QFileDialog::getSaveFileName(this, "保存数据库", "", "SQL files (*.sql)");
    if (path.isEmpty()) {
        return;
    }
    QProcess process;
    process.setStandardOutputFile(path);
    process.start("mysqldump", QStringList() << "-u" + db.userName() << "-p" + db.password() << "-c"
                                             << "--skip-extended-insert"
                                             << "--add-drop-table"
                                             << "diary");
    process.waitForFinished();
    QMessageBox::information(this, "导出", "导出完成", QMessageBox::Ok);
}

void Welcome::setBaseDir() {
    imgBase = QFileDialog::getExistingDirectory(this, "选择图片库地址", imgBase) + "/";
    QSettings setting("config.ini", QSettings::IniFormat);
    setting.setValue("resource/imgBase", imgBase);
    QMessageBox::information(this, "修改", "已将图片库地址修改为：" + imgBase);
}

void Welcome::logInButton_clicked() {
    auto logInWindow = new Login(this);
    auto res = logInWindow->exec();
    if (res == QDialog::Accepted) {
        // ui->lineEdit->setText(imgBase);
    }
}

void Welcome::logOutButton_clicked() {
    std::optional<DBInstance *> instance = DBInstance::getInstance();
    if (instance == std::nullopt) {
        QMessageBox::warning(this, "登出", "当前未以任何用户登录");
    } else {
        (*instance)->close();
        QMessageBox::information(this, "登出", "已登出用户：" + (*instance)->db.userName());
    }
}
