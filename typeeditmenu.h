#ifndef TYPEEDITMENU_H
#define TYPEEDITMENU_H

#include <QWidget>

namespace Ui {
class typeEditMenu;
}

class typeEditMenu : public QWidget
{
    Q_OBJECT

public:
    explicit typeEditMenu(QWidget *parent = nullptr);
    ~typeEditMenu();

private:
    Ui::typeEditMenu *ui;
};

#endif // TYPEEDITMENU_H
