#ifndef LABELING_H
#define LABELING_H

#include "item.h"
#include <QDir>
#include <QMap>
#include <QMainWindow>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <window.h>

namespace Ui {
    class LabelWindow;
}

class LabelWindow : public Window {
    Q_OBJECT

public:
    explicit LabelWindow(QWidget *parent = 0);
    ~LabelWindow();

private:
    QDir dir;
    int ImageCount;
    int currentImgIndex;
    QString imagePath;
    //QList<Item> itemList;
    QMap<QString, Item> itemMap;
    QSqlDatabase dbVisitor;
    void updateTypes();
    virtual void keyPressEvent(QKeyEvent *) override;
    virtual void closeEvent(QCloseEvent *) override;
private slots:
    void pushButtonNext_clicked();
    void pushButtonStart_clicked();
    void pushButtonAdd_clicked();
    void pushButtonFinish_clicked();
    void pushButtonLast_clicked();
    void pushButtonDelete_clicked();
    void pushButtonAddType_clicked();
    void freshButton_clicked();
    void freshDateButton_clicked();

private:
    Ui::LabelWindow *ui;
    bool isLogedIn;
    void updateTable();
};

#endif // LABELING_H
