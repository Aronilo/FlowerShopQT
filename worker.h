#ifndef WORKER_H
#define WORKER_H

#include <QString>

struct Worker {
    int id;
    QString name;
    int salary;

    Worker(int, QString, int);
};

#endif // WORKER_H
