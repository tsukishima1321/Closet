#include "search.h"
#include "detailview.h"
#include "imagepreviewform.h"
#include "qfiledialog.h"
#include "qimagereader.h"
#include "qsqlerror.h"
#include "qsqlquery.h"
#include "qsqlrecord.h"
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
        pageSizeTable(itemModel::getPageSize()),
        db(db),
        queryModel(nullptr, db),
        countModel(nullptr, db) {
    ui->setupUi(this);
    ui->deleteButton->setIcon(QIcon(":/pic/trash.png"));
    ui->exportButton->setIcon(QIcon(":/pic/download.png"));
    ui->deleteButton_2->setIcon(QIcon(":/pic/trash.png"));
    ui->exportButton_2->setIcon(QIcon(":/pic/download.png"));
    ui->checkBoxTitle->setChecked(true);
    ui->radioButtonAsc->setChecked(true);
    ui->dateEditFrom->setDate(QDate::currentDate());
    ui->dateEditTo->setDate(QDate::currentDate());
    ui->tableView->setModel(&queryModel);
    ui->tableView_2->setModel(&queryModel);
    ui->tableView->setStyleSheet("selection-background-color: rgb(200, 200, 200);\nselection-color: black;");
    ui->tableView_2->setStyleSheet("selection-background-color: rgb(200, 200, 200);\nselection-color: black;");
    ui->tableView->horizontalHeader()->setHighlightSections(false);
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
    QStringList typeList;
    while (query.next()) {
        QString type = query.value("typename").toString();
        typeList.append(type);
        // qDebug()<<type;
    }
    ui->comboBoxType->xAddItems(typeList);
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
    connect(ui->selectButton, &QCheckBox::stateChanged, this, [this](int stat) {
        if (stat == Qt::CheckState::Checked) {
            queryModel.checkAll();
        } else {
            queryModel.uncheckAll();
        }
    });
    connect(ui->selectButton_2, &QCheckBox::stateChanged, this, [this](int stat) {
        if (stat == Qt::CheckState::Checked) {
            queryModel.checkAll();
        } else {
            queryModel.uncheckAll();
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
    connect(ui->tableView, &QTableView::doubleClicked, this, &Search::tableCellDoubleClicked);
    connect(ui->tableView, &QTableView::clicked, this, [this](const QModelIndex &index) { dynamic_cast<itemModel *>(ui->tableView->model())->check(index.row()); });
    connect(ui->tableView_2, &QTableView::clicked, this, [this](const QModelIndex &index) { dynamic_cast<itemModel *>(ui->tableView->model())->check(index.row()); });
    connect(ui->deleteButton, &QPushButton::clicked, this, &Search::deleteButton_clicked);
    connect(ui->exportButton, &QPushButton::clicked, this, &Search::exportButton_clicked);
    connect(ui->deleteButton_2, &QPushButton::clicked, this, &Search::deleteButton_clicked);
    connect(ui->exportButton_2, &QPushButton::clicked, this, &Search::exportButton_clicked);
    connect(ui->sendSQLButton, &QPushButton::clicked, this, [this]() { this->sendSQL(ui->lineEdit_2->text()); });
}

void Search::searchButton_clicked() {
    /*构造查询的条件，预先查询结果的总数来更新页码，最后调用updateSearch()*/
    QString sql = "1=1";
    QStringList conditions;
    if (ui->lineEdit->text() == "") {
    } else {
        if (ui->checkBoxTitle->isChecked() && !ui->checkBoxText->isChecked()) {
            queryModel.setJoin(false);
            countModel.setJoin(false);
            conditions.append("match(description) against('" + ui->lineEdit->text() + +"'in boolean mode)");
        } else if (ui->checkBoxTitle->isChecked() && ui->checkBoxText->isChecked()) {
            queryModel.setJoin(true);
            countModel.setJoin(true);
            conditions.append("match(description) against('" + ui->lineEdit->text() + "'in boolean mode) or match(ocr_result) against('" + ui->lineEdit->text() + "'in boolean mode)");
        } else if (!ui->checkBoxTitle->isChecked() && ui->checkBoxText->isChecked()) {
            queryModel.setJoin(true);
            countModel.setJoin(true);
            conditions.append("match(ocr_result) against('" + ui->lineEdit->text() + "'in boolean mode)");
        }
    }

    if (ui->comboBoxType->isEnabled()) {
        QString typeFilter = "1=0";
        QStringList typeFilterList;
        QStringList typeList = ui->comboBoxType->GetSelItemsText();
        for (auto &&s : typeList) {
            typeFilterList.append("type='" + s + "'");
        }
        for (auto &&s : typeFilterList) {
            typeFilter += " or (" + s + ")";
        }
        conditions.append(typeFilter);
    }

    if (ui->dateEditFrom->isEnabled()) {
        conditions.append("date between '" + ui->dateEditFrom->text() + "' and '" + ui->dateEditTo->text() + "'");
    }

    for (auto &&c : conditions) {
        sql += " and (" + c + ")";
    }
    countModel.setFilter(sql);
    queryModel.setFilter(sql);
    countModel.setCountOnly(true);
    qDebug() << countModel.getSelectStatement();
    qDebug() << countModel.select();
    if (ui->comboBoxShow->currentIndex() == 0) {
        ui->pageNavigate->setMaxPage(countModel.record(0).value("count(*)").toInt() / pageSize + 1);
        ui->pageNavigate->setCurrentPage(1, true);
        currentPage = 1;
    } else {
        ui->pageNavigate->setMaxPage(countModel.record(0).value("count(*)").toInt() / pageSizeTable + 1);
        ui->pageNavigate->setCurrentPage(1, true);
        currentPage = 1;
    }
    updateSearch();
}

void Search::updateSearch() {
    /*接受查询条件，在此之上构造排序和分页条件，进行实际查询，根据radioButtn状态调用updateImgView()或updateTableView()
      除了被searchButtonClicked调用外，不改变查询条件，只改变排序和分类的操作最后也会调用此函数来显示结果*/
    queryModel.uncheckAll();
    switch (ui->comboBoxOrder->currentIndex()) {
    case 0:
        if (ui->radioButtonAsc->isChecked()) {
            queryModel.setSort(queryModel.fieldIndex("href"), Qt::SortOrder::AscendingOrder);
        } else {
            queryModel.setSort(queryModel.fieldIndex("href"), Qt::SortOrder::DescendingOrder);
        }
        break;
    case 1:
        if (ui->radioButtonAsc->isChecked()) {
            queryModel.setSort(queryModel.fieldIndex("date"), Qt::SortOrder::AscendingOrder);
        } else {
            queryModel.setSort(queryModel.fieldIndex("date"), Qt::SortOrder::DescendingOrder);
        }
        break;
    case 2:
        if (ui->radioButtonAsc->isChecked()) {
            queryModel.setSort(queryModel.fieldIndex("description"), Qt::SortOrder::AscendingOrder);
        } else {
            queryModel.setSort(queryModel.fieldIndex("description"), Qt::SortOrder::DescendingOrder);
        }
        break;
    default:
        if (ui->radioButtonAsc->isChecked()) {
            queryModel.setSort(queryModel.fieldIndex("href"), Qt::SortOrder::AscendingOrder);
        } else {
            queryModel.setSort(queryModel.fieldIndex("href"), Qt::SortOrder::DescendingOrder);
        }
        break;
    }
    if (ui->stackedWidget->currentIndex() == 0) {
        queryModel.setLimit((currentPage - 1) * pageSize, pageSize);
    } else if (ui->stackedWidget->currentIndex() == 1) {
        queryModel.setLimit((currentPage - 1) * pageSizeTable, pageSizeTable);
    }
    qDebug() << queryModel.getSelectStatement();
    ui->lineEdit_2->setText(queryModel.getSelectStatement());
    qDebug() << queryModel.select();
    if (ui->stackedWidget->currentIndex() == 0) {
        updateImgView();
    } else if (ui->stackedWidget->currentIndex() == 1) {
        updateTableView();
    }
}

void Search::updateImgView() {
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
    for (int i = 0; i < queryModel.rowCount(); i++) {
        if (!queryModel.record(i).isEmpty())
            addImgItem(queryModel.record(i), queryModel.index(i, 0));
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

void Search::updateTableView() {
    queryModel.resetHeader();
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
}

void Search::tableCellDoubleClicked(const QModelIndex &index) {
    QString name = queryModel.data(queryModel.index(index.row(), queryModel.fieldIndex("href"))).toString();
    //qDebug() << name;
    if (name != "") {
        openDetailMenu(name, index.row());
    }
}

void Search::deleteButton_clicked() {
    QSqlQuery query(db);
    query.prepare("delete from pictures where href=:href;");
    for (int i = 0; i < queryModel.rowCount(); i++) {
        if (queryModel.isChecked(i)) {
            int res = QMessageBox::warning(this, "确认",
                                           "确定删除" + queryModel.data(queryModel.index(i, queryModel.fieldIndex("href"))).toString() + "吗？",
                                           QMessageBox::Yes | QMessageBox::Cancel,
                                           QMessageBox::Cancel);
            if (res == QMessageBox::Yes) {
                query.bindValue(":href", queryModel.data(queryModel.index(i, queryModel.fieldIndex("href"))).toString());
                query.exec();
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
    for (int i = 0; i < queryModel.rowCount(); i++) {
        if (queryModel.isChecked(i)) {
            QString current = queryModel.data(queryModel.index(i, queryModel.fieldIndex("href"))).toString().simplified();
            if (QFile::copy(imgBase + current, targetDirPath + current)) {
                success++;
            } else {
                fail++;
                qDebug() << current;
            }
        }
    }
    QMessageBox::information(this, "导出", "导出完成\n成功：" + QString::number(success) + "，失败：" + QString::number(fail));
}

imgLoader::imgLoader(imagePreviewForm *target, QImageReader *reader, QSqlRecord record, QModelIndex index) :
        target(target), reader(reader), record(record), index(index) {}

void imgLoader::run() {
    QImage *img = new QImage(reader->read());
    if (img->isNull()) {
        QMessageBox::information(nullptr,
                                 "打开图像失败",
                                 "打开图像失败" + reader->fileName());
    }
    target->setImg(record, img, index);
    emit loadReady();
    delete reader;
}

imagePreviewForm *Search::addImgItem(QSqlRecord record, QModelIndex index) {
    /*根据图片信息设置寻找preViewList中可用的imagePreviewForm放入，大小和文字同步设置，实际图片读取放入Thread中*/
    QString filename(imgBase + record.value("href").toString());
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
            form->setImg(record, img, index);
            imgLoader *loader = new imgLoader(form, reader, record, index); //该指针由QThreadPool释放内存
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

void Search::openDetailMenu(QString href, int row) {
    auto newWindow = new DetailView(nullptr, db);
    connect(newWindow, &DetailView::edit, this, [this]() { updateSearch(); });
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
    query.prepare(sql);
    query.exec();
    queryModel.setQuery(std::move(query));
    if (queryModel.lastError().isValid()) {
        QSqlError sqlerror = queryModel.lastError();
        qDebug() << sqlerror.nativeErrorCode();
        QString errortext = sqlerror.text();
        qDebug() << errortext;
        if (errortext == "") {
            errortext = "empty query";
        }
        QMessageBox::critical(this, "错误", errortext);
    } else {
        ui->tableView_2->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
        ui->tableView_2->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        ui->tableView_2->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
        ui->tableView_2->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
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
