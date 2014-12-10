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
    void createInterfaceHeader(vector<string> vecTables);
    void createInterfaceCpp(vector<string> vecTables);
};

#endif // PROCESSDATABASE_H
