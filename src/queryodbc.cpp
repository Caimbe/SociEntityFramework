#include "queryodbc.h"

QueryODBC::QueryODBC()
{

}

QueryODBC::~QueryODBC()
{

}

string QueryODBC::selectAllTables(string dbName)
{
    return "select * from sys.tables";
}

string QueryODBC::selectAllColumn(string table, string dbName)
{
    return "SELECT cols.DATA_TYPE, cols.COLUMN_NAME, refs.CONSTRAINT_NAME , refs.CONSTRAINT_NAME\
            FROM INFORMATION_SCHEMA.COLUMNS as cols\
            LEFT JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE AS refs\
            ON refs.TABLE_SCHEMA=cols.TABLE_SCHEMA\
                AND refs.TABLE_CATALOG=cols.TABLE_CATALOG \
                AND refs.TABLE_NAME=cols.TABLE_NAME\
                AND refs.COLUMN_NAME=cols.COLUMN_NAME\
            WHERE cols.TABLE_CATALOG='"+dbName+"'\
            AND cols.TABLE_NAME='"+table+"'";
}


