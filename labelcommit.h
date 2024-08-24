#ifndef LABELCOMMIT_H
#define LABELCOMMIT_H

#include "item.h"
#include <QDebug>
#include <QList>
#include <QSqlQuery>
#include <QString>
#include <QWidget>
#include <QtSql/QSqlDatabase>

namespace Ui {
    class labelCommit;
}

class labelCommit : public QWidget {
    Q_OBJECT

public:
    labelCommit(QWidget *parent, QList<Item> *itemList, QSqlDatabase &db);

private:
    QList<Item> *itemList;
    QList<QString> typeList;
    QSqlDatabase &db;
    ~labelCommit();
private slots:
    void pushButtonSearch_clicked();
    void pushButtonCommitAll_clicked();
    void pushButtonCommit_clicked();
    void pushButtonDelete_clicked();
    void pushButtonBuild_clicked();
    void pushButtonSendSQL_clicked();
    void table1CellDoubleClicked(int, int);
    void table2CellDoubleClicked(int, int);

private:
    void updateTable1();
    void updateTable2();
    Ui::labelCommit *ui;
};

#endif // LABELCOMMIT_H
