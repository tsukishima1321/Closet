#ifndef SEARCH_H
#define SEARCH_H

#include "itemmodel.h"
#include "window.h"
#include <QLayout>
#include <QList>
#include <QMainWindow>
#include <QRunnable>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <imgpreviewform.h>
#include <span>

namespace imgViewConstants {
    constexpr int pageSize = 20;
}
// constexpr int pageSizeTable = 100;

namespace Ui {
    class ImgSearch;
}

class ImgSearch : public Window {
    Q_OBJECT

public:
    explicit ImgSearch(QWidget *parent, QSqlDatabase &db);
    ~ImgSearch();

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Ui::ImgSearch *ui;
    int currentPage;
    int currentColumnCount;
    int pageSizeTable;
    QHBoxLayout *hBoxLayout;
    QList<QVBoxLayout *> vBoxLayouts;

    QSqlDatabase &db;
    ItemModel queryModel;
    ItemModel countModel;
    ImgPreviewForm *preViewList;
    std::span<ImgPreviewForm> preViewListSpan;
    void searchButton_clicked();
    void updateSearch();
    ImgPreviewForm *addImgItem(QSqlRecord record, QModelIndex index);
    void openDetailMenu(QString href, int row);
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
    imgLoader(ImgPreviewForm *target, QImageReader *reader, QSqlRecord record, QModelIndex index);
    void run() override;

private:
    ImgPreviewForm *target;
    QImageReader *reader;
    QSqlRecord record;
    QModelIndex index;

signals:
    void loadReady();
};

#endif // SEARCH_H
