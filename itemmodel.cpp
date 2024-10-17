#include "itemmodel.h"

int itemModel::getPageSize() {
    return pageSizeTable;
}

itemModel::itemModel(QObject *parent, const QSqlDatabase &db) :
        QSqlTableModel{parent, db},
        offset(0),
        pageSize(0),
        countOnly(false),
        join(false) {
    setTable("pictures");
    //setEditStrategy(EditStrategy::OnFieldChange);
}

void itemModel::setLimit(int offset, int pageSize) {
    this->offset = offset;
    this->pageSize = pageSize;
}

void itemModel::setJoin(bool join) {
    this->join = join;
}

void itemModel::setCountOnly(bool countOnly) {
    this->countOnly = countOnly;
}

QString itemModel::getSelectStatement() const {
    return selectStatement();
}

void itemModel::resetAllFilter() {
    this->offset = 0;
    this->pageSize = 0;
    this->countOnly = false;
    this->join = false;
    this->setFilter("1=1");
}

void itemModel::resetHeader() {
    setHeaderData(0, Qt::Horizontal, "文件名");
    setHeaderData(1, Qt::Horizontal, "描述");
    setHeaderData(2, Qt::Horizontal, "分类");
    setHeaderData(3, Qt::Horizontal, "日期");
}

QString itemModel::selectStatement() const {
    QString sql = QSqlTableModel::selectStatement();
    if (countOnly) {
        sql.replace("`href`, `description`, `type`, `date`", "count(*)");
    } else {
        sql.replace("SELECT `href`", "SELECT pictures.href");
    }
    if (join) {
        sql.replace("FROM `pictures`", "from `pictures` join pictures_ocr on pictures.href=pictures_ocr.href");
    }
    if (pageSize != 0) {
        sql += QString(" limit %1,%2").arg(QString::number(offset), QString::number(pageSize));
    }
    return sql;
}

void itemModel::check(int row) {
    checkState[row] = (checkState[row] == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
    emit dataChanged(index(row, 0), index(row, 0));
}

bool itemModel::isChecked(int row) const {
    return checkState[row] == Qt::Checked;
}

void itemModel::checkAll() {
    for (int r = 0; r < rowCount(); r++) {
        checkState[r] = Qt::Checked;
        emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
    }
}

void itemModel::uncheckAll() {
    for (int r = 0; r < rowCount(); r++) {
        checkState[r] = Qt::Unchecked;
        emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
    }
}

QVariant itemModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    switch (role) {
        //case Qt::TextAlignmentRole:
        //              return Qt::AlignLeft | Qt::AlignVCenter;

        //case Qt::DisplayRole:
        //              return arr_row_list->at(index.row()).at(index.column());

    case Qt::CheckStateRole:
        if (index.column() == 0) {
            if (checkState.contains(index.row()))
                return checkState[index.row()] == Qt::Checked ? Qt::Checked : Qt::Unchecked;

            return Qt::Unchecked;
        }
    default:
        return QSqlTableModel::data(index, role);
    }

    return QSqlTableModel::data(index, role);
}

Qt::ItemFlags itemModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == 0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;

    return QSqlTableModel::flags(index);
}
