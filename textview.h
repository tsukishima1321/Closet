#ifndef TEXTVIEW_H
#define TEXTVIEW_H

#include "textpreviewform.h"
#include "window.h"
#include <QLayout>
#include <QMainWindow>
#include <QSqlDatabase>

namespace Ui {
    class TextView;
}

namespace TextViewConstants {
    constexpr int pageSize = 30;
}

class TextView : public Window {
    Q_OBJECT

public:
    explicit TextView(QWidget *parent, QSqlDatabase &db);
    ~TextView();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Ui::TextView *ui;
    QSqlDatabase &db;

    int currentColumnCount;
    QHBoxLayout *hBoxLayout;
    QList<QVBoxLayout *> vBoxLayouts;
    textPreviewForm *preViewList;
    void updateTextView(QSqlQuery &&query);
    void locateText();
    textPreviewForm *addTextItem(QString text, QString date, int id);

    void searchButton_clicked();
    void updateSearch();
    QString currentFilter;

    void openDetailMenu(int id);
    int currentPage;
};

#endif // TEXTVIEW_H