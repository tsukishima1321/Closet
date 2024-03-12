
#ifndef ITEM_H
#define ITEM_H

#include <QString>


class Item
{
public:
    Item();
    Item(QString date,QString href,QString des,QString type);
    QString href;
    QString description;
    QString date;
    QString type;
};

#endif // ITEM_H
