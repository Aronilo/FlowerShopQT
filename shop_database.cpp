#include "shop_database.h"

#include <QSqlQuery>

#include "worker.h"
#include "flower.h"

ShopDatabase::ShopDatabase(QString databasename,
                           QObject *parent) : QObject(parent) {
    m_db = QSqlDatabase::addDatabase("QSQLITE", databasename);
    m_db.setDatabaseName(databasename);

    if (false == m_db.open())
        return;

    m_query = new QSqlQuery(m_db);
}


QString ShopDatabase::qs(QString str) {
    return "'" + str + "'";
}

QString ShopDatabase::qs(std::string str) {
    return qs(QString::fromStdString(str));
}

void ShopDatabase::exec(QString str) {
    if (!m_query->exec(str))
        return;
}

void ShopDatabase::createTables() {
    if (!m_db.tables().contains("flowers")) {
      exec("CREATE TABLE flowers"
           "("
              "id INTEGER PRIMARY KEY AUTOINCREMENT, "
              "name TEXT NOT NULL, "
              "color TEXT NOT NULL, "
              "price INTEGER NOT NULL, "
              "prime_cost INTEGER NOT NULL, "
              "count INTEGER NOT NULL"
           ");"
      );
    }

    if (!m_db.tables().contains("sold_flowers")) {
      exec("CREATE TABLE sold_flowers"
           "("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "color TEXT NOT NULL, "
               "price INTEGER NOT NULL, "
               "prime_cost INTEGER NOT NULL, "
               "count INTEGER NOT NULL, "
               "date TEXT NOT NULL"
          ");"
      );
    }

    if (!m_db.tables().contains("workers")) {
      exec("CREATE TABLE workers"
           "("
              "id INTEGER PRIMARY KEY, "
              "name TEXT NOT NULL, "
              "salary INTEGER NOT NULL"
          ");"
      );
    }
    if (!m_db.tables().contains("balance")) {
      exec("CREATE TABLE balance"
           "("
              "value INTEGER NOT NULL"
          ");"
      );

      exec("INSERT INTO balance(value) VALUES (10000)");
    }
}

int ShopDatabase::getBalance() {
    QString query = tr("SELECT value FROM balance ");
    exec(query);

    if (m_query->next()) {
        return m_query->value(0).toInt();
    }

    return 0;
}

void ShopDatabase::updateBalance(int balance) {
    exec("UPDATE balance SET value = " + QString::number(balance));
}

void ShopDatabase::clearTable(QString table) {

    QString query = tr("DELETE FROM ") + table + ";";

    exec(query);

    emit updated();
}

void ShopDatabase::addWorker(Worker worker) {
    QString query = tr("INSERT INTO workers(name, salary) VALUES (")
                        + qs(worker.name) + ","
                        + QString::number(worker.salary) + ")";
    exec(query);
    emit updated();
}

QList<Worker> ShopDatabase::workers() {
    QList<Worker> workers;

    QString query = tr("SELECT id, name, salary FROM workers ");
    exec(query);

    while (m_query->next()) {
        int id = m_query->value(0).toInt();
        QString name = m_query->value(1).toString();
        int salary = m_query->value(2).toInt();

        workers.push_back(Worker(id, name, salary));
    }

    return workers;
}

bool ShopDatabase::removeWorker(QString name) {
    exec(tr("SELECT name FROM workers WHERE name = ") + qs(name));

    if (!m_query->next())
        return false;

    QString query = tr("DELETE FROM workers WHERE name = ") + qs(name);
    exec(query);
    emit updated();

    return true;
}

int ShopDatabase::totalSalary() {
    exec(
          tr("SELECT SUM(salary) FROM workers")
    );
    m_query->next();

    return m_query->value(0).toInt();
}

void ShopDatabase::addFlower(Flower flower) {
    QString query = tr("INSERT INTO flowers(name, color, price, prime_cost, count) VALUES (")
                        + qs(flower.name) + ","
                        + qs(flower.color) + ","
                        + QString::number(flower.price) + ","
                        + QString::number(flower.primeCost) + ","
                        + QString::number(flower.count) + ")";
    exec(query);
    emit updated();
}

QList<Flower> ShopDatabase::flowers() {
    QList<Flower> flowers;

    QString query = tr("SELECT id, name, color, price, prime_cost, count FROM flowers ");
    exec(query);

    while (m_query->next()) {
        Flower flower;
        flower.id = m_query->value(0).toInt();
        flower.name = m_query->value(1).toString();
        flower.color = m_query->value(2).toString();
        flower.price = m_query->value(3).toInt();
        flower.primeCost = m_query->value(4).toInt();
        flower.count = m_query->value(5).toInt();

        flowers.push_back(flower);
    }
    return flowers;
}

void ShopDatabase::removeFlower(QString name) {
    QString query = tr("DELETE FROM flowers WHERE name = ") + qs(name);
    exec(query);
    emit updated();
}

bool ShopDatabase::flowerById(int id, Flower& flower) {
    QString query = tr("SELECT id, name, color, price, prime_cost, count "
                     "FROM flowers WHERE id = ") + QString::number(id);
    exec(query);

    if (!m_query->next()) {
        return false;
    }

    flower.id = m_query->value(0).toInt();
    flower.name = m_query->value(1).toString();
    flower.color = m_query->value(2).toString();
    flower.price = m_query->value(3).toInt();
    flower.primeCost = m_query->value(4).toInt();
    flower.count = m_query->value(5).toInt();

    return true;
}

void ShopDatabase::addSoldFlower(Flower flower) {
    QString query = tr("INSERT INTO sold_flowers(name, color, price, prime_cost, count, date) VALUES (")
                        + qs(flower.name) + ","
                        + qs(flower.color) + ","
                        + QString::number(flower.price) + ","
                        + QString::number(flower.primeCost) + ","
                        + QString::number(flower.count) + ")";
    exec(query);
    emit updated();
}

QList<Flower> ShopDatabase::soldFlowers() {
    QList<Flower> flowers;

    QString query = tr("SELECT id, name, color, price, prime_cost, count, date FROM sold_flowers ");
    exec(query);

    while (m_query->next()) {
        Flower flower;
        flower.id = m_query->value(0).toInt();
        flower.name = m_query->value(1).toString();
        flower.color = m_query->value(2).toString();
        flower.price = m_query->value(3).toInt();
        flower.primeCost = m_query->value(4).toInt();
        flower.count = m_query->value(5).toInt();
        flower.date = m_query->value(6).toString();

        flowers.push_back(flower);
    }
    return flowers;
}

int ShopDatabase::sellFlowers(QString name, int count) {
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QString query = tr("SELECT id, count FROM flowers WHERE ") +
                     "name = " + qs(name) +
                     tr(" AND count >= ") + QString::number(count);

    exec(query);

    int id;

    if (m_query->next()) {
        id = m_query->value(0).toInt();
    } else {
        return -1;
    }


    Flower flower;
    flowerById(id, flower);

    query = tr("INSERT INTO sold_flowers")
         + tr(" (name, color, price, prime_cost, count, date)")
         + tr(" VALUES (")
         + qs(flower.name) + ","
         + qs(flower.color) + ","
         + QString::number(flower.price) + ","
         + QString::number(flower.primeCost) + ","
         + QString::number(count) + ","
         + qs(date) + ")";

    exec(query);

    if (count == flower.count) {
        query = tr("DELETE FROM flowers WHERE id = ") + QString::number(id);
    } else {
        query = tr("UPDATE flowers SET count = ")
                   + QString::number(flower.count - count)
                   + " WHERE id = "
                   + QString::number(id);
    }

    exec(query);

    emit updated();

    return flower.primeCost * count;
}
