#ifndef OCRMENU_H
#define OCRMENU_H

#include <QSqlDatabase>
#include <QWidget>

namespace Ui {
    class ocrMenu;
}

class ocrMenu : public QWidget {
    Q_OBJECT

public:
    explicit ocrMenu(QWidget *parent, QSqlDatabase &db, QString key);
    ~ocrMenu();

private:
    Ui::ocrMenu *ui;
    QSqlDatabase &db;
    QString key;
private slots:
    void editCommit();
    void editCancel();
};

#endif // OCRMENU_H
