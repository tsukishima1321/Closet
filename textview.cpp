#include "textview.h"
#include "iconresources.h"
#include "textdetailview.h"
#include "ui_textview.h"
#include <QMessageBox>
#include <QSqlQuery>

TextView::TextView(QWidget *parent, QSqlDatabase &db) :
        Window(parent),
        ui(new Ui::TextView),
        db(db),
        currentColumnCount(3),
        currentPage(1) {
    ui->setupUi(this);
    ui->radioButtonDesc->setChecked(true);
    ui->dateEditFrom->setDate(QDate::currentDate());
    ui->dateEditTo->setDate(QDate::currentDate());
    ui->deleteButton->setIcon(IconResources::getIcons()["trash"]);
    ui->newTextButton->setIcon(IconResources::getIcons()["new-file"]);
    ui->combineButton->setIcon(IconResources::getIcons()["combine"]);

    hBoxLayout = new QHBoxLayout;
    for (int i = 0; i < currentColumnCount; i++) {
        QVBoxLayout *column = new QVBoxLayout;
        column->setAlignment(Qt::AlignTop);
        hBoxLayout->addLayout(column);
        vBoxLayouts.append(column);
    }
    ui->scrollAreaWidgetContents->setLayout(hBoxLayout);

    currentPage = ui->pageNavigate->getCurrentPage();
    preViewList = new textPreviewForm[TextViewConstants::pageSize];
    for (int i = 0; i < TextViewConstants::pageSize; i++) {
        textPreviewForm *form = &preViewList[i];
        connect(form, &textPreviewForm::isClicked, this, &TextView::openDetailMenu);
    }

    connect(ui->selectButton, &QCheckBox::stateChanged, this, [this](int stat) {
        if (stat == Qt::CheckState::Checked) {
            for (int i = 0; i < TextViewConstants::pageSize; i++) {
                textPreviewForm *form = &preViewList[i];
                if (!form->isAvailable()) {
                    form->check();
                }
            }
        } else {
            for (int i = 0; i < TextViewConstants::pageSize; i++) {
                textPreviewForm *form = &preViewList[i];
                if (!form->isAvailable()) {
                    form->uncheck();
                }
            }
        }
    });

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

    connect(ui->checkBoxDate, &QCheckBox::stateChanged, this, [this](int stat) {
        if (stat == Qt::CheckState::Checked) {
            ui->dateEditFrom->setEnabled(true);
            ui->dateEditTo->setEnabled(true);
        } else {
            ui->dateEditFrom->setEnabled(false);
            ui->dateEditTo->setEnabled(false);
        }
    });

    connect(ui->comboBoxOrder, &QComboBox::currentIndexChanged, this, [this](int i) {
        (void)i;
        updateSearch();
    });

    connect(ui->searchButton, &QPushButton::clicked, this, &TextView::searchButton_clicked);
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &TextView::searchButton_clicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &TextView::deleteButton_clicked);
    connect(ui->combineButton, &QPushButton::clicked, this, &TextView::combineButton_clicked);
    connect(ui->newTextButton, &QPushButton::clicked, this, [this]() {
        textDetailView *detail = new textDetailView(this, this->db);
        connect(detail, &textDetailView::edit, this, [this]() { updateSearch(); });
        detail->setDate(QDate::currentDate());
        detail->show();
    });
}

void TextView::searchButton_clicked() {
    /*构造查询的条件，预先查询结果的总数来更新页码，最后调用updateSearch()*/
    QString sql = "1=1";
    QStringList conditions;

    if (ui->lineEdit->text() == "") {
    } else {
        conditions.append("match(text) against('" + ui->lineEdit->text() + "'in boolean mode)");
    }

    if (ui->dateEditFrom->isEnabled()) {
        conditions.append("date between '" + ui->dateEditFrom->text() + "' and '" + ui->dateEditTo->text() + "'");
    }

    for (auto &&c : conditions) {
        sql += " and (" + c + ")";
    }

    QSqlQuery query(db);
    query.prepare("select count(*) from texts where " + sql);
    query.exec();
    query.next();
    ui->pageNavigate->setMaxPage(query.value(0).toInt() / TextViewConstants::pageSize + 1);
    ui->pageNavigate->setCurrentPage(1, true);
    currentPage = 1;

    currentFilter = sql;
    updateSearch();
}

void TextView::updateSearch() {
    /*接受查询条件，在此之上构造排序和分页条件，进行实际查询，根据radioButtn状态调用updateImgView()或updateTableView()
      除了被searchButtonClicked调用外，不改变查询条件，只改变排序和分类的操作最后也会调用此函数来显示结果*/
    QString sql = "select id,date,left(text,200) as preview,length(text) as length from texts where " + currentFilter;
    switch (ui->comboBoxOrder->currentIndex()) {
    case 0:
        if (ui->radioButtonAsc->isChecked()) {
            sql += " order by date asc";
        } else {
            sql += " order by date desc";
        }
        break;
    case 1:
        if (ui->radioButtonAsc->isChecked()) {
            sql += " order by id asc";
        } else {
            sql += " order by id desc";
        }
        break;
    case 2:
        if (ui->radioButtonAsc->isChecked()) {
            sql += " order by text asc";
        } else {
            sql += " order by text desc";
        }
        break;
    default:
        sql += " order by date desc";
        break;
    }
    QSqlQuery query(db);
    query.prepare(sql + " limit " + QString::number((currentPage - 1) * TextViewConstants::pageSize) + ", " + QString::number(TextViewConstants::pageSize));
    query.exec();
    updateTextView(std::move(query));
}

void TextView::updateTextView(QSqlQuery &&query) {
    for (int i = 0; i < TextViewConstants::pageSize; i++) {
        textPreviewForm *form = &preViewList[i];
        form->~textPreviewForm();
        new (form) textPreviewForm;
        connect(form, &textPreviewForm::isClicked, this, &TextView::openDetailMenu);
        // form->hideElements();
    }
    for (int i = 0; i < TextViewConstants::pageSize; i++) {
        if (query.next()) {
            auto item = addTextItem(query.value("preview").toString(), query.value("date").toString(), query.value("id").toInt());
            if (query.value("length").toInt() > 200) {
                item->setOmit(true);
            }
        }
    }
    for (QVBoxLayout *column : vBoxLayouts) {
        for (int i = column->count() - 1; i >= 0; i--) {
            column->removeItem(column->itemAt(i));
        }
    }
    locateText();
}

textPreviewForm *TextView::addTextItem(QString text, QString date, int id) {
    textPreviewForm *form = nullptr;
    for (int i = 0; i < TextViewConstants::pageSize; i++) {
        textPreviewForm *f = &preViewList[i];
        if (f->isAvailable()) {
            form = f;
            break;
        }
    }
    if (form) {
        form->setText(text, date, id);
    }
    return form;
}

void TextView::locateText() {
    for (int i = 0; i < TextViewConstants::pageSize; i++) {
        textPreviewForm *form = &preViewList[i];
        QVBoxLayout *columnMinHeight = nullptr;
        int minHeight = 999999;
        for (QVBoxLayout *column : vBoxLayouts) {
            int height = 0;
            for (int i = column->count() - 1; i >= 0; i--) {
                height += dynamic_cast<textPreviewForm *>(column->itemAt(i)->widget())->getHeight();
            }
            if (height < minHeight) {
                // qDebug() << height;
                columnMinHeight = column;
                minHeight = height;
            }
        }
        if (columnMinHeight) {
            columnMinHeight->addWidget(form);
        }
    }
}

void TextView::resizeEvent(QResizeEvent *event) {
    (void)event;
    int width = ui->scrollArea->width();
    if (ui->scrollArea->width() == 100) {
        currentColumnCount = 3;
    } else {
        currentColumnCount = width / 300;
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
        QVBoxLayout *column = new QVBoxLayout();
        column->setAlignment(Qt::AlignTop);
        hBoxLayout->addLayout(column);
        vBoxLayouts.append(column);
    }
    locateText();
    Window::resizeEvent(event);
}

void TextView::openDetailMenu(int id) {
    auto newWindow = new textDetailView(nullptr, db);
    newWindow->OpenText(id);
    newWindow->show();
    connect(newWindow, &textDetailView::edit, this, [this]() { updateSearch(); });
}

void TextView::deleteButton_clicked() {
    QSqlQuery query(db);
    query.prepare("delete from texts where id=:id;");
    for (int i = 0; i < TextViewConstants::pageSize; i++) {
        if (preViewList[i].isCheck()) {
            int res = QMessageBox::warning(this, "确认",
                                           "确定删除" + QString::number(preViewList[i].getId()) + "：" + preViewList[i].getDate() + "吗？",
                                           QMessageBox::Yes | QMessageBox::Cancel,
                                           QMessageBox::Cancel);
            if (res == QMessageBox::Yes) {
                query.bindValue(":id", preViewList[i].getId());
                query.exec();
            }
        }
    }
    updateSearch();
}

void TextView::combineButton_clicked() {
    QList<int> ids;
    for (int i = 0; i < TextViewConstants::pageSize; i++) {
        if (preViewList[i].isCheck()) {
            ids.append(preViewList[i].getId());
        }
    }
    if (ids.size() < 2) {
        QMessageBox::warning(this, "错误", "请至少选择两个文本");
        return;
    }
    std::sort(ids.begin(), ids.end());
    QString text;
    QDate date = QDate::fromString("2000-01-01");
    for (int id : ids) {
        QSqlQuery query(db);
        query.prepare("select text,date from texts where id=:id");
        query.bindValue(":id", id);
        query.exec();
        query.next();
        text += query.value("text").toString() + "\n\n";
        if (query.value("date").toDate() > date) {
            date = query.value("date").toDate();
        }
    }
    textDetailView *detail = new textDetailView(this, db);
    connect(detail, &textDetailView::edit, this, [this]() { deleteButton_clicked();updateSearch(); });
    detail->setText(text);
    detail->setDate(date);
    detail->show();
}

TextView::~TextView() {
    delete[] preViewList;
    if (hBoxLayout) {
        delete hBoxLayout;
    }
    delete ui;
}
