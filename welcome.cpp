#include "welcome.h"
#include "ui_welcome.h"
#include "labeling.h"
#include "search.h"

Welcome::Welcome(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Welcome)
{
    ui->setupUi(this);
    QPixmap *pixmap = new QPixmap(":/pic/back.jpg");
    pixmap->scaled(ui->label->size(), Qt::KeepAspectRatio);
    ui->label->setScaledContents(true);
    ui->label->setPixmap(*pixmap);
    connect(ui->labelingButton,&QPushButton::clicked,this,&Welcome::labelingButton_clicked);
    connect(ui->searchButton,&QPushButton::clicked,this,&Welcome::searchButton_clicked);
}

Welcome::~Welcome()
{
    delete ui;
}

void Welcome::labelingButton_clicked()
{
    auto newWindow = new LabelWindow;
    newWindow->show();
}

void Welcome::searchButton_clicked()
{
    auto newWindow = new Search;
    newWindow->show();
}

