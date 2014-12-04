#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <soci/soci.h>
using namespace std;

#define DIR_ENTITY "entity/"
#define INCLUDES_DEFAULTS "#include <iostream>\n#include <memory>\n"

inline string typeDb2Cpp(string typeDB)
{
    if(typeDB == "varchar" || typeDB == "text" || typeDB == "tinytext" || typeDB == "blob")
        return "string";
    else if(typeDB == "timestamp")
        return "tm";
    else if(typeDB == "tinyint")
        return "bool";

    return typeDB;
}


inline soci::rowset<soci::row> getColumnsDB(string &table, soci::session& dataBase, string tableSchema)
{
    soci::rowset<soci::row> columns = dataBase.prepare << "SELECT cols.DATA_TYPE, cols.COLUMN_NAME, refs.REFERENCED_TABLE_NAME\
                                              FROM INFORMATION_SCHEMA.COLUMNS as cols\
                                              LEFT JOIN INFORMATION_SCHEMA.KEY_COLUMN_USAGE AS refs\
                                              ON refs.TABLE_SCHEMA=cols.TABLE_SCHEMA\
                                                  AND refs.REFERENCED_TABLE_SCHEMA=cols.TABLE_SCHEMA\
                                                  AND refs.TABLE_NAME=cols.TABLE_NAME\
                                                  AND refs.COLUMN_NAME=cols.COLUMN_NAME\
                                              WHERE cols.TABLE_SCHEMA='"+tableSchema+"'\
                                              AND cols.TABLE_NAME='"+table+"'";
    return columns;
}

struct Column
{
    string type;
    string var;
    string relation;
};

inline vector<Column> getColumns(soci::rowset<soci::row> columnsDb)
{
    vector<Column> vecColumns;
    string tipo;
    string var;
    string relation;
    size_t fid;
    for(soci::row& column: columnsDb)
    {
        tipo = typeDb2Cpp( column.get<string>(0) );
        var = column.get<string>(1);
        relation = column.get<string>(2, "");
        if(relation.size()){
            tipo = relation;
            tipo[0] = toupper(tipo[0]);
            tipo += "Ptr";

            fid = var.rfind("_id");
            fid = fid==string::npos ? var.rfind("Id"):fid;
            if(fid != string::npos)
                var.erase(fid);
        }
        vecColumns.push_back( {tipo, var, relation} );
    }
    return vecColumns;
}


#endif // UTIL_HPP
