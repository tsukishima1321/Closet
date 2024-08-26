#include "search.h"
#include "detailview.h"
#include "flowlayout.h"
#include "imagepreviewform.h"
#include "qimagereader.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "ui_search.h"
#include <QFile>
#include <QMessageBox>
#include <QRegularExpression>

extern QString imgBase;

Search::Search(QWidget *parent, QSqlDatabase &db) :
        QMainWindow(parent),
        ui(new Ui::Search),
        currentPage(1),
        db(db) {
    ui->setupUi(this);
    ui->deleteButton->setIcon(QIcon(":/pic/trash.png"));
    ui->checkBoxTitle->setChecked(true);
    ui->checkBoxAsc->setChecked(true);
    ui->dateEditFrom->setDate(QDate::currentDate());
    ui->dateEditTo->setDate(QDate::currentDate());
    flowLayout = new FlowLayout;
    ui->scrollAreaWidgetContents->setLayout(flowLayout);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    ui->comboBoxType->clear();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableWidget->setRowCount(pageSizeTable);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableWidget_2->setRowCount(100);
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
    connect(ui->pageNavigate, &PageNavigator::currentPageChanged, this, [this](int p) {
        if (p != currentPage) {
            currentPage = p;
            updateSearch();
        }
        currentPage = p;
    });
    /*connect(ui->checkBoxTitle, &QCheckBox::stateChanged, this, [this](int stat) {
        if (stat == Qt::CheckState::Checked) {
            ui->checkBoxText->blockSignals(true);
            ui->checkBoxText->setCheckState(Qt::CheckState::Unchecked);
            ui->checkBoxText->blockSignals(false);
        }
    });
    connect(ui->checkBoxText, &QCheckBox::stateChanged, this, [this](int stat) {
        if (stat == Qt::CheckState::Checked) {
            ui->checkBoxTitle->blockSignals(true);
            ui->checkBoxTitle->setCheckState(Qt::CheckState::Unchecked);
            ui->checkBoxTitle->blockSignals(false);
        }
    });*/
    connect(ui->checkBoxAsc, &QCheckBox::stateChanged, this, [this](int stat) {
        if (stat == Qt::CheckState::Checked) {
            ui->checkBoxDesc->blockSignals(true);
            ui->checkBoxDesc->setCheckState(Qt::CheckState::Unchecked);
            ui->checkBoxDesc->blockSignals(false);
        }
        updateSearch();
    });
    connect(ui->checkBoxDesc, &QCheckBox::stateChanged, this, [this](int stat) {
        if (stat == Qt::CheckState::Checked) {
            ui->checkBoxAsc->blockSignals(true);
            ui->checkBoxAsc->setCheckState(Qt::CheckState::Unchecked);
            ui->checkBoxAsc->blockSignals(false);
        }
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
        updateSearch();
    });
    connect(ui->comboBoxOrder, &QComboBox::currentIndexChanged, this, [this](int i) {
        (void)i;
        updateSearch();
    });
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &Search::tableCellDoubleClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &Search::deleteButton_clicked);
    connect(ui->sendSQLButton, &QPushButton::clicked, this, &Search::sendSQL);
}

void Search::searchButton_clicked() {
    ui->statusbar->showMessage("正在搜索...");
    QString sql = "";

    QStringList conditions;

    if (ui->lineEdit->text() == "") {
    } else {
        if (ui->checkBoxTitle->isChecked() && !ui->checkBoxText->isChecked()) {
            conditions.append("match(description) against('" + ui->lineEdit->text() + "')");
        } else if (ui->checkBoxTitle->isChecked() && ui->checkBoxText->isChecked()) {
            conditions.append("match(description) against('" + ui->lineEdit->text() + "') or match(ocr_result) against('" + ui->lineEdit->text() + "')");
        } else if (!ui->checkBoxTitle->isChecked() && ui->checkBoxText->isChecked()) {
            conditions.append("match(ocr_result) against('" + ui->lineEdit->text() + "')");
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
            sql += " " + c + " and";
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
        ui->pageNavigate->setMaxPage(query.value("count(*)").toInt() / pageSize + 1);
    }
    currentConditon = sql;
    updateSearch();
}

void Search::updateSearch() {
    if (ui->stackedWidget->currentIndex() == 0) {
        while (QLayoutItem *item = flowLayout->takeAt(0)) {
            if (QWidget *widget = item->widget())
                widget->deleteLater();
            delete item;
        }
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
        if (ui->checkBoxAsc->checkState() == Qt::Checked) {
            sql += "asc ";
        } else {
            sql += "desc ";
        }
        sql += "limit " + QString::number((currentPage - 1) * pageSize) + "," + QString::number((currentPage)*pageSize) + " ";
        QSqlQuery query(db);
        if (ui->checkBoxText->isChecked()) {
            query.prepare("select * from pictures_ocr join pictures on pictures.href=pictures_ocr.href " + sql);
        } else {
            query.prepare("select * from pictures " + sql);
        }

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
            while (query.next()) {
                addImgItem(query.value("href").toString(), query.value("description").toString());
            }
        }
    } else if (ui->stackedWidget->currentIndex() == 1) {
        ui->tableWidget->clear();
        QStringList headers;
        headers << "文件名"
                << "日期"
                << "描述"
                << "分类";
        ui->tableWidget->setHorizontalHeaderLabels(headers);
        while (QLayoutItem *item = flowLayout->takeAt(0)) {
            if (QWidget *widget = item->widget())
                widget->deleteLater();
            delete item;
        }
        QString sql = currentConditon;
        sql += "limit " + QString::number((currentPage - 1) * pageSizeTable) + "," + QString::number((currentPage)*pageSizeTable) + " ";
        QSqlQuery query(db);
        if (ui->checkBoxText->isChecked()) {
            query.prepare("select * from pictures_ocr join pictures on pictures.href=pictures_ocr.href " + sql);
        } else {
            query.prepare("select * from pictures " + sql);
        }

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
            int i = 0;
            ui->tableWidget->clearContents();
            while (query.next()) {
                auto check = new QTableWidgetItem(query.value("href").toString());
                check->setCheckState(Qt::CheckState::Unchecked);
                ui->tableWidget->setItem(i, 0, check);
                ui->tableWidget->setItem(i, 1, new QTableWidgetItem(query.value("date").toString()));
                ui->tableWidget->setItem(i, 2, new QTableWidgetItem(query.value("description").toString()));
                ui->tableWidget->setItem(i, 3, new QTableWidgetItem(query.value("type").toString()));
                i++;
            }
        }
    }
}

void Search::tableCellDoubleClicked(int row, int column) {
    (void)column;
    QString name = ui->tableWidget->item(row, 0)->text();
    if (name != "") {
        auto newWindow = new DetailView(nullptr, db);
        newWindow->OpenImg(name);
        newWindow->show();
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
                                               "确定删除" + item->text() + "吗？",
                                               QMessageBox::Yes | QMessageBox::Cancel,
                                               QMessageBox::Cancel);
                if (res == QMessageBox::Yes) {
                    query.bindValue(":href", item->text());
                    query.exec();
                }
            }
        }
    }
}

void Search::addImgItem(QString href, QString des) {
    imagePreviewForm *form = new imagePreviewForm(ui->scrollArea);
    flowLayout->addWidget(form);
    QString filename(imgBase + href);
    QImageReader reader(filename.simplified());
    reader.setDecideFormatFromContent(true);
    QSize size = reader.size();
    if (size.width() > 4000) {
        size.setWidth(size.width() / 8);
        size.setHeight(size.height() / 8);
    } else if (size.width() > 1000) {
        size.setWidth(size.width() / 4);
        size.setHeight(size.height() / 4);
    }
    reader.setScaledSize(size);
    std::shared_ptr<QImage> img(new QImage(reader.read()));
    if (img->isNull()) {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << file.errorString();
            QMessageBox::warning(this, QString("提示"), QString("打开图片文件失败！%1").arg(filename));
        }
        img->loadFromData(file.readAll());
        if (img->isNull()) {
            QMessageBox::information(this,
                                     tr("打开图像失败"),
                                     tr("打开图像失败") + href);
        }
    }
    form->setImg(href, img, des);
    connect(form, &imagePreviewForm::isClicked, this, &Search::openDetailMenu);
}

void Search::openDetailMenu(QString href) {
    auto newWindow = new DetailView(nullptr, db);
    newWindow->OpenImg(href);
    newWindow->show();
}

void Search::sendSQL() {
    QString sql = ui->lineEdit_2->text();
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
    }
    ui->tableWidget_2->clear();
    QStringList headers;
    headers << "文件名"
            << "日期"
            << "描述"
            << "分类";
    ui->tableWidget_2->setHorizontalHeaderLabels(headers);
    int i = 0;
    while (query.next()) {
        if (i > ui->tableWidget_2->rowCount()) {
            ui->tableWidget_2->insertRow(i);
        }
        ui->tableWidget_2->setItem(i, 0, new QTableWidgetItem(query.value("href").toString()));
        ui->tableWidget_2->setItem(i, 1, new QTableWidgetItem(query.value("date").toString()));
        ui->tableWidget_2->setItem(i, 2, new QTableWidgetItem(query.value("description").toString()));
        ui->tableWidget_2->setItem(i, 3, new QTableWidgetItem(query.value("type").toString()));
        i++;
    }
}

Search::~Search() {
    delete ui;
}
