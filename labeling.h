#ifndef LABELING_H
#define LABELING_H

#include <QMainWindow>
#include <QDir>
#include <QString>
#include <QList>
#include <QtSql/QSqlDatabase>
#include "item.h"

namespace Ui {
class LabelWindow;
}

class LabelWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LabelWindow(QWidget *parent = 0);
    ~LabelWindow();
private:
    QDir dir;
    int ImageCount;
    int flag;
    QString  imagePath;
    QList<Item> itemList;
    QSqlDatabase db;
    void updateTypes();
private slots:
    void pushButtonNext_clicked();
    void pushButtonStart_clicked();
    void pushButtonAdd_clicked();
    void pushButtonFinish_clicked();
    void pushButtonLast_clicked();
    void pushButtonAddType_clicked();

private:
    Ui::LabelWindow *ui;
};

#endif // LABELING_H
