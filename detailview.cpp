#include "detailview.h"
#include "ui_detailview.h"
#include <QSqlError>
#include <QMessageBox>

DetailView::DetailView(QWidget *parent, QSqlDatabase& db, QString base) :
    QMainWindow(parent),
    base(base),
    ui(new Ui::DetailView),
    db(db)
{
    ui->setupUi(this);
    connect(ui->zoomInButton,&QPushButton::clicked,ui->imgWidget,&ImageViewWidget::onZoomInImage);
    connect(ui->zoomOutButton,&QPushButton::clicked,ui->imgWidget,&ImageViewWidget::onZoomOutImage);
    connect(ui->zoomResetButton,&QPushButton::clicked,ui->imgWidget,&ImageViewWidget::onPresetImage);
    connect(ui->commitButton,&QPushButton::clicked,this,&DetailView::commitChange);
    connect(ui->cancelButton,&QPushButton::clicked,this,&DetailView::cancelChange);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    QString sql = "SELECT * FROM types";
    QSqlQuery query(db);
    query.prepare(sql);
    query.exec();
    while (query.next()) {
        QString type=query.value("typename").toString();
        ui->typeSelect->addItem(type);
    }
}

void DetailView::OpenImg(QString href)
{
    ui->imgWidget->loadImage(base + href);
    QSqlQuery query(db);
    query.prepare("select * from pictures where href=:href");
    query.bindValue(":href",href);
    if(!query.exec()){
        QSqlError sqlerror= query.lastError();
        qDebug()<<sqlerror.nativeErrorCode();
        QString errortext=sqlerror.text();
        qDebug()<<errortext;
        if(errortext==""){
            errortext="empty query";
        }
        QMessageBox::critical(this, "错误", errortext);
    }else{
        query.next();
        ui->dateText->setText(query.value("date").toString());
        ui->desText->setText(query.value("description").toString());
        ui->typeSelect->setCurrentText(query.value("type").toString());
    }
}

DetailView::~DetailView()
{
    delete ui;
}
