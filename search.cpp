#include "search.h"
#include "detailview.h"
#include "imagepreviewform.h"
#include "qfiledialog.h"
#include "qimagereader.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_search.h"
#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QThreadPool>

extern QString imgBase;

Search::Search(QWidget *parent, QSqlDatabase &db) :
        Window(parent),
        ui(new Ui::Search),
        currentPage(1),
        currentColumnCount(3),
        pageSizeTable(TableWidget::getPageSize()),
        db(db) {
    ui->setupUi(this);
    ui->deleteButton->setIcon(QIcon(":/pic/trash.png"));
    ui->exportButton->setIcon(QIcon(":/pic/download.png"));
    ui->checkBoxTitle->setChecked(true);
    ui->radioButtonAsc->setChecked(true);
    ui->dateEditFrom->setDate(QDate::currentDate());
    ui->dateEditTo->setDate(QDate::currentDate());
    ui->tableWidget->resetHeader();
    hBoxLayout = new QHBoxLayout;
    for (int i = 0; i < currentColumnCount; i++) {
        QVBoxLayout *column = new QVBoxLayout;
        column->setAlignment(Qt::AlignTop);
        hBoxLayout->addLayout(column);
        vBoxLayouts.append(column);
    }
    ui->scrollAreaWidgetContents->setLayout(hBoxLayout);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->comboBoxType->clear();
    // qDebug()<<"database connected";
    QString sql = "SELECT * FROM types";
    QSqlQuery query(db);
    query.prepare(sql);
    query.exec();
    while (query.next()) {
        QString type = query.value("typename").toString();
        ui->comboBoxType->addItem(type);
        // qDebug()<<type;
    }
    currentPage = ui->pageNavigate->getCurrentPage();
    preViewList = new imagePreviewForm[pageSize];
    for (int i = 0; i < pageSize; i++) {
        imagePreviewForm *form = &preViewList[i];
        connect(form, &imagePreviewForm::isClicked, this, &Search::openDetailMenu);
    }
    connect(ui->pageNavigate, &PageNavigator::currentPageChanged, this, [this](int p) {
        if (p != currentPage) {
            currentPage = p;
            updateSearch();
        }
        currentPage = p;
    });
    connect(ui->radioButtonAsc, &QRadioButton::clicked, this, [this]() {
        updateSearch();
    });
    connect(ui->radioButtonDesc, &QRadioButton::clicked, this, [this]() {
        updateSearch();
    });
    connect(ui->checkBoxType, &QCheckBox::stateChanged, this, [this](int stat) {
        if (stat == Qt::CheckState::Checked) {
            ui->comboBoxType->setEnabled(true);
        } else {
            ui->comboBoxType->setEnabled(false);
        }
    });
    connect(ui->checkBoxDate, &QCheckBox::stateChanged, this, [this](int stat) {
        if (stat == Qt::CheckState::Checked) {
            ui->dateEditFrom->setEnabled(true);
            ui->dateEditTo->setEnabled(true);
        } else {
            ui->dateEditFrom->setEnabled(false);
            ui->dateEditTo->setEnabled(false);
        }
    });
    connect(ui->searchButton, &QPushButton::clicked, this, &Search::searchButton_clicked);
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &Search::searchButton_clicked);
    connect(ui->comboBoxShow, &QComboBox::currentIndexChanged, this, [this](int i) {
        if (i == 0) {
            ui->stackedWidget->setCurrentIndex(0);
            ui->pageNavigate->setCurrentPage(1);
        } else if (i == 1) {
            ui->stackedWidget->setCurrentIndex(1);
            ui->pageNavigate->setCurrentPage(1);
        }
        searchButton_clicked();
    });
    connect(ui->comboBoxOrder, &QComboBox::currentIndexChanged, this, [this](int i) {
        (void)i;
        updateSearch();
    });
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &Search::tableCellDoubleClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &Search::deleteButton_clicked);
    connect(ui->exportButton, &QPushButton::clicked, this, &Search::exportButton_clicked);
    connect(ui->sendSQLButton, &QPushButton::clicked, this, [this]() { this->sendSQL(ui->lineEdit_2->text()); });
}

void Search::searchButton_clicked() {
    /*构造查询的条件，预先查询结果的总数来更新页码，最后调用updateSearch()*/
    //ui->statusbar->showMessage("正在搜索...");
    QString sql = "";

    QStringList conditions;

    if (ui->lineEdit->text() == "") {
    } else {
        if (ui->checkBoxTitle->isChecked() && !ui->checkBoxText->isChecked()) {
            conditions.append("match(description) against('" + ui->lineEdit->text() + +"'in boolean mode)");
        } else if (ui->checkBoxTitle->isChecked() && ui->checkBoxText->isChecked()) {
            conditions.append("match(description) against('" + ui->lineEdit->text() + "'in boolean mode) or match(ocr_result) against('" + ui->lineEdit->text() + "'in boolean mode)");
        } else if (!ui->checkBoxTitle->isChecked() && ui->checkBoxText->isChecked()) {
            conditions.append("match(ocr_result) against('" + ui->lineEdit->text() + "'in boolean mode)");
        }
    }

    if (ui->comboBoxType->isEnabled()) {
        conditions.append("type='" + ui->comboBoxType->currentText() + "'");
    }

    if (ui->dateEditFrom->isEnabled()) {
        conditions.append("date between '" + ui->dateEditFrom->text() + "' and '" + ui->dateEditTo->text() + "'");
    }

    if (!conditions.empty()) {
        sql += "where";
        for (auto &&c : conditions) {
            sql += " (" + c + ") and";
        }
        if (sql.right(3) == "and")
            sql = sql.remove(sql.length() - 3, 3);
    }
    sql += " ";

    //qDebug() << sql;
    QSqlQuery query(db);
    if (ui->checkBoxText->isChecked()) {
        query.prepare("select count(*) from pictures_ocr join pictures on pictures.href=pictures_ocr.href " + sql);
    } else {
        query.prepare("select count(*) from pictures " + sql);
    }
    query.exec();
    if (!query.exec()) {
        QSqlError sqlerror = query.lastError();
        qDebug() << sqlerror.nativeErrorCode();
        QString errortext = sqlerror.text();
        qDebug() << errortext;
        if (errortext == "") {
            errortext = "empty query";
        }
        QMessageBox::critical(this, "错误", errortext);
    } else {
        query.next();
        if (ui->comboBoxShow->currentIndex() == 0) {
            ui->pageNavigate->setMaxPage(query.value("count(*)").toInt() / pageSize + 1);
            ui->pageNavigate->setCurrentPage(1, true);
            currentPage = 1;
        } else {
            ui->pageNavigate->setMaxPage(query.value("count(*)").toInt() / pageSizeTable + 1);
            ui->pageNavigate->setCurrentPage(1, true);
            currentPage = 1;
        }
    }
    currentConditon = sql;
    updateSearch();
}

void Search::updateSearch() {
    /*接受查询条件，在此之上构造排序和分页条件，进行实际查询，根据radioButtn状态调用updateImgView()或updateTableView()
      除了被searchButtonClicked调用外，不改变查询条件，只改变排序和分类的操作最后也会调用此函数来显示结果*/
    QString sql = currentConditon;
    switch (ui->comboBoxOrder->currentIndex()) {
    case 0:
        sql += "order by pictures.href ";
        break;
    case 1:
        sql += "order by pictures.date ";
        break;
    case 2:
        sql += "order by pictures.description ";
        break;
    default:
        sql += "order by pictures.href ";
        break;
    }
    if (ui->radioButtonAsc->isChecked()) {
        sql += "asc ";
    } else {
        sql += "desc ";
    }
    if (ui->stackedWidget->currentIndex() == 0) {
        sql += "limit " + QString::number((currentPage - 1) * pageSize) + "," + QString::number(pageSize) + " ";
    } else if (ui->stackedWidget->currentIndex() == 1) {
        sql += "limit " + QString::number((currentPage - 1) * pageSizeTable) + "," + QString::number(pageSizeTable) + " ";
    }
    QSqlQuery query(db);
    if (ui->checkBoxText->isChecked()) {
        query.prepare("select * from pictures_ocr join pictures on pictures.href=pictures_ocr.href " + sql);
    } else {
        query.prepare("select * from pictures " + sql);
    }
    qDebug() << sql;
    if (!query.exec()) {
        QSqlError sqlerror = query.lastError();
        qDebug() << sqlerror.nativeErrorCode();
        QString errortext = sqlerror.text();
        qDebug() << errortext;
        if (errortext == "") {
            errortext = "empty query";
        }
        QMessageBox::critical(this, "错误", errortext);
    } else {
        if (ui->stackedWidget->currentIndex() == 0) {
            updateImgView(query);
        } else if (ui->stackedWidget->currentIndex() == 1) {
            updateTableView(query);
        }
    }
}

void Search::updateImgView(QSqlQuery &query) {
    //重置preViewList中所有Form到可用状态
    for (int i = 0; i < pageSize; i++) {
        imagePreviewForm *form = &preViewList[i];
        form->~imagePreviewForm();
        new (form) imagePreviewForm;
        connect(form, &imagePreviewForm::isClicked, this, &Search::openDetailMenu);
        //form->hideElements();
    }
    //执行查询，将结果存入preViewList
    //图片大小和文字信息全部同步设置完毕，实际图片读取放入Thread中
    while (query.next()) {
        addImgItem(query.value("href").toString(), query.value("description").toString());
    }
    //清空子布局
    for (QVBoxLayout *column : vBoxLayouts) {
        for (int i = column->count() - 1; i >= 0; i--) {
            column->removeItem(column->itemAt(i));
        }
    }
    //将preViewList中的窗口排入布局
    locateImg();
}

void Search::locateImg() {
    /*读取preViewList中的所有窗口，根据他们的高度将他们排入布局
    调用该函数前所有子布局应该已被创建好且是空的
    要显示的图片发生改变，或者窗口大小发生改变时会调用此函数*/
    for (int i = 0; i < pageSize; i++) {
        imagePreviewForm *form = &preViewList[i];
        QVBoxLayout *columnMinHeight = nullptr;
        int minHeight = 999999;
        for (QVBoxLayout *column : vBoxLayouts) {
            int height = 0;
            for (int i = column->count() - 1; i >= 0; i--) {
                height += dynamic_cast<imagePreviewForm *>(column->itemAt(i)->widget())->getHeight();
            }
            if (height < minHeight) {
                //qDebug() << height;
                columnMinHeight = column;
                minHeight = height;
            }
        }
        if (columnMinHeight) {
            columnMinHeight->addWidget(form);
        }
    }
}

void Search::updateTableView(QSqlQuery &query) {
    ui->tableWidget->fillItems(query, (currentPage - 1) * pageSizeTable);
}

void Search::tableCellDoubleClicked(int row, int column) {
    (void)column;
    auto item = ui->tableWidget->item(row, 1);
    if (item) {
        QString name = item->text();
        if (name != "") {
            openDetailMenu(name);
        }
    }
}

void Search::deleteButton_clicked() {
    QSqlQuery query(db);
    query.prepare("delete from pictures where href=:href;");
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        QTableWidgetItem *item = ui->tableWidget->item(i, 0);
        if (item != NULL) {
            if (item->checkState() == Qt::CheckState::Checked) {
                int res = QMessageBox::warning(this, "确认",
                                               "确定删除" + ui->tableWidget->item(i, 1)->text() + "吗？",
                                               QMessageBox::Yes | QMessageBox::Cancel,
                                               QMessageBox::Cancel);
                if (res == QMessageBox::Yes) {
                    query.bindValue(":href", ui->tableWidget->item(i, 1)->text());
                    query.exec();
                }
            }
        }
    }
}

void Search::exportButton_clicked() {
    QString targetDirPath = "";
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    if (dialog.exec() == QDialog::Accepted) {
        auto files = dialog.selectedFiles();
        if (files.size() >= 0) {
            targetDirPath = files.at(0) + "/";
        }
    }
    if (targetDirPath == "") {
        return;
    }
    int success = 0;
    int fail = 0;
    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        QTableWidgetItem *item = ui->tableWidget->item(i, 0);
        if (item != NULL) {
            if (item->checkState() == Qt::CheckState::Checked) {
                QString current = ui->tableWidget->item(i, 1)->text().simplified();
                if (QFile::copy(imgBase + current, targetDirPath + current)) {
                    success++;
                } else {
                    fail++;
                    qDebug() << current;
                }
            }
        }
    }
    QMessageBox::information(this, "导出", "导出完成\n成功：" + QString::number(success) + "，失败：" + QString::number(fail));
}

imgLoader::imgLoader(imagePreviewForm *target, QImageReader *reader, QString href, QString des) :
        target(target), reader(reader), href(href), des(des) {}

void imgLoader::run() {
    QImage *img = new QImage(reader->read());
    if (img->isNull()) {
        QMessageBox::information(nullptr,
                                 "打开图像失败",
                                 "打开图像失败" + reader->fileName());
    }
    target->setImg(href, img, des);
    emit loadReady();
    delete reader;
}

imagePreviewForm *Search::addImgItem(QString href, QString des) {
    /*根据图片信息设置寻找preViewList中可用的imagePreviewForm放入，大小和文字同步设置，实际图片读取放入Thread中*/
    QString filename(imgBase + href);
    QImageReader *reader = new QImageReader(filename.simplified()); //该指针由imgLoader::run释放内存
    reader->setDecideFormatFromContent(true);
    QSize size = reader->size();
    if (size.width() > 4000) {
        size.setWidth(size.width() / 8);
        size.setHeight(size.height() / 8);
    } else if (size.width() > 1000) {
        size.setWidth(size.width() / 4);
        size.setHeight(size.height() / 4);
    }
    reader->setScaledSize(size);
    QImage *img = new QImage(reader->scaledSize().width(), reader->scaledSize().height(), QImage::Format_RGB888); //该指针由form->setImg传入对象内，由~imagePreViewForm释放内存
    img->fill(QColor(Qt::white));
    imagePreviewForm *form = nullptr;
    bool flag = false;
    for (int i = 0; i < pageSize; i++) {
        imagePreviewForm *form = &preViewList[i];
        if (form->isAvailable()) {
            form->setImg(href, img, des);
            imgLoader *loader = new imgLoader(form, reader, href, des); //该指针由QThreadPool释放内存
            connect(loader, &imgLoader::loadReady, this, [this]() { update(); });
            QThreadPool::globalInstance()->start(loader);
            flag = true;
            break;
        }
    }
    if (!flag) {
        qDebug() << "why?";
        throw;
    }
    return form;
}

void Search::openDetailMenu(QString href) {
    auto newWindow = new DetailView(nullptr, db);
    newWindow->show();
    newWindow->OpenImg(href);
}

void Search::sendSQL(QString text) {
    QString sql = text;
    QString warning = "";
    if (sql.contains("insert")) {
        warning += "insert ";
    }
    if (sql.contains("delete")) {
        warning += "delete ";
    }
    if (sql.contains("update")) {
        warning += "delete ";
    }
    if (warning != "") {
        QMessageBox message(QMessageBox::Warning, "警告", "含有危险操作:" + warning + "\n是否继续？", QMessageBox::Yes | QMessageBox::No, NULL);
        if (message.exec() != QMessageBox::Yes) {
            return;
        }
    }
    if ((sql.contains("delete") || sql.contains("update")) && !sql.contains("where")) {
        QMessageBox::critical(this, "错误", "操作不被允许");
        return;
    }
    QSqlQuery query(db);
    if (!query.exec(sql)) {
        QSqlError sqlerror = query.lastError();
        qDebug() << sqlerror.nativeErrorCode();
        QString errortext = sqlerror.text();
        qDebug() << errortext;
        if (errortext == "") {
            errortext = "empty query";
        }
        QMessageBox::critical(this, "错误", errortext);
    } else {
        ui->tableWidget_2->clear();
        ui->tableWidget_2->resetHeader();
        ui->tableWidget_2->fillItems(query, 0, query.size());
    }
}

void Search::resizeEvent(QResizeEvent *event) {
    (void)event;
    int width = ui->stackedWidget->width();
    currentColumnCount = width / 300;
    if (width == 622) {
        currentColumnCount = 3;
    }
    for (QVBoxLayout *column : vBoxLayouts) {
        for (int i = column->count() - 1; i >= 0; i--) {
            column->removeItem(column->itemAt(i));
        }
    }
    QLayoutItem *column;
    while ((column = hBoxLayout->itemAt(0))) {
        hBoxLayout->removeItem(column);
        delete column;
    }
    vBoxLayouts.clear();
    for (int i = 0; i < currentColumnCount; i++) {
        QVBoxLayout *column = new QVBoxLayout;
        column->setAlignment(Qt::AlignTop);
        hBoxLayout->addLayout(column);
        vBoxLayouts.append(column);
    }
    locateImg();
}

Search::~Search() {
    delete[] preViewList;
    if (hBoxLayout) {
        delete hBoxLayout;
    }
    delete ui;
}
