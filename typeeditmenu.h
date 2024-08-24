#ifndef TYPEEDITMENU_H
#define TYPEEDITMENU_H

#include <QListWidget>
#include <QSqlDatabase>
#include <QWidget>

namespace Ui {
    class typeEditMenu;
}

class typeEditMenu : public QWidget {
    Q_OBJECT
signals:
    void typeUpdateEvent();

public:
    explicit typeEditMenu(QWidget *parent, QSqlDatabase &db);
    ~typeEditMenu();

private slots:
    void listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::typeEditMenu *ui;
    QSqlDatabase &db;
    QString itemSelected;
    void addType();
    void deleteType();
    void updateTypes();
};

#endif // TYPEEDITMENU_H
