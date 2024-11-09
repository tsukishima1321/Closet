#ifndef TYPEEDITMENU_H
#define TYPEEDITMENU_H

#include <QListWidget>
#include <QSqlDatabase>
#include <QWidget>

namespace Ui {
    class TypeEditMenu;
}

class TypeEditMenu : public QWidget {
    Q_OBJECT
signals:
    void typeUpdateEvent();

public:
    explicit TypeEditMenu(QWidget *parent, QSqlDatabase &db);
    ~TypeEditMenu();

private slots:
    void listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::TypeEditMenu *ui;
    QSqlDatabase &db;
    QString itemSelected;
    void addType();
    void deleteType();
    void updateTypes();
};

#endif // TYPEEDITMENU_H
