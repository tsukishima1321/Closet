#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QSqlQuery>
#include <QTableWidget>

constexpr int pageSizeTable = 100;

class TableWidget : public QTableWidget {
    Q_OBJECT
public:
    TableWidget(QWidget *parent = nullptr);
    static int getPageSize();
    void resetHeader();
    void fillItems(QSqlQuery &query, int indexOffset, int maxRow = pageSizeTable);

private:
    void allSelectItems(int n);
    bool allSelected;
};

#endif // TABLEWIDGET_H
