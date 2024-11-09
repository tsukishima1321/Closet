#ifndef OCRMENU_H
#define OCRMENU_H

#include <QSqlDatabase>
#include <QWidget>

namespace Ui {
    class OCRMenu;
}

class OCRMenu : public QWidget {
    Q_OBJECT

public:
    explicit OCRMenu(QWidget *parent, QSqlDatabase &db, QString key);
    ~OCRMenu();

private:
    Ui::OCRMenu *ui;
    QSqlDatabase &db;
    QString key;
private slots:
    void editCommit();
    void editCancel();
};

#endif // OCRMENU_H
