#ifndef TEXTDETAILVIEW_H
#define TEXTDETAILVIEW_H

#include <QSqlDatabase>
#include "window.h"

namespace Ui {
    class TextDetailView;
}

class TextDetailView : public Window {
    Q_OBJECT

signals:
    void edit();

public:
    explicit TextDetailView(QWidget *parent, QSqlDatabase &db);
    void OpenText(int id);
    void setDate(QDate date);
    void setText(QString text);
    ~TextDetailView();

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::TextDetailView *ui;
    QSqlDatabase &db;
    int current;
    void commitChange();
    void cancelChange();
    void deleteText();
};

#endif // TEXTDETAILVIEW_H
