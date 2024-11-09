#ifndef ITEMMODEL_H
#define ITEMMODEL_H

#include <QMap>
#include <QSqlTableModel>

constexpr int pageSizeTable = 100;

class ItemModel : public QSqlTableModel {
public:
    explicit ItemModel(QObject *parent = nullptr, const QSqlDatabase &db = QSqlDatabase());
    void setLimit(int offset, int pageSize);
    void setJoin(bool join);
    void setCountOnly(bool countOnly);
    void resetAllFilter();
    bool isChecked(int row) const;
    void check(int row);
    void checkAll();
    void uncheckAll();
    void resetHeader();
    static int getPageSize();
    QString getSelectStatement() const;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

protected:
    virtual QString selectStatement() const override;
    QMap<int, Qt::CheckState> checkState;
    int offset;
    int pageSize;
    bool countOnly;
    bool join;
};

#endif // ITEMMODEL_H
