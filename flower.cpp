#include "flower.h"

Flower::Flower(int id, QString name, QString color,
               int price, int primeCost, int count, QString date)
    : id(id)
    , name(name)
    , color(color)
    , price(price)
    , primeCost(primeCost)
    , count(count)
    , date(date)
{}
