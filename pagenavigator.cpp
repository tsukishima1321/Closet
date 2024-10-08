﻿//在此基础上修改而来：http://www.cppblog.com/biao/archive/2011/10/30/159350.html
#include "pagenavigator.h"
#include "qvalidator.h"
#include "ui_pagenavigator.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QtGlobal>

PageNavigator::PageNavigator(QWidget *parent /* = nullptr*/, int blockSize /* = 3*/) :
        QWidget(parent), ui(new Ui::PageNavigator) {
    ui->setupUi(this);
    setBlockSize(blockSize);
    initialize();

    m_maxPage = 0;
    setMaxPage(1);
    QString qss = QString(".QLabel[page=\"true\"] { padding: 6px; }") + QString(".QLabel[currentPage=\"true\"] { color: rgb(190, 0, 0);}") + QString(".QLabel[page=\"true\"]:hover { color: white; border-radius: 4px; background-color: qlineargradient(spread:reflect, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(53, 121, 238, 255), stop:1 rgba(0, 202, 237, 255));}");
    this->setStyleSheet(qss);
}

PageNavigator::~PageNavigator() {
    delete ui;
    delete m_pageLabels;
}

bool PageNavigator::eventFilter(QObject *watched, QEvent *e) {
    if (e->type() == QEvent::MouseButtonRelease) {
        int page = -1;
        if (watched == ui->previousPageLabel) {
            page = getCurrentPage() - 1;
        }

        if (watched == ui->nextPageLabel) {
            page = getCurrentPage() + 1;
        }

        for (int i = 0; i < m_pageLabels->count(); ++i) {
            if (watched == m_pageLabels->at(i)) {
                page = m_pageLabels->at(i)->text().toInt();
                break;
            }
        }

        if (-1 != page) {
            if (!ui->pageLineEdit->text().isEmpty()) {
                ui->pageLineEdit->clear();
            }
            setCurrentPage(page, true);
            return true;
        }
    }

    if (watched == ui->pageLineEdit && e->type() == QEvent::KeyRelease) {
        QKeyEvent *ke = static_cast<QKeyEvent *>(e);
        if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return) {
            setCurrentPage(ui->pageLineEdit->text().toInt(), true);
            //setMaxPage(ui->pageLineEdit->text().toInt()); // 测试生成多个页码
            return true;
        }
    }

    return QWidget::eventFilter(watched, e);
}

int PageNavigator::getBlockSize() const {
    return m_blockSize;
}

int PageNavigator::getMaxPage() const {
    return m_maxPage;
}

int PageNavigator::getCurrentPage() const {
    return m_currentPage;
}

void PageNavigator::setMaxPage(int page) {
    page = qMax(page, 1);

    if (m_maxPage != page) {
        m_maxPage = page;
        m_currentPage = 1;
        updatePageLabels();
    }
}

void PageNavigator::setCurrentPage(int page, bool signalEmitted /* = false*/) {
    page = qMax(page, 1);
    page = qMin(page, m_maxPage);

    if (page != m_currentPage) {
        m_currentPage = page;
        updatePageLabels();

        if (signalEmitted) {
            emit currentPageChanged(page);
        }
    }
}

void PageNavigator::setBlockSize(int blockSize) {
    // 为了便于计算, block size 必须是奇数, 且最小为3
    blockSize = qMax(blockSize, 3);
    if (blockSize % 2 == 0) {
        ++blockSize;
    }
    m_blockSize = blockSize;
}

QString pageToText(int page) {
    return QString::number(page);
}

// 初始化页码的labels
// 分成三个部分, 左...中...右
void PageNavigator::initialize() {
    ui->pageLineEdit->installEventFilter(this);
    ui->pageLineEdit->setValidator(new QIntValidator(1, 10000000, this));

    ui->nextPageLabel->setProperty("page", "true");
    ui->previousPageLabel->setProperty("page", "true");
    ui->nextPageLabel->installEventFilter(this);
    ui->previousPageLabel->installEventFilter(this);

    m_pageLabels = new QList<QLabel *>();

    QHBoxLayout *leftLayout = new QHBoxLayout();
    QHBoxLayout *centerLayout = new QHBoxLayout();
    QHBoxLayout *rightLayout = new QHBoxLayout();
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(0);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);

    for (int i = 0; i < m_blockSize * 3; ++i) {
        QLabel *label = new QLabel(pageToText(i + 1), this);
        label->setProperty("page", "true");
        label->installEventFilter(this);

        m_pageLabels->append(label);

        if (i < m_blockSize) {
            leftLayout->addWidget(label);
        } else if (i < m_blockSize * 2) {
            centerLayout->addWidget(label);
        } else {
            rightLayout->addWidget(label);
        }
    }

    ui->leftPagesWidget->setLayout(leftLayout);
    ui->centerPagesWidget->setLayout(centerLayout);
    ui->rightPagesWidget->setLayout(rightLayout);
}

void PageNavigator::updatePageLabels() {
    ui->leftSeparateLabel->hide();
    ui->rightSeparateLabel->hide();

    if (m_maxPage <= m_blockSize * 3) {
        for (int i = 0; i < m_pageLabels->count(); i += 1) {
            QLabel *label = m_pageLabels->at(i);
            if (i < m_maxPage) {
                label->setText(pageToText(i + 1));
                label->show();
            } else {
                label->hide();
            }

            if (m_currentPage - 1 == i) {
                label->setProperty("currentPage", "true");
            } else {
                label->setProperty("currentPage", "false");
            }

            label->setStyleSheet("/**/");
        }
        return;
    }

    // 以下情况为maxPageNumber大于blockSize * 3, 所有的页码label都要显示
    // c 为 currentPage
    // n 为 block size
    // m 为 maxPage

    // 1. c ∈ [1, n + n/2 + 1]: 显示前 n * 2 个, 后 n 个: 只显示右边的分隔符
    // 2. c ∈ [m - n - n/2, m]: 显示前 n 个, 后 n * 2 个: 只显示左边的分隔符
    // 3. 显示[1, n], [c - n/2, c + n/2], [m - 2*n + 1, m]: 两个分隔符都显示

    int c = m_currentPage;
    int n = m_blockSize;
    int m = m_maxPage;
    int centerStartPage = 0;
    if (c >= 1 && c <= n + n / 2 + 1) {
        // 1. c ∈ [1, n + n/2 + 1]: 显示前 n * 2 个, 后 n 个: 只显示右边的分隔符
        centerStartPage = n + 1;
        ui->rightSeparateLabel->show();
    } else if (c >= m - n - n / 2 && c <= m) {
        // 2. c ∈ [m - n - n/2, m]: 显示前 n 个, 后 n * 2 个: 只显示左边的分隔符
        centerStartPage = m - n - n + 1;
        ui->leftSeparateLabel->show();
    } else {
        // 3. 显示[1, n], [c - n/2, c + n/2], [m - n + 1, m]: 两个分隔符都显示
        centerStartPage = c - n / 2;
        ui->rightSeparateLabel->show();
        ui->leftSeparateLabel->show();
    }

    for (int i = 0; i < n; ++i) {
        m_pageLabels->at(i)->setText(pageToText(i + 1));                   // 前面 n 个
        m_pageLabels->at(n + i)->setText(pageToText(centerStartPage + i)); // 中间 n 个
        m_pageLabels->at(3 * n - i - 1)->setText(pageToText(m - i));       // 后面 n 个
    }

    for (int i = 0; i < m_pageLabels->count(); ++i) {
        QLabel *label = m_pageLabels->at(i);
        int page = label->text().toInt();
        if (page == m_currentPage) {
            label->setProperty("currentPage", "true");
        } else {
            label->setProperty("currentPage", "false");
        }

        label->setStyleSheet("/**/");
        label->show();
    }
}
