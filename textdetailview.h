#ifndef TEXTDETAILVIEW_H
#define TEXTDETAILVIEW_H

#include <QSqlDatabase>
#include "window.h"

namespace Ui {
    class textDetailView;
}

class textDetailView : public Window {
    Q_OBJECT

signals:
    void edit();

public:
    explicit textDetailView(QWidget *parent, QSqlDatabase &db);
    void OpenText(int id);
    void setDate(QDate date);
    ~textDetailView();

private:
    Ui::textDetailView *ui;
    QSqlDatabase &db;
    int current;
    void commitChange();
    void cancelChange();
    void deleteText();
};

#endif // TEXTDETAILVIEW_H