#ifndef DETAILVIEW_H
#define DETAILVIEW_H

#include <QMainWindow>
#include <QSqlQuery>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <window.h>

namespace Ui {
    class DetailView;
}

class DetailView : public Window {
    Q_OBJECT

public:
    explicit DetailView(QWidget *parent, QSqlDatabase &db);
    void OpenImg(QString href);
    ~DetailView();

private:
    Ui::DetailView *ui;
    QSqlDatabase &db;
    QString current;
    void commitChange();
    void cancelChange();
    void typeMenuOpen();
    void ocrMenuOpen();
    void updateTypes();
    void enableEdit();
    void disableEdit();
    void savePic();
};

#endif // DETAILVIEW_H
