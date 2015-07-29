#ifndef PROCESSDATABASE_H
#define PROCESSDATABASE_H

#include <soci/soci.h>
#include <fstream>
#include <string>
using namespace std;
using namespace soci;

struct Options
{
    string urlDataBase;
    string sufixRepository;
};

class ProcessDataBase
{
    string tableSchema;
    soci::session dataBase;
    Options options;
public:
    ProcessDataBase(const Options& opt);
    void start();
    void createInterfaceHeader(vector<string> vecTables);
    void createInterfaceCpp(vector<string> vecTables);
};

#endif // PROCESSDATABASE_H
