#include "TextSearch.h"
#include "iconresources.h"
#include "textdetailview.h"
#include "ui_textsearch.h"
#include <QMessageBox>
#include <QSqlQuery>

TextSearch::TextSearch(QWidget *parent, QSqlDatabase &db) :
        Window(parent),
        ui(new Ui::TextSearch),
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
    previewList = new TextPreviewForm[textSearchConstants::pageSize];
    previewListSpan = std::span<TextPreviewForm>(previewList, textSearchConstants::pageSize);
    for (auto &&form : previewListSpan) {
        connect(&form, &TextPreviewForm::isClicked, this, &TextSearch::openDetailMenu);
    }

    connect(ui->selectButton, &QCheckBox::stateChanged, this, [this](int stat) {
        if (stat == Qt::CheckState::Checked) {
            for (auto &&form : previewListSpan) {
                if (!form.isAvailable()) {
                    form.check();
                }
            }
        } else {
            for (auto &&form : previewListSpan) {
                if (!form.isAvailable()) {
                    form.uncheck();
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

    connect(ui->searchButton, &QPushButton::clicked, this, &TextSearch::searchButton_clicked);
    connect(ui->lineEdit, &QLineEdit::returnPressed, this, &TextSearch::searchButton_clicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &TextSearch::deleteButton_clicked);
    connect(ui->combineButton, &QPushButton::clicked, this, &TextSearch::combineButton_clicked);
    connect(ui->newTextButton, &QPushButton::clicked, this, [this]() {
        TextDetailView *detail = new TextDetailView(this, this->db);
        connect(detail, &TextDetailView::edit, this, [this]() { updateSearch(); });
        detail->setDate(QDate::currentDate());
        detail->show();
    });

    searchButton_clicked();
}

void TextSearch::searchButton_clicked() {
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
    ui->pageNavigate->setMaxPage(query.value(0).toInt() / textSearchConstants::pageSize + 1);
    ui->pageNavigate->setCurrentPage(1, true);
    currentPage = 1;

    currentFilter = sql;
    updateSearch();
}

void TextSearch::updateSearch() {
    /*接受查询条件，在此之上构造排序和分页条件，进行实际查询，根据radioButtn状态调用updateImgView()或updateTableView()
      除了被searchButtonClicked调用外，不改变查询条件，只改变排序和分类的操作最后也会调用此函数来显示结果*/
    QString sql = "select id,date,left(text,200) as preview,length(text) as length from texts where " + currentFilter;
    switch (ui->comboBoxOrder->currentIndex()) {
    case 0:
        if (ui->radioButtonAsc->isChecked()) {
            sql += " order by date asc, id asc";
        } else {
            sql += " order by date desc, id desc";
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
            sql += " order by text asc, id asc";
        } else {
            sql += " order by text desc, id desc";
        }
        break;
    default:
        sql += " order by date desc, id desc";
        break;
    }
    QSqlQuery query(db);
    query.prepare(sql + " limit " + QString::number((currentPage - 1) * textSearchConstants::pageSize) + ", " + QString::number(textSearchConstants::pageSize));
    query.exec();
    updatetextSearch(std::move(query));
}

void TextSearch::updatetextSearch(QSqlQuery &&query) {
    for (auto &&form : previewListSpan) {
        form.~TextPreviewForm();
        new (&form) TextPreviewForm;
        connect(&form, &TextPreviewForm::isClicked, this, &TextSearch::openDetailMenu);
        // form->hideElements();
    }
    for (int i = 0; i < textSearchConstants::pageSize; i++) {
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

TextPreviewForm *TextSearch::addTextItem(QString text, QString date, int id) {
    TextPreviewForm *formToAdd = nullptr;
    for (auto &&form : previewListSpan) {
        if (form.isAvailable()) {
            formToAdd = &form;
            break;
        }
    }
    if (formToAdd) {
        formToAdd->setText(text, date, id);
    }
    return formToAdd;
}

void TextSearch::locateText() {
    for (auto &&form : previewListSpan) {
        QVBoxLayout *columnMinHeight = nullptr;
        int minHeight = 999999;
        for (QVBoxLayout *column : vBoxLayouts) {
            int height = 0;
            for (int i = column->count() - 1; i >= 0; i--) {
                height += dynamic_cast<TextPreviewForm *>(column->itemAt(i)->widget())->getHeight();
            }
            if (height < minHeight) {
                // qDebug() << height;
                columnMinHeight = column;
                minHeight = height;
            }
        }
        if (columnMinHeight) {
            columnMinHeight->addWidget(&form);
        }
    }
}

void TextSearch::resizeEvent(QResizeEvent *event) {
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

void TextSearch::openDetailMenu(int id) {
    auto newWindow = new TextDetailView(nullptr, db);
    newWindow->OpenText(id);
    newWindow->show();
    connect(newWindow, &TextDetailView::edit, this, [this]() { updateSearch(); });
}

void TextSearch::deleteButton_clicked() {
    QSqlQuery query(db);
    query.prepare("delete from texts where id=:id;");
    for (auto &&form : previewListSpan) {
        if (form.isCheck()) {
            int res = QMessageBox::warning(this, "确认",
                                           "确定删除" + QString::number(form.getId()) + "：" + form.getDate() + "吗？",
                                           QMessageBox::Yes | QMessageBox::Cancel,
                                           QMessageBox::Cancel);
            if (res == QMessageBox::Yes) {
                query.bindValue(":id", form.getId());
                query.exec();
            }
        }
    }
    updateSearch();
}

void TextSearch::combineButton_clicked() {
    QList<int> ids;
    for (auto &&form : previewListSpan) {
        if (form.isCheck()) {
            ids.append(form.getId());
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
    TextDetailView *detail = new TextDetailView(this, db);
    connect(detail, &TextDetailView::edit, this, [this]() { deleteButton_clicked();updateSearch(); });
    detail->setText(text);
    detail->setDate(date);
    detail->show();
}

TextSearch::~TextSearch() {
    delete[] previewList;
    if (hBoxLayout) {
        delete hBoxLayout;
    }
    delete ui;
}
