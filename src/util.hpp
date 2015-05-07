#ifndef UTIL_HPP
#define UTIL_HPP

#include <string>
#include <fstream>
#include <soci/soci.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "entity.h"
using namespace std;

#define INCLUDES_DEFAULTS "#include <iostream>\n#include <memory>\n#include <vector>\n"


inline void insertIncludesRelations(ofstream &file, vector<Column>& vecColumns, string sufixFile = "", string dir="")
{
    for(Column column: vecColumns)
    {
        if(column.relation.size()){
            file << "#include \"" << dir << column.relation << sufixFile << ".h\"\n";
        }
    }
}

inline string geraDefinersOpen(string className)
{
    boost::algorithm::to_upper(className);
    return "#ifndef "+className+"_H\n#define "+className+"_H\n\n";
}

inline string geraDefinersClose(string className)
{
    boost::algorithm::to_upper(className);
    return "\n\n#endif // "+className+"_H\n";
}

inline string table2className(string& table)
{
    string className = table;
    className[0] = toupper(className[0]);
    for(size_t find = className.find("_"); find != string::npos; find=className.find("_"))
    {
        className.erase(find, 1);
        className[find] = toupper(className[find]);
    }
    return className;
}

inline string typeDb2Cpp(string typeDB)
{
    if(typeDB == "varchar" || typeDB == "text" || typeDB == "tinytext" || typeDB == "blob" || typeDB=="longtext")
        return "string";
    else if(typeDB == "timestamp")
        return "tm";
    else if(typeDB == "tinyint")
        return "bool";
    else if(typeDB == "decimal" || typeDB=="float")
        return "double";

    return typeDB;
}


inline soci::rowset<soci::row> getColumnsDB(string &table, soci::session& dataBase, string tableSchema)
{
    soci::rowset<soci::row> columns = dataBase.prepare << "SELECT cols.DATA_TYPE, cols.COLUMN_NAME, refs.REFERENCED_TABLE_NAME, cols.COLUMN_KEY\
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

inline vector<Column> getColumns(soci::rowset<soci::row> columnsDb)
{
    vector<Column> vecColumns;
    string tipoDb;
    string tipo;
    string var;
    string nameDb;
    string relation;
    string key;
    size_t fid;
    for(soci::row& column: columnsDb)
    {
        tipoDb =  column.get<string>(0);
        tipo = typeDb2Cpp( tipoDb );
        var = column.get<string>(1);
        nameDb = var;
        relation = column.get<string>(2, "");        
        key = column.get<string>(3, "");
        if(relation.size()){
            fid = var.rfind("_id");
            fid = fid==string::npos ? var.rfind("Id"):fid;
            if(fid != string::npos)
                var.erase(fid);

            tipo = relation;
            tipo[0] = toupper(tipo[0]);
            tipo += "Ptr";
        }
        vecColumns.push_back( {tipo, tipoDb, var, relation, key, nameDb} );
    }
    return vecColumns;
}


#endif // UTIL_HPP
