#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <string>

using namespace std;

struct Column
{
    string type;
    string typeDb;
    string var;
    string relation;
    string key;
    string nameDb;
};


struct Entity
{
public:
    string name;
    vector<Column> vecColumn;
    Entity();
};

#endif // ENTITY_H
