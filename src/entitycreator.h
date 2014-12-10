#ifndef ENTITYCREATOR_H
#define ENTITYCREATOR_H

#include <fstream>
#include <iostream>
#include <string>
#include <soci/soci.h>
#include "util.hpp"
using namespace std;

class EntityCreator
{
    string table;
    string tableSchema;
    string className;
    soci::session& dataBase;
    vector<Column> vecColumns;
public:
    EntityCreator(soci::session& dataBase, string table, string tableSchema);
    void createHeader();
    void createCpp();
    void insertDeclarationsAttribs(ofstream& file);
    void insertDeclarationConstructors(ofstream& file);
    void insertDeclarationsGetsAndSets(ofstream& file);
    void insertImplemetationConstructors(ofstream& file);
    void insertImplementationGetsAndSets(ofstream& file);
    void insertTypedefPointers(ofstream& file);
};

#endif // ENTITYCREATOR_H
