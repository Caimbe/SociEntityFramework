#ifndef QUERY_H
#define QUERY_H

#include <string>
#include <memory>
using namespace std;


class Query
{
public:
    Query();
    ~Query();

    virtual string selectAllTables(string dbName);
    virtual string selectAllColumn(string table, string dbName);
};

typedef shared_ptr<Query> QueryPtr;

#endif // QUERY_H
