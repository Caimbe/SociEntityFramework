#include "processdatabase.h"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include "util.hpp"
#include "entitycreator.h"
#include "repositorycreator.h"
#include "entity.h"

ProcessDataBase::ProcessDataBase(const Options& opt)
{
    size_t ini = opt.urlDataBase.find("db=")+3;
    size_t fim = opt.urlDataBase.find(" ", ini);
    tableSchema = opt.urlDataBase.substr(ini, fim-ini);
    dataBase.open(opt.urlDataBase);
    options = opt;
}

void ProcessDataBase::start()
{
    vector<Entity> vecEntity;
    vector<string> vecTables;
    rowset<string> tables = dataBase.prepare << "SELECT table_name FROM information_schema.tables WHERE table_type = 'base table' AND table_schema='"+tableSchema+"'";
    for(string table: tables)
    {
        Entity entity;
        entity.name = table;
        auto rowset = getColumnsDB(table, dataBase, tableSchema);
        entity.vecColumn = getColumns( rowset, dataBase, table );
        vecEntity.push_back(entity);
    }
    for(Entity& entity: vecEntity)
    {
        cout << "processing table: " << entity.name << endl;
        EntityCreator(dataBase, entity.name, tableSchema, vecEntity);
        RepositoryCreator(entity.name, tableSchema, getColumns( getColumnsDB(entity.name, dataBase, tableSchema), dataBase, entity.name ), dataBase);
        vecTables.push_back(entity.name);
    }
    createInterfaceHeader(vecTables);
    createInterfaceCpp(vecTables);
}

void ProcessDataBase::createInterfaceHeader(vector<string> vecTables)
{
    ofstream file(DIR_REPOSITORY"repository"+options.sufixRepository+".h");
    file << "#ifndef REPOSITORY"+options.sufixRepository+"_H\n#define REPOSITORY"+options.sufixRepository+"_H\n";

    file << "#include <soci/soci.h>\n#include <typeinfo>\n";
    for(string table: vecTables)
    {
        file << "#include \""<<boost::algorithm::to_lower_copy(table2className(table))<<"repository.h\"\n";
    }
    file << "\nclass Repository"+options.sufixRepository+"\n{\n\tsoci::session dataBase;\n";
    for(string table: vecTables)
    {
        file << '\t'<<table2className(table)<<"Repository "<<boost::algorithm::to_lower_copy(table2className(table))<<";\n";
    }
    file << "public:\n\tRepository"+options.sufixRepository+"();\n\n";
    file << "\tvoid open(const std::string& connectStringDataBase);\n";
    file << "\ttemplate<class R, class T> R select(const T& obj);\n";
    file << "\ttemplate<class T> T select(const string& where=\"\");\n";
    file << "\ttemplate<class T> int insert(const T& obj);\n";
    file << "\ttemplate<class T> void update(const T& obj);\n";
    file << "\ttemplate<class T> void update(const T& oldObj, const T& newObj);\n";
    file << "\ttemplate<class T> void remove(const T& obj);\n";
    file << "\n};\n\n";
    file << "#endif // REPOSITORY"+options.sufixRepository+"_H\n";
}

void ProcessDataBase::createInterfaceCpp(vector<string> vecTables)
{
    ofstream file(DIR_REPOSITORY"repository"+options.sufixRepository+".cpp");
    file << "#include \"repository"+options.sufixRepository+".h\"\nRepository"+options.sufixRepository+"::Repository"+options.sufixRepository+"() :";
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

    file << "void Repository"+options.sufixRepository+"::open(const std::string& connectStringDataBase)\n{\n"
            "\tif(connectStringDataBase.size())\n"
            "\t\tdataBase.open(connectStringDataBase);\n"
            "\telse\n"
            "\t\tthrow runtime_error(\"connectStringDataBase is EMPTY, info a url to connect in data base\");\n"
            "}\n";

    for(string table: vecTables)
    {
        file << "\ntemplate<> "<<table2className(table)<<"Ptr Repository"+options.sufixRepository+"::select(const "<<table2className(table)<<"& obj)\n{\n\t";
        file <<"return "<<boost::algorithm::to_lower_copy(table2className(table)) << ".select(obj);\n}\n";

        file << "template<> "<<table2className(table)<<"List Repository"+options.sufixRepository+"::select(const string& where)\n{\n\t";
        file <<"return "<<boost::algorithm::to_lower_copy(table2className(table)) << ".select(where);\n}\n";

        file << "template<> int Repository"+options.sufixRepository+"::insert(const "<<table2className(table)<<"& obj)\n{\n\t";
        file <<"return "<<boost::algorithm::to_lower_copy(table2className(table)) << ".insert(obj);\n}\n";

        file << "template<> void Repository"+options.sufixRepository+"::update(const "<<table2className(table)<<"& obj)\n{\n\t";
        file <<boost::algorithm::to_lower_copy(table2className(table)) << ".update(obj);\n}\n";

        file << "template<> void Repository"+options.sufixRepository+"::update(const "<<table2className(table)<<"& oldObj, const "<<table2className(table)<<"& newObj)\n{\n\t";
        file <<boost::algorithm::to_lower_copy(table2className(table)) << ".update(oldObj, newObj);\n}\n";

        file << "template<> void Repository"+options.sufixRepository+"::remove(const "<<table2className(table)<<"& obj)\n{\n\t";
        file <<boost::algorithm::to_lower_copy(table2className(table)) << ".remove(obj);\n}\n";
    }
}



