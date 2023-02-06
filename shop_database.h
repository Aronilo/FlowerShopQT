#ifndef SHOPDATABASE_H
#define SHOPDATABASE_H

#include <QObject>
#include <QSql>
#include <QSqlDatabase>
#include <QDate>

#include "worker.h"
#include "flower.h"

class ShopDatabase : public QObject
{
    Q_OBJECT
public:
    ShopDatabase(QString databasename,
                 QObject *parent = 0);

    void createTables();

    void addWorker(Worker worker);
    QList<Worker> workers();
    bool removeWorker(QString name);

    void addFlower(Flower flower);
    QList<Flower> flowers();
    void removeFlower(QString name);
    bool flowerById(int id, Flower& flower);

    void addSoldFlower(Flower flower);
    QList<Flower> soldFlowers();

    int sellFlowers(QString name, int count);

    int totalSalary();

    void clearTable(QString table);

    int getBalance();
    void updateBalance(int);

private:
    void exec(QString);
    QString qs(QString);
    QString qs(std::string);

    QSqlDatabase m_db;
    QSqlQuery *m_query;

signals:
    void updated();
};

#endif // SHOPDATABASE_H
