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
        RepositoryCreator(table, tableSchema, getColumns( getColumnsDB(table, dataBase, tableSchema) ), dataBase);
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
    file << "public:\n\tRepository();\n\n";
    file << "\tvoid open(std::string& connectStringDataBase);\n";
#ifdef SELECT_UNICO
    file << "\ttemplate<class T> T select(int id);\n";
#endif
    file << "\ttemplate<class T> T select(const string& where=\"\");\n";
    file << "\ttemplate<class T> int insert(const T& obj);\n";
    file << "\ttemplate<class T> void update(const T& obj);\n";
    file << "\ttemplate<class T> void remove(const T& obj);\n";
    file << "\n};\n\n";
    file << "#endif // REPOSITORY_H\n";
}

void ProcessDataBase::createInterfaceCpp(vector<string> vecTables)
{
    ofstream file(DIR_REPOSITORY"repository.cpp");
    file << "#include \"repository.h\"\nRepository::Repository() :";
    bool first=true;
    for(string table: vecTables)
    {
        if(first)
            file << boost::algorithm::to_lower_copy(table2className(table))<<"(dataBase) ";
        else
            file << ',' << boost::algorithm::to_lower_copy(table2className(table))<<"(dataBase) ";
        first = false;
    }
    file <<"\n{}\n";

    file << "void Repository::open(std::string& connectStringDataBase)\n{\n"
            "\tif(connectStringDataBase.size())\n"
            "\t\tdataBase.open(connectStringDataBase);\n"
            "\telse\n"
            "\t\tthrow runtime_error(\"connectStringDataBase is EMPTY, info a url to connect in data base\");\n"
            "}\n";

    for(string table: vecTables)
    {
#ifdef SELECT_UNICO
        file << "\ntemplate<> "<<table2className(table)<<"Ptr Repository::select(int id)\n{\n\t";
        file <<"return "<<boost::algorithm::to_lower_copy(table2className(table)) << ".select(id);\n}\n";
#endif
        file << "template<> "<<table2className(table)<<"List Repository::select(const string& where)\n{\n\t";
        file <<"return "<<boost::algorithm::to_lower_copy(table2className(table)) << ".select(where);\n}\n";

        file << "template<> int Repository::insert(const "<<table2className(table)<<"& obj)\n{\n\t";
        file <<"return "<<boost::algorithm::to_lower_copy(table2className(table)) << ".insert(obj);\n}\n";

        file << "template<> void Repository::update(const "<<table2className(table)<<"& obj)\n{\n\t";
        file <<boost::algorithm::to_lower_copy(table2className(table)) << ".update(obj);\n}\n";

        file << "template<> void Repository::remove(const "<<table2className(table)<<"& obj)\n{\n\t";
        file <<boost::algorithm::to_lower_copy(table2className(table)) << ".remove(obj);\n}\n";
    }
}



