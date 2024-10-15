#include "tablewidget.h"
#include "qheaderview.h"

int TableWidget::getPageSize() {
    return pageSizeTable;
}

TableWidget::TableWidget(QWidget *parent) :
        QTableWidget(parent) {
    verticalHeader()->setHidden(true);
    setRowCount(pageSizeTable);
    setStyleSheet("selection-background-color: rgb(200, 200, 200);\nselection-color: black;");
    clear();
    connect(horizontalHeader(), &QHeaderView::sectionClicked, this, &TableWidget::allSelectItems);
    //resetHeader();
}

void TableWidget::allSelectItems(int n) {
    if (n == 0) {
        if (allSelected) {
            for (int i = 0; i < rowCount(); i++) {
                if (item(i, 0)) {
                    item(i, 0)->setCheckState(Qt::Checked);
                }
            }
            horizontalHeaderItem(0)->setIcon(QIcon(":/pic/square-check.png"));
            allSelected = false;
        } else {
            for (int i = 0; i < rowCount(); i++) {
                if (item(i, 0)) {
                    item(i, 0)->setCheckState(Qt::Unchecked);
                }
            }
            horizontalHeaderItem(0)->setIcon(QIcon(":/pic/square.png"));
            allSelected = true;
        }
    }
}

void TableWidget::resetHeader() {
    QStringList headers;
    headers << ""
            << "文件名"
            << "日期"
            << "描述"
            << "分类";
    setHorizontalHeaderLabels(headers);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Interactive);
    horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    horizontalHeaderItem(0)->setIcon(QIcon(":/pic/square.png"));
    horizontalHeader()->setHighlightSections(false);
}

void TableWidget::fillItems(QSqlQuery &query, int indexOffset, int maxRow) {
    clear();
    resetHeader();
    int i = 0;
    if (maxRow > pageSizeTable) {
        setRowCount(maxRow);
    }
    while (query.next()) {
        auto check = new QTableWidgetItem(QString::number(i + 1 + indexOffset));
        check->setCheckState(Qt::CheckState::Unchecked);
        setItem(i, 0, check);
        setItem(i, 1, new QTableWidgetItem(query.value("href").toString()));
        setItem(i, 2, new QTableWidgetItem(query.value("date").toString()));
        setItem(i, 3, new QTableWidgetItem(query.value("description").toString()));
        setItem(i, 4, new QTableWidgetItem(query.value("type").toString()));
        i++;
    }
}
