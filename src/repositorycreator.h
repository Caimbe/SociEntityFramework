#ifndef REPOSITORYCREATOR_H
#define REPOSITORYCREATOR_H

#include "soci/soci.h"
#include <set>
#include "util.hpp"
#include <queue>

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
    void insertColumnsToSelectOfRelation(ofstream& file, string& table, set<string>& relationsInserted, bool virgula=false);
    void insertLeftJoinsOfRelation(ofstream& file, string table, set<string>& relationsInserted);
    void getLeftJoinsOfRelation(string table, set<string>& relationsInserted, vector<string>& vecIners);
    string getIdFuncRelation(Column& column);
};

#endif // REPOSITORYCREATOR_H
