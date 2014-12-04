#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "processdatabase.h"

using namespace std;

enum ExitReturn{
    SUCCESS=0,
    SUCCESS_IN_COMMAND_LINE=1,
    ERROR_IN_COMMAND_LINE=2,
    ERROR_IN_FILE_LOCK=3,
    ERROR_UNHANDLED_EXCEPTION=4,
};

ExitReturn process_command_line(int argc, char** argv, string& urlDataBase);

int main(int argc, char** argv)
{
    try{
        string urlDataBase;
        ExitReturn result = process_command_line(argc, argv, urlDataBase);
        if (result != ExitReturn::SUCCESS_IN_COMMAND_LINE)
            return ExitReturn::ERROR_IN_COMMAND_LINE;

        ProcessDataBase processaDataBase(urlDataBase);
        processaDataBase.start();
    }catch(exception& ex){
        std::cerr << "Unhandled Exception reached the top of main: "
                  << ex.what() << ", application will now exit" << std::endl;
        return ExitReturn::ERROR_UNHANDLED_EXCEPTION;
    }

    return ExitReturn::SUCCESS;
}

ExitReturn process_command_line(int argc, char** argv, string& urlDataBase)
{
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
        ("help,h", "Print help messages")
        ("url-database,u", po::value<string>(), "string for conection in database ex.: \"mysql://host=localhost db=mydatabase user=root password=123456\"");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc),vm); // can throw
    po::notify(vm); // throws on error, so do after help in case there are any problems
    if (vm.count("help") || vm.size()==0)
    {
        std::cout << "Basic Command Line Parameter App" << std::endl
                  << desc << std::endl;
        return ExitReturn::ERROR_IN_COMMAND_LINE;
    }
    if (vm.count("url-database"))
    {
        urlDataBase = vm["url-database"].as<string>();
    }
    return ExitReturn::SUCCESS_IN_COMMAND_LINE;
}
