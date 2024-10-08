﻿#ifndef PAGENAVIGATOR_H
#define PAGENAVIGATOR_H

#include <QLabel>
#include <QList>
#include <QWidget>

//
namespace Ui {
    class PageNavigator;
}

class PageNavigator : public QWidget {
    Q_OBJECT

public:
    explicit PageNavigator(QWidget *parent = nullptr, int blockSize = 3);
    ~PageNavigator();

    int getBlockSize() const;
    int getMaxPage() const;
    int getCurrentPage() const;

    // 其他组件只需要调用这两个函数即可
    void setMaxPage(int page);                                 // 当总页数改变时调用
    void setCurrentPage(int page, bool signalEmitted = false); // 修改当前页时调用

protected:
    virtual bool eventFilter(QObject *watched, QEvent *e);

signals:
    void currentPageChanged(int page);

private:
    Ui::PageNavigator *ui;
    int m_blockSize;
    int m_maxPage;
    int m_currentPage;
    QList<QLabel *> *m_pageLabels;

    void setBlockSize(int blockSize);
    void updatePageLabels();
    void initialize();
};

#endif // PAGENAVIGATOR_H
