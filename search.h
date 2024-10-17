#ifndef SEARCH_H
#define SEARCH_H

#include "itemmodel.h"
#include <QLayout>
#include <QList>
#include <QMainWindow>
#include <QRunnable>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <imagepreviewform.h>
#include <window.h>

constexpr int pageSize = 20;
//constexpr int pageSizeTable = 100;

namespace Ui {
    class Search;
}

class Search : public Window {
    Q_OBJECT

public:
    explicit Search(QWidget *parent, QSqlDatabase &db);
    ~Search();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Ui::Search *ui;
    int currentPage;
    int currentColumnCount;
    int pageSizeTable;
    QHBoxLayout *hBoxLayout;
    QList<QVBoxLayout *> vBoxLayouts;

    QSqlDatabase &db;
    itemModel queryModel;
    QString currentConditon;
    imagePreviewForm *preViewList;
    void searchButton_clicked();
    void updateSearch();
    imagePreviewForm *addImgItem(QString href, QString des);
    void openDetailMenu(QString href);
    void updateImgView();
    void updateTableView();
    void locateImg();
    void tableCellDoubleClicked(const QModelIndex &index);
    void deleteButton_clicked();
    void exportButton_clicked();
    void sendSQL(QString text);
};

class imgLoader : public QObject, public QRunnable {
    Q_OBJECT
public:
    imgLoader(imagePreviewForm *target, QImageReader *reader, QString href, QString des);
    imagePreviewForm *target;
    QImageReader *reader;
    QString href;
    QString des;
    void run() override;
signals:
    void loadReady();
};

#endif // SEARCH_H
