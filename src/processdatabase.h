#ifndef PROCESSDATABASE_H
#define PROCESSDATABASE_H

#include <soci/soci.h>
#include <fstream>
#include <string>
using namespace std;
using namespace soci;

class ProcessDataBase
{
    string tableSchema;
    soci::session dataBase;
public:
    ProcessDataBase(const string& urlDataBase);
    void start();
};

#endif // PROCESSDATABASE_H
