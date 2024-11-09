#include "itemmodel.h"

int ItemModel::getPageSize() {
    return pageSizeTable;
}

ItemModel::ItemModel(QObject *parent, const QSqlDatabase &db) :
        QSqlTableModel{parent, db},
        offset(0),
        pageSize(0),
        countOnly(false),
        join(false) {
    setTable("pictures");
    //setEditStrategy(EditStrategy::OnFieldChange);
}

void ItemModel::setLimit(int offset, int pageSize) {
    this->offset = offset;
    this->pageSize = pageSize;
}

void ItemModel::setJoin(bool join) {
    this->join = join;
}

void ItemModel::setCountOnly(bool countOnly) {
    this->countOnly = countOnly;
}

QString ItemModel::getSelectStatement() const {
    return selectStatement();
}

void ItemModel::resetAllFilter() {
    this->offset = 0;
    this->pageSize = 0;
    this->countOnly = false;
    this->join = false;
    this->setFilter("1=1");
}

void ItemModel::resetHeader() {
    setHeaderData(0, Qt::Horizontal, "文件名");
    setHeaderData(1, Qt::Horizontal, "描述");
    setHeaderData(2, Qt::Horizontal, "分类");
    setHeaderData(3, Qt::Horizontal, "日期");
}

QString ItemModel::selectStatement() const {
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
    //附加一个主键排序来保证分页的稳定性
    sql.replace("ASC", "ASC, pictures.href ASC");
    sql.replace("DESC", "DESC, pictures.href DESC");
    return sql;
}

void ItemModel::check(int row) {
    checkState[row] = (checkState[row] == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
    emit dataChanged(index(row, 0), index(row, 0));
}

bool ItemModel::isChecked(int row) const {
    return checkState[row] == Qt::Checked;
}

void ItemModel::checkAll() {
    for (int r = 0; r < rowCount(); r++) {
        checkState[r] = Qt::Checked;
        emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
    }
}

void ItemModel::uncheckAll() {
    for (int r = 0; r < rowCount(); r++) {
        checkState[r] = Qt::Unchecked;
        emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
    }
}

QVariant ItemModel::data(const QModelIndex &index, int role) const {
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
        break;
    default:
        return QSqlTableModel::data(index, role);
    }

    return QSqlTableModel::data(index, role);
}

Qt::ItemFlags ItemModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    if (index.column() == 0)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;

    return QSqlTableModel::flags(index);
}
