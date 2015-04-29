#ifndef REPOSITORYCREATOR_H
#define REPOSITORYCREATOR_H

#include "soci/soci.h"
#include "util.hpp"

#define DIR_REPOSITORY "repository/"

class RepositoryCreator
{
    string className;
    string table;
    string tableSchema;
    string entity;
    string entityLower;
    vector<Column> vecColumns;    
    soci::session& dataBase;
public:
    RepositoryCreator(string& table, string& tableSchema, vector<Column> vecColumns, soci::session& dataBase);
    void createHeader();
    void insertObjectRelationalMapping(ofstream& file);
    void createCpp();
    void insertImplementationSelect(ofstream& file);
    void insertImplementationUpdate(ofstream& file);
    void insertImplementationUpdate2(ofstream& file);
    void insertImplementationInsert(ofstream& file);
    void insertImplementationRemove(ofstream& file);
    void insertDeclarationConstructor(ofstream& file);
    void insertImplementationConstructor(ofstream& file);
    void insertColumnsToSelectOfRelation(ofstream& file, string& table, bool virgula=false);
    void insertLeftJoinsOfRelation(ofstream& file, string table);
};

#endif // REPOSITORYCREATOR_H
