#ifndef OCRMENU_H
#define OCRMENU_H

#include <QWidget>

namespace Ui {
class ocrMenu;
}

class ocrMenu : public QWidget
{
    Q_OBJECT

public:
    explicit ocrMenu(QWidget *parent = nullptr);
    ~ocrMenu();

private:
    Ui::ocrMenu *ui;
};

#endif // OCRMENU_H
