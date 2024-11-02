
#ifndef ITEM_H
#define ITEM_H

#include <QSqlRecord>
#include <QString>
#include <QVariant>

class Item {
public:
    Item(){};
    Item(QString date, QString href, QString des, QString type, QString ocr = "") :
            href(href), description(des), date(date), type(type), ocr(ocr){};
    Item(const QSqlRecord &record) {
        if (record.value("href").isValid())
            href = record.value("href").toString();
        if (record.value("description").isValid())
            description = record.value("description").toString();
        if (record.value("date").isValid())
            date = record.value("date").toString();
        if (record.value("type").isValid())
            type = record.value("type").toString();
        if (record.value("ocr_result").isValid())
            ocr = record.value("ocr_result").toString();
    }
    QString href;
    QString description;
    QString date;
    QString type;
    QString ocr;
};

#endif // ITEM_H
