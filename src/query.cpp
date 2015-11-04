#include "query.h"

Query::Query()
{

}

Query::~Query()
{

}

string Query::selectAllTables(string dbName)
{
    return "SELECT table_name FROM information_schema.tables WHERE table_type = 'base table' AND table_schema='"+dbName+"'";
}

string Query::selectAllColumn(string table, string dmName)
{
    return "SELECT cols.DATA_TYPE, cols.COLUMN_NAME, refs.REFERENCED_TABLE_NAME, cols.COLUMN_KEY\
            FROM INFORMATION_SCHEMA.COLUMNS as cols\
            LEFT JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE AS refs\
            ON refs.TABLE_SCHEMA=cols.TABLE_SCHEMA\
                AND refs.REFERENCED_TABLE_SCHEMA=cols.TABLE_SCHEMA\
                AND refs.TABLE_NAME=cols.TABLE_NAME\
                AND refs.COLUMN_NAME=cols.COLUMN_NAME\
            WHERE cols.TABLE_SCHEMA='"+dmName+"'\
            AND cols.TABLE_NAME='"+table+"'";
}


