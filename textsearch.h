#ifndef textSearch_H
#define textSearch_H

#include "textpreviewform.h"
#include "window.h"
#include <QLayout>
#include <QMainWindow>
#include <QSqlDatabase>
#include <span>

namespace Ui {
    class TextSearch;
}

namespace textSearchConstants {
    constexpr int pageSize = 30;
}

class TextSearch : public Window {
    Q_OBJECT

public:
    explicit TextSearch(QWidget *parent, QSqlDatabase &db);
    ~TextSearch();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Ui::TextSearch *ui;
    QSqlDatabase &db;

    int currentColumnCount;
    QHBoxLayout *hBoxLayout;
    QList<QVBoxLayout *> vBoxLayouts;
    TextPreviewForm *previewList;
    std::span<TextPreviewForm> previewListSpan;
    void updatetextSearch(QSqlQuery &&query);
    void locateText();
    TextPreviewForm *addTextItem(QString text, QString date, int id);

    void searchButton_clicked();
    void updateSearch();
    QString currentFilter;
    int currentPage;

    void openDetailMenu(int id);

    void deleteButton_clicked();
    void combineButton_clicked();
};

#endif // textSearch_H
