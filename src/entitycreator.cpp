#include "entitycreator.h"
#include "util.hpp"
#include <soci/soci.h>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
using namespace soci;

EntityCreator::EntityCreator(soci::session& db, string table, string tableSchema) : dataBase(db)
{
    this->table = table;
    this->tableSchema = tableSchema;
    className = table;
    className[0] = toupper(className[0]);
    for(size_t find = className.find("_"); find != string::npos; find=className.find("_"))
    {
        className.erase(find, 1);
        className[find] = toupper(className[find]);
    }
    vecColumns = getColumns(getColumnsDB(table, dataBase, tableSchema));
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
    insertIncludesRelations(file);
    file << "\nusing namespace std;\n";
    file << "\nclass " << className << endl << '{' << endl;
    insertDeclarationsAttribs(file);
    file << endl;
    insertDeclarationsGetsAndSets(file);
    file << "};\n" << endl;
    insertTypedefPointers(file);
    file << geraDefinersClose(className);
}

void EntityCreator::createCpp()
{
    cout << "creating cpp for " << className << endl;
    string fileName = className;
    boost::algorithm::to_lower(fileName);
    ofstream file(DIR_ENTITY+fileName+".cpp");

    file << "#include \"" << fileName << ".h\"\n\n";
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

void EntityCreator::insertDeclarationsGetsAndSets(ofstream &file)
{
    for(Column column: vecColumns)
    {
        column.var[0] = toupper(column.var[0]);
        file << '\t' << column.type << " " << "get" << column.var << "();" << endl;
        file << '\t' << "void " << "set" << column.var << "("<<column.type<<" value);" << endl;
    }
}

void EntityCreator::insertImplementationGetsAndSets(ofstream &file)
{
    for(Column column: vecColumns)
    {
        string varLow = column.var;
        column.var[0] = toupper(column.var[0]);
        file << column.type << ' ' << className << "::" << "get" << column.var << "()" << endl;
        file << "{\n\treturn " << varLow << ";\n}\n";
        file << "void " << className << "::" << "set" << column.var << "("<<column.type<<" value)" << endl;
        file << "{\n\t" << varLow << " = value;\n}\n";
    }
}

void EntityCreator::insertTypedefPointers(ofstream &file)
{
    file << "typedef shared_ptr<"<<className<<"> "<< className<<"Ptr;";
}

void EntityCreator::insertIncludesRelations(ofstream &file)
{
    for(Column column: vecColumns)
    {
        if(column.relation.size())
            file << "#include \"" << column.relation << ".h\"\n";
    }
}

string EntityCreator::geraDefinersOpen(string className)
{
    boost::algorithm::to_upper(className);
    return "#ifndef "+className+"_H\n#define "+className+"_H\n\n";
}

string EntityCreator::geraDefinersClose(string className)
{
    boost::algorithm::to_upper(className);
    return "\n\n#endif // "+className+"_H\n";
}
