#include "entitycreator.h"
#include "util.hpp"
#include <soci/soci.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
using namespace soci;

#define DIR_ENTITY "entity/"

EntityCreator::EntityCreator(soci::session& db, string table, string tableSchema) : dataBase(db)
{
    this->table = table;
    this->tableSchema = tableSchema;
    className = table2className(table);

    vecColumns = getColumns(getColumnsDB(table, dataBase, tableSchema));

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
        file << '\t' << column.type << " " << column.var << ';' << endl;
    }
}

void EntityCreator::insertDeclarationConstructors(ofstream &file)
{
    file << '\t'<<className<<"();\n";
    file << '\t'<<className<<"(";
    bool virgula = false;
    for(Column column: vecColumns)
    {
        if(column.key.size()){
            file << (virgula?", ":"") << column.type << " " << column.var;
            virgula=true;
        }
    }
    file << ");\n";
}

void EntityCreator::insertDeclarationsGetsAndSets(ofstream &file)
{
    for(Column column: vecColumns)
    {
        column.var[0] = toupper(column.var[0]);
        file << '\t' << column.type << " " << "get" << column.var << "() const;" << endl;
        file << '\t' << "void " << "set" << column.var << "("<<column.type<<" value);" << endl;
    }
}

void EntityCreator::insertImplemetationConstructors(ofstream &file)
{
    file << className <<"::"<<className<<"(){}\n";
    file << className <<"::"<<className<<"(";
    bool virgula = false;
    for(Column column: vecColumns)
    {
        if(column.key.size()){
            file << (virgula?", ":"") << column.type << " " << column.var;
            virgula=true;
        }
    }
    file << ")\n{\n";
    for(Column column: vecColumns)
    {
        if(column.key.size())
            file << "\tthis->"<<column.var <<" = "<< column.var<<";\n";
    }
    file << "}\n\n";
}

void EntityCreator::insertImplementationGetsAndSets(ofstream &file)
{
    for(Column column: vecColumns)
    {
        string varLow = column.var;
        column.var[0] = toupper(column.var[0]);
        file << column.type << ' ' << className << "::" << "get" << column.var << "() const" << endl;
        file << "{\n\treturn " << varLow << ";\n}\n";
        file << "void " << className << "::" << "set" << column.var << "("<<column.type<<" value)" << endl;
        file << "{\n\t" << varLow << " = value;\n}\n";
    }
}

void EntityCreator::insertTypedefPointers(ofstream &file)
{
    file << "\nclass "<<className<<";\ntypedef shared_ptr<"<<className<<"> "<< className<<"Ptr;\n";
    file << "typedef vector<"<<className<<"Ptr> "<<className<<"List;\n";
}

