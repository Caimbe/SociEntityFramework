#include "entitycreator.h"
#include "util.hpp"
#include <soci/soci.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
using namespace soci;

#define DIR_ENTITY "entity/"

EntityCreator::EntityCreator(soci::session& db, string table, string tableSchema, vector<Entity>& vecEntity) : dataBase(db)
{
    this->table = table;
    this->tableSchema = tableSchema;
    this->vecEntity = vecEntity;
    className = table2className(table);

    vecColumns = getColumns(getColumnsDB(table, dataBase, tableSchema), dataBase, table);

    boost::filesystem::create_directories(DIR_ENTITY);
    createHeader();
    createCpp();
}

void EntityCreator::createHeader()
{
    cout << "creating header for " << className << endl;
    string fileName = className;
    boost::algorithm::to_lower(fileName);
    fileName+=".h";
    ofstream file(DIR_ENTITY+fileName);

    file << geraDefinersOpen(className);
    file << INCLUDES_DEFAULTS;
    insertIncludesRelations(file, vecColumns);
    file << "\nusing namespace std;\n";
    insertTypedefPointers(file);
    file << "\nclass " << className << endl << '{' << endl;
    insertDeclarationsAttribs(file);
    file << "public:"<<endl;
    insertDeclarationConstructors(file);
    insertDeclarationsGetsAndSets(file);
    file << "};\n" << endl;
    file << geraDefinersClose(className);
}

void EntityCreator::createCpp()
{
    cout << "creating cpp for " << className << endl;
    string fileName = className;
    boost::algorithm::to_lower(fileName);
    ofstream file(DIR_ENTITY+fileName+".cpp");

    file << "#include \"" << fileName << ".h\"\n\n";
    insertImplemetationConstructors(file);
    insertImplementationGetsAndSets(file);
    file << endl;
}

void EntityCreator::insertDeclarationsAttribs(ofstream &file)
{

    for(Column column: vecColumns)
    {
        file << '\t' << column.type << " " << column.var << ';';
        if(column.key.size())
            file << " //key: "<<column.key;
        file << endl;
    }
}

void EntityCreator::insertDeclarationConstructors(ofstream &file)
{
    file << '\t'<<className<<"();\n";
    bool isKeyPri = false;
    for(Column column: vecColumns)
        if(column.key=="PRI")
            isKeyPri=true;
    if(isKeyPri){
        file << '\t'<<className<<"(";
        bool virgula = false;
        for(Column column: vecColumns)
        {
            if(column.key=="PRI"){
                file << (virgula?", ":"") << column.type << " " << column.var;
                virgula=true;
            }
        }
        file << ");\n";
    }
    file << "\tvoid init();\n";
}

void EntityCreator::insertDeclarationsGetsAndSets(ofstream &file)
{
    for(Column column: vecColumns)
    {
        column.var[0] = toupper(column.var[0]);
        file << '\t' << column.type << " " << "get" << table2className( column.var ) << "() const;" << endl;
        file << '\t' << "void " << "set" << table2className(column.var) << "("<<column.type<<" value);" << endl;
        /*if(column.key.size()){
            for(auto itE=vecEntity.begin(); itE!=vecEntity.end(); itE++){
                if(itE->name==column.relation)
                    for(auto itV=itE->vecColumn.begin();itV!=itE->vecColumn.end();itV++)
                        if(itV->key == "PRI")
                            file << '\t' << "void " << "set" << column.var << "("<<itV->type<<" value);" << endl;
            }


        }*/
    }
}

void EntityCreator::insertImplemetationConstructors(ofstream &file)
{
    file << className <<"::"<<className<<"(){\n"
            "\tinit();\n"
            "}\n";
    bool isKeyPri = false;
    for(Column column: vecColumns)
        if(column.key=="PRI")
            isKeyPri=true;
    if(isKeyPri){
        file << className <<"::"<<className<<"(";
        bool virgula = false;
        for(Column column: vecColumns)
        {
            if(column.key=="PRI"){
                file << (virgula?", ":"") << column.type << " " << column.var;
                virgula=true;
            }
        }
        file << ")\n{\n";
        file << "\tinit();\n";
        for(Column column: vecColumns)
        {
            if(column.key=="PRI")
                file << "\tthis->"<<column.var <<" = "<< column.var<<";\n";
        }
        file << "}\n\n";
    }
    file << "void "<<className <<"::"<<"init()\n{\n";
    for(Column column: vecColumns)
        if(column.type == "tm")
            file <<'\t'<< column.var<<" = {0};\n";
    file << "}\n";
}

void EntityCreator::insertImplementationGetsAndSets(ofstream &file)
{
    for(Column column: vecColumns)
    {
        string varLow = column.var;
        column.var[0] = toupper(column.var[0]);
        file << column.type << ' ' << className << "::" << "get" << table2className(column.var) << "() const" << endl;
        file << "{\n\treturn " << varLow << ";\n}\n";
        file << "void " << className << "::" << "set" << table2className(column.var) << "("<<column.type<<" value)" << endl;
        file << "{\n\t" << varLow << " = value;\n}\n";
    }
}

void EntityCreator::insertTypedefPointers(ofstream &file)
{
    file << "\nclass "<<className<<";\ntypedef shared_ptr<"<<className<<"> "<< className<<"Ptr;\n";
    file << "typedef vector<"<<className<<"Ptr> "<<className<<"List;\n";
}

