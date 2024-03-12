
#include "item.h"

Item::Item()
{

}

Item::Item(QString date, QString href, QString des, QString type){
    this->date=date;
    this->href=href;
    this->description=des;
    this->type=type;
}

