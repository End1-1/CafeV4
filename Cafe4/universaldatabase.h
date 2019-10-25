#ifndef UNIVERSALDATABASE_H
#define UNIVERSALDATABASE_H

#include <QSqlDatabase>

class UniversalDatabase
{
public:
    UniversalDatabase();

private:
    QSqlDatabase fDb;
};

#endif // UNIVERSALDATABASE_H
