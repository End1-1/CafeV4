#include "universaldatabase.h"

static int fDbNum = 0;

UniversalDatabase::UniversalDatabase()
{
    fDb = QSqlDatabase::addDatabase("QIBASE", QString::number(fDbNum++));
}
