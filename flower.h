#ifndef FLOWER_H
#define FLOWER_H

#include <QString>

struct Flower {

    int id;
    QString name;
    QString color;
    int price;
    int primeCost;
    int count;
    QString date;

    Flower(int id = 0, QString name = "",
           QString color = "", int price = 0,
           int primeCost = 0, int count = 0, QString date = "");
};

#endif // FLOWER_H
