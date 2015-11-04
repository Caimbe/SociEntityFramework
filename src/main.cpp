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

ExitReturn process_command_line(int argc, char** argv, Options& opt);

int main(int argc, char** argv)
{
    try{
        Options opt;
        ExitReturn result = process_command_line(argc, argv, opt);
        if (result == ExitReturn::SUCCESS_IN_COMMAND_LINE){
            ProcessDataBase processaDataBase(opt);
            processaDataBase.start();
        }
    }catch(exception& ex){
        std::cerr << "Unhandled Exception reached the top of main: "
                  << ex.what() << ", application will now exit" << std::endl;
        return ExitReturn::ERROR_UNHANDLED_EXCEPTION;
    }

    return ExitReturn::SUCCESS;
}

ExitReturn process_command_line(int argc, char** argv, Options& opt)
{
    namespace po = boost::program_options;
    po::options_description desc("Options");
    desc.add_options()
            ("help,h", "Print help messages")
            ("connectdb,c", po::value<string>(), "string REQUERID for conection in database ex.: \"mysql://host=localhost db=mydatabase user=root password=123456\"")
            ("sufix-repository,s", po::value<string>(), "sufix of termination Repository name, ex.: -s Postfix - The repository name is: RepositoryPostfix.");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc),vm); // can throw
    po::notify(vm); // throws on error, so do after help in case there are any problems
    if (vm.count("help") || vm.size()==0)
    {
        std::cout << "Basic Command Line Parameter App" << std::endl
                  << desc << std::endl;
        return ExitReturn::ERROR_IN_COMMAND_LINE;
    }
    if (vm.count("connectdb"))
    {
        opt.urlDataBase = vm["connectdb"].as<string>();
    }
    else
    {
        return ExitReturn::ERROR_IN_COMMAND_LINE;
    }
    if (vm.count("sufix-repository"))
    {
        opt.sufixRepository = vm["sufix-repository"].as<string>();
    }
    return ExitReturn::SUCCESS_IN_COMMAND_LINE;
}
