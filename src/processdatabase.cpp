#include "processdatabase.h"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "util.hpp"
#include "entitycreator.h"
#include "repositorycreator.h"

ProcessDataBase::ProcessDataBase(const string& urlDataBase)
{
    size_t ini = urlDataBase.find("db=")+3;
    size_t fim = urlDataBase.find(" ", ini);
    tableSchema = urlDataBase.substr(ini, fim-ini);
    dataBase.open(urlDataBase);    

}

void ProcessDataBase::start()
{
    vector<string> vecTables;
    rowset<string> tables = dataBase.prepare << "SELECT table_name FROM information_schema.tables WHERE table_type = 'base table' AND table_schema='"+tableSchema+"'";
    for(string table: tables)
    {
        cout << "processing table: " << table << endl;
        EntityCreator(dataBase, table, tableSchema);
        RepositoryCreator(table, tableSchema, getColumns( getColumnsDB(table, dataBase, tableSchema) ));
        vecTables.push_back(table);
    }
    createInterfaceHeader(vecTables);
    createInterfaceCpp(vecTables);
}

void ProcessDataBase::createInterfaceHeader(vector<string> vecTables)
{
    ofstream file(DIR_REPOSITORY"repository.h");
    file << "#ifndef REPOSITORY_H\n#define REPOSITORY_H\n";

    file << "#include <soci/soci.h>\n#include <typeinfo>\n";
    for(string table: vecTables)
    {
        file << "#include \""<<boost::algorithm::to_lower_copy(table2className(table))<<"repository.h\"\n";
    }
    file << "\nclass Repository\n{\n\tsoci::session dataBase;\n";
    for(string table: vecTables)
    {
        file << '\t'<<table2className(table)<<"Repository "<<boost::algorithm::to_lower_copy(table2className(table))<<";\n";
    }
    file << "public:\n\tRepository(std::string connectStringDataBase);\n\n";
    file << "\ttemplate<class T> T select(int id);\n";
    file << "\n};\n\n";
    file << "#endif // REPOSITORY_H\n";
}

void ProcessDataBase::createInterfaceCpp(vector<string> vecTables)
{
    ofstream file(DIR_REPOSITORY"repository.cpp");
    file << "#include \"repository.h\"\nRepository::Repository(std::string connectStringDataBase) :";
    bool first=true;
    for(string table: vecTables)
    {
        if(first)
            file << boost::algorithm::to_lower_copy(table2className(table))<<"(dataBase) ";
        else
            file << ',' << boost::algorithm::to_lower_copy(table2className(table))<<"(dataBase) ";
        first = false;
    }
    file <<"\n{\n\tdataBase.open(connectStringDataBase);\n}\n";

    for(string table: vecTables)
    {
        file << "\ntemplate<> "<<table2className(table)<<"Ptr Repository::select(int id)\n{\n\t";
        file <<"return "<<boost::algorithm::to_lower_copy(table2className(table)) << ".select(id);\n}\n";
    }
}


