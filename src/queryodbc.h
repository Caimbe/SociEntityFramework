#ifndef QUERYODBC_H
#define QUERYODBC_H

#include "query.h"

class QueryODBC : public Query
{
public:
    QueryODBC();
    ~QueryODBC();

    string selectAllTables(string dbName);
    string selectAllColumn(string table, string dbName);
};

#endif // QUERYODBC_H
