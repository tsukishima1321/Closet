#include "textdetailview.h"
#include "iconresources.h"
#include "ui_textdetailview.h"
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

textDetailView::textDetailView(QWidget *parent, QSqlDatabase &db) :
        Window(parent),
        ui(new Ui::textDetailView),
        db(db) {
    current = 0;

    ui->setupUi(this);
    ui->deleteButton->setIcon(IconResources::getIcons()["trash"]);
    ui->status->setText("新建记录");

    connect(ui->commitButton, &QPushButton::clicked, this, &textDetailView::commitChange);
    connect(ui->cancelButton, &QPushButton::clicked, this, &textDetailView::cancelChange);
    connect(ui->deleteButton, &QToolButton::clicked, this, &textDetailView::deleteText);
    connect(ui->textEdit, QTextEdit::textChanged, this, [this]() {
        if (ui->status->text() != "新建记录") {
            ui->status->setText("修改未提交");
        }
    });
    connect(ui->dateEdit, QDateEdit::dateChanged, this, [this]() {
        if (ui->status->text() != "新建记录") {
            ui->status->setText("修改未提交");
        }
    });
}

void textDetailView::OpenText(int id) {
    current = id;
    QSqlQuery query(db);
    QString sql = "select * from texts where id=:id;";
    query.prepare(sql);
    query.bindValue(":id", id);
    query.exec();
    if (query.next()) {
        ui->textEdit->setText(query.value("text").toString());
        ui->dateEdit->setDate(query.value("date").toDate());
    }
    ui->status->setText("未修改");
}

void textDetailView::commitChange() {
    if (current != 0) {
        QSqlQuery query(db);
        QString sql = "select * from texts where id=:id;";
        query.prepare(sql);
        query.bindValue(":id", current);
        query.exec();
        if (query.next()) {
            sql = "update texts set text=:text,date=:date where id=:id;";
        }
        query.prepare(sql);
        query.bindValue(":id", current);
        query.bindValue(":text", ui->textEdit->toPlainText());
        query.bindValue(":date", ui->dateEdit->text());
        if (!query.exec()) {
            QSqlError sqlerror = query.lastError();
            qDebug() << sqlerror.nativeErrorCode();
            QString errortext = sqlerror.text();
            qDebug() << errortext;
            if (errortext == "") {
                errortext = "empty query";
            }
            QMessageBox::critical(this, "错误", errortext);
        } else {
            ui->status->setText("修改已提交");
            QMessageBox::information(this, "成功", "修改成功");
        }
    } else {
        QSqlQuery query(db);
        QString sql = "insert into texts (date,text) values(:date,:text);";
        query.prepare(sql);
        query.bindValue(":text", ui->textEdit->toPlainText());
        query.bindValue(":date", ui->dateEdit->text());
        if (!query.exec()) {
            QSqlError sqlerror = query.lastError();
            qDebug() << sqlerror.nativeErrorCode();
            QString errortext = sqlerror.text();
            qDebug() << errortext;
            if (errortext == "") {
                errortext = "empty query";
            }
            QMessageBox::critical(this, "错误", errortext);
        } else {
            ui->status->setText("新建已提交");
            QMessageBox::information(this, "成功", "添加成功");
            current = query.lastInsertId().toInt();
        }
    }
    emit edit();
}

void textDetailView::cancelChange() {
    if (current != 0) {
        QSqlQuery query(db);
        QString sql = "select * from texts where id=:id;";
        query.prepare(sql);
        query.bindValue(":id", current);
        query.exec();
        if (query.next()) {
            ui->textEdit->setText(query.value("text").toString());
            ui->dateEdit->setDate(query.value("date").toDate());
        }
        ui->status->setText("未修改");
    } else {
        ui->textEdit->clear();
        ui->dateEdit->setDate(QDate::currentDate());
    }
}

void textDetailView::deleteText() {
    if (current == 0) {
        return;
    }
    int res = QMessageBox::warning(this, "确认",
                                   "确定删除" + QString::number(current) + "：" + ui->dateEdit->text() + "吗？",
                                   QMessageBox::Yes | QMessageBox::Cancel,
                                   QMessageBox::Cancel);
    if (res == QMessageBox::Yes) {
        QSqlQuery query(db);
        query.prepare("delete from texts where id=:id;");
        query.bindValue(":id", current);
        query.exec();
        emit edit();
        this->close();
        this->deleteLater();
    }
}

void textDetailView::setDate(QDate date) {
    ui->dateEdit->setDate(date);
}

void textDetailView::setText(QString text) {
    ui->textEdit->setText(text);
}

void textDetailView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_S && event->modifiers() == Qt::ControlModifier) {
        commitChange();
    }
    Window::keyPressEvent(event);
}

void textDetailView::closeEvent(QCloseEvent *event) {
    if (ui->status->text() == "修改未提交" || ui->status->text() == "新建记录") {
        QMessageBox box(QMessageBox::Information, "关闭", "有未提交的修改，是否保存？");
        QAbstractButton *yesButton = box.addButton("保存", QMessageBox::AcceptRole);
        QAbstractButton *noButton = box.addButton("不保存", QMessageBox::RejectRole);
        QAbstractButton *cancelButton = box.addButton("取消", QMessageBox::RejectRole);
        box.exec();
        if (box.clickedButton() == cancelButton) {
            event->ignore();
            return;
        } else if (box.clickedButton() == noButton) {
            return Window::closeEvent(event);
        } else if (box.clickedButton() == yesButton) {
            commitChange();
            return Window::closeEvent(event);
        } else {
            event->ignore();
            return;
        }
    } else {
        return Window::closeEvent(event);
    }
}

textDetailView::~textDetailView() {
    delete ui;
}