#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QString>
#include <QList>
#include <QtSql/QSqlDatabase>
#include "item.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void initial();
    ~MainWindow();
private:
    QDir dir;
    int ImageCount;
    int flag;
    QString  imagePath;
    QList<QString> list;
    QList<Item*> itemList;
    QSqlDatabase db;
    void updateTypes();
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();

    void on_buttonAddType_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
