#ifndef DBWIDGET_H
#define DBWIDGET_H

#include <QWidget>
#include "item.h"
#include <QList>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QString>

namespace Ui {
class dbWidget;
}

class dbWidget : public QWidget
{
    Q_OBJECT

public:
    explicit dbWidget(QWidget *parent,QList<Item*>* itemList,QString name,QString password);
    QList<Item*>* itemList;
    QList<QString> typeList;
    QSqlDatabase db;
    ~dbWidget();

private slots:
    void on_searchButton_clicked();

    void on_addallButton_clicked();

    void on_addButton_2_clicked();

    void on_deleteButton_clicked();

    void on_buildButton_clicked();

    void on_pushButton_clicked();

private:
    void updateTable1();
    void updateTable2();
    Ui::dbWidget *ui;
};

#endif // DBWIDGET_H
