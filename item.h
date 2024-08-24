
#ifndef ITEM_H
#define ITEM_H

#include <QString>

class Item {
public:
    Item(){};
    Item(QString date, QString href, QString des, QString type) :
            href(href), description(des), date(date), type(type){};
    QString href;
    QString description;
    QString date;
    QString type;
};

#endif // ITEM_H
