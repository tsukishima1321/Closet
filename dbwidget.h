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
    static dbWidget* Instance(QWidget *parent,QList<Item*>* itemList,QString name,QString password);
protected:
    explicit dbWidget(QWidget *parent,QList<Item*>* itemList,QString name,QString password);
private:
    static dbWidget* _instance;
    QList<Item*>* itemList;
    QList<QString> typeList;
    QSqlDatabase db;
    ~dbWidget();
private slots:
    void on_pushButtonSearch_clicked();
    void on_pushButtonCommitAll_clicked();
    void on_pushButtonCommit_clicked();
    void on_pushButtonDelete_clicked();
    void on_pushButtonBuild_clicked();
    void on_pushButtonSendSQL_clicked();
private:
    void updateTable1();
    void updateTable2();
    Ui::dbWidget *ui;
};

#endif // DBWIDGET_H
