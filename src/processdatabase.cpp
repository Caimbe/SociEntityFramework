#include "processdatabase.h"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "util.hpp"
#include "entitycreator.h"

ProcessDataBase::ProcessDataBase(const string& urlDataBase)
{
    size_t ini = urlDataBase.find("db=")+3;
    size_t fim = urlDataBase.find(" ", ini);
    tableSchema = urlDataBase.substr(ini, fim-ini);
    dataBase.open(urlDataBase);    

}

void ProcessDataBase::start()
{
    boost::filesystem::create_directories(DIR_ENTITY);
    rowset<string> tables = dataBase.prepare << "SELECT table_name FROM information_schema.tables WHERE table_type = 'base table' AND table_schema='"+tableSchema+"'";
    for(string table: tables)
    {
        cout << "processing table: " << table << endl;
        EntityCreator(dataBase, table, tableSchema);
    }
}


