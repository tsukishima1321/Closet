#include "labeling.h"
#include "dbinstance.h"
#include "iconresources.h"
#include "labelcommit.h"
#include "login.h"
#include "typeeditmenu.h"
#include "ui_labeling.h"
#include <QDebug>
#include <QFile>
#include <QImage>
#include <QInputDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>

LabelWindow::LabelWindow(QWidget *parent) :
        Window(parent),
        ui(new Ui::LabelWindow),
        isLogedIn(false) {
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tabWidget->setTabText(0, "录入记录");
    ui->tabWidget->setTabText(1, "记录预览");
    ui->tabWidget->setTabText(2, "记录提交");
    ui->freshTypeButton->setIcon(IconResources::getIcons()["refresh"]);
    ui->freshDateButton->setIcon(IconResources::getIcons()["refresh"]);
    ui->toolButtonNext->setIcon(IconResources::getIcons()["step-forward"]);
    ui->toolButtonLast->setIcon(IconResources::getIcons()["step-back"]);
    ui->dateEdit->setDate(QDate::currentDate());
    //链接来自gui的信号
    connect(ui->imgToolBar, &ImgToolBar::zoomIn, ui->imageViewPort, &ImageView::slot_zoomIn);
    connect(ui->imgToolBar, &ImgToolBar::zoomOut, ui->imageViewPort, &ImageView::slot_zoomOut);
    connect(ui->imgToolBar, &ImgToolBar::reset, ui->imageViewPort, &ImageView::slot_reset);
    connect(ui->imgToolBar, &ImgToolBar::rotateLeft, ui->imageViewPort, &ImageView::slot_rotateLeft);
    connect(ui->imgToolBar, &ImgToolBar::rotateRight, ui->imageViewPort, &ImageView::slot_rotateRight);
    connect(ui->pushButtonAdd, &QPushButton::clicked, this, &LabelWindow::pushButtonAdd_clicked);
    connect(ui->toolButtonNext, &QPushButton::clicked, this, &::LabelWindow::pushButtonNext_clicked);
    connect(ui->toolButtonLast, &QPushButton::clicked, this, &::LabelWindow::pushButtonLast_clicked);
    connect(ui->lineEditPath, &QLineEdit::returnPressed, this, &LabelWindow::pushButtonStart_clicked);
    connect(ui->pushButtonFinish, &QPushButton::clicked, this, &LabelWindow::pushButtonFinish_clicked);
    connect(ui->pushButtonFinish_2, &QPushButton::clicked, this, &LabelWindow::pushButtonFinish_clicked);
    connect(ui->pushButtonAddType, &QPushButton::clicked, this, &LabelWindow::pushButtonAddType_clicked);
    connect(ui->freshTypeButton, &QPushButton::clicked, this, &LabelWindow::freshButton_clicked);
    connect(ui->freshDateButton, &QPushButton::clicked, this, &LabelWindow::freshDateButton_clicked);
    connect(ui->pushButtonDelete, &QPushButton::clicked, this, &LabelWindow::pushButtonDelete_clicked);
    connect(ui->imgToolBar, &ImgToolBar::setWheelMode, ui->imageViewPort, &ImageView::setWheelMode);
    connect(ui->tabWidget, &QTabWidget::tabBarClicked, this, [this](int i) {
        if (i == 2) {
            pushButtonFinish_clicked();
        } else if (i == 1) {
            updateTable();
        }
    });
    dbVisitor = QSqlDatabase::addDatabase("QMYSQL", "typeEdit");
    dbVisitor.setHostName("127.0.0.1");
    dbVisitor.setDatabaseName("diary");
    dbVisitor.setUserName("visitor");
    dbVisitor.setPassword("password");
    dbVisitor.setPort(3306);
    if (!dbVisitor.open()) {
        // qDebug()<<"Database Connect Failed";
        QMessageBox::critical(this, "ERROR", "Database Connection Failed");
        // delete this;
    }
    updateTypes();
    currentImgIndex = 0;
}

void LabelWindow::updateTypes() {
    //更新分类列表
    ui->comboBoxType->clear();
    // qDebug()<<"database connected";
    QString sql = "SELECT * FROM types";
    QSqlQuery query(dbVisitor);
    query.prepare(sql);
    query.exec();
    while (query.next()) {
        QString type = query.value("typename").toString();
        ui->comboBoxType->addItem(type);
        // qDebug()<<type;
    }
}

LabelWindow::~LabelWindow() { delete ui; }

void LabelWindow::pushButtonNext_clicked() {
    ui->checkBoxOCR->setChecked(true);
    currentImgIndex++;
    if (currentImgIndex < ImageCount) {
        QString ImageName = imagePath + dir[currentImgIndex];
        ui->imageViewPort->loadImage(ImageName);
        ui->labelName->setText(dir[currentImgIndex]);
    } else {
        currentImgIndex = ImageCount - 1;
    }
}

void LabelWindow::pushButtonLast_clicked() {
    ui->checkBoxOCR->setChecked(true);
    currentImgIndex--;
    if (currentImgIndex >= 0) {
        QString ImageName = imagePath + dir[currentImgIndex];
        ui->imageViewPort->loadImage(ImageName);
        ui->labelName->setText(dir[currentImgIndex]);
    } else {
        currentImgIndex = 0;
    }
}

void LabelWindow::pushButtonStart_clicked() {
    imagePath = ui->lineEditPath->text(); //文件夹路径
    if (imagePath.back() != '/') {
        imagePath += '/';
    }
    dir.setPath(imagePath);
    QStringList ImageList;
    ImageList << "*.bmp"
              << "*.jpg"
              << "*.png"
              << "*.jpeg";          //向字符串列表添加图片类型
    dir.setNameFilters(ImageList); //获得文件夹下图片的名字
    ImageCount = dir.count();      //获得dir里名字的个数，也表示文件夹下图片的个数
    if (ImageCount > 0) {
        QString ImageName = imagePath + dir[0];
        ui->imageViewPort->loadImage(ImageName);
        ui->imageViewPort->update();
        ui->labelName->setText(dir[0]);
    }
    currentImgIndex = 0;
}

void LabelWindow::updateTable() {
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(itemMap.count());
    int i = 0;
    for (const auto &itemTuple : (this->itemMap)) {
        const Item &item = std::get<0>(itemTuple);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(item.date));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(item.href));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(item.description));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(item.type));
        i++;
    }
}

void LabelWindow::pushButtonAdd_clicked() {
    if(ui->dateEdit->date() > QDate::currentDate()) {
        QMessageBox::warning(this, "错误", "日期不可大于当前日期");
        return;
    }
    QString date = ui->dateEdit->text();
    QString name = ui->labelName->text();
    QString des = ui->plainTextEdit->toPlainText();
    itemMap[name] = std::make_tuple(Item(date, name, des, ui->comboBoxType->currentText()),ui->checkBoxOCR->isChecked());
    this->pushButtonNext_clicked();
}

void LabelWindow::keyPressEvent(QKeyEvent *event) {
    if (event->matches(QKeySequence::Save)) {
        this->pushButtonAdd_clicked();
    }
    Window::keyPressEvent(event);
}

void LabelWindow::freshButton_clicked() { updateTypes(); }

void LabelWindow::freshDateButton_clicked() {
    ui->dateEdit->setDate(QDate::currentDate());
}

void LabelWindow::pushButtonDelete_clicked() {
    if (ui->tableWidget->rowCount() == 0) {
        return;
    }
    int i = ui->tableWidget->currentRow();
    itemMap.remove(ui->tableWidget->item(i, 1)->text());
    updateTable();
}

void LabelWindow::pushButtonFinish_clicked() {
    if (isLogedIn) {
        ui->tabWidget->setCurrentIndex(2);
        return;
    }
    std::optional<DBInstance *> instance = DBInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new Login(this);
        LabelCommit *newWindow = nullptr;
        connect(loginWindow, &Login::loginRes, this, [&newWindow, this](QSqlDatabase &db) { newWindow = new LabelCommit(nullptr, &itemMap, db, ui->lineEditPath->text()); });
        auto res = loginWindow->exec();
        if (res == QDialog::Accepted) {
            isLogedIn = true;
            connect(ui->tabWidget, &QTabWidget::tabBarClicked, newWindow, &LabelCommit::tabClicked);
            ui->tabWidget->removeTab(2);
            ui->tabWidget->addTab(newWindow, "记录提交");
            ui->tabWidget->setCurrentIndex(2);
        }
    } else {
        LabelCommit *newWindow = nullptr;
        isLogedIn = true;
        newWindow = new LabelCommit(nullptr, &itemMap, (*instance)->db, ui->lineEditPath->text());
        connect(ui->tabWidget, &QTabWidget::tabBarClicked, newWindow, &LabelCommit::tabClicked);
        ui->tabWidget->removeTab(2);
        ui->tabWidget->addTab(newWindow, "记录提交");
        ui->tabWidget->setCurrentIndex(2);
    }
}

void LabelWindow::pushButtonAddType_clicked() {
    std::optional<DBInstance *> instance = DBInstance::getInstanceByName("root");
    if (instance == std::nullopt) {
        auto loginWindow = new Login(this);
        TypeEditMenu *newWindow = nullptr;
        connect(loginWindow, &Login::loginRes, this, [&newWindow](QSqlDatabase &db) { newWindow = new TypeEditMenu(nullptr, db); });
        auto res = loginWindow->exec();
        if (res == QDialog::Accepted) {
            newWindow->show();
        }
    } else {
        auto newWindow = new TypeEditMenu(nullptr, (*instance)->db);
        newWindow->show();
    }
}

void LabelWindow::closeEvent(QCloseEvent *event) {
    if (isLogedIn) {
        if (dynamic_cast<LabelCommit *>(ui->tabWidget->widget(2))->isRunning()) {
            event->ignore();
            QMessageBox::information(this, "提示", "OCR进程运行中，请勿关闭窗口");
            return;
        }
    }
    return Window::closeEvent(event);
}
