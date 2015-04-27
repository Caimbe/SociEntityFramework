#include "repositorycreator.h"
#include "boost/filesystem.hpp"


RepositoryCreator::RepositoryCreator(string &table, string &tableSchema, vector<Column> vecColumns, soci::session& db) : dataBase(db)
{
    this->table = table;
    this->tableSchema = tableSchema;
    this->vecColumns = vecColumns;
    className = table2className(table) + "Repository";
    entity = table2className(table);
    entityLower = boost::algorithm::to_lower_copy(entity);

    boost::filesystem::create_directories(DIR_REPOSITORY);
    createHeader();
    createCpp();
}

void RepositoryCreator::createHeader()
{
    string fileName = className;
    boost::algorithm::to_lower(fileName);
    fileName+=".h";
    ofstream file(DIR_REPOSITORY+fileName);
    cout << "creating header: " << fileName << endl;

    file << geraDefinersOpen(className);

    file << INCLUDES_DEFAULTS;
    file << "#include <soci/soci.h>\n";
    insertIncludesRelations(file, vecColumns, "repository");
    file << "#include \"entity/" << entityLower << ".h\"\n";
    file << "using namespace soci;\n\n";

    file << "\nclass " << className << "\n{\n";
    file << "\tsoci::session& dataBase;\n";
    file << "public:\n";
    insertDeclarationConstructor(file);
    file << "\tint insert(const "<<entity <<"& "<< entityLower << ");\n";
#ifdef SELECT_UNICO
    file << '\t' << entity << "Ptr select(int id);\n";
#endif
    file << '\t' << entity << "List select(const string& where=\"\");\n";
    file << "\tvoid update(const "<< entity <<"& "<< entityLower << ");\n";
    file << "\tvoid remove(const "<< entity <<"& "<< entityLower << ");\n";
    file << "};\n\n";
    insertObjectRelationalMapping(file);
    file << geraDefinersClose(className);
}

void RepositoryCreator::insertObjectRelationalMapping(ofstream &file)
{
    file << "namespace soci\n{\ntemplate<>\nstruct type_conversion<"<<entity<<">\n{\ntypedef values base_type;\n";
    file << "\ttemplate<class T>";
    file << "\tstatic void from_base(const T& v, const indicator& ind, "<<entity<<" & p)\n\t{\n";
    for(Column coluna: vecColumns)
    {
        string asColumn = entity+'_'+coluna.var;
        coluna.var[0] = toupper(coluna.var[0]);
        file << "\t\tif (v.get_indicator(\""<<asColumn<<"\") != i_null){\n";
        if(coluna.relation.size()){
            coluna.relation[0] = toupper(coluna.relation[0]);
            file << "\t\t\tp.set"<<coluna.var<<"( "<<coluna.type<<"( new "<<coluna.relation<<"(v.get<"<<coluna.type<<">(\""<<asColumn<<"\")) ) );\n";
            file << "\t\t\ttype_conversion<"<<coluna.relation<<">::from_base(v, i_ok, *p.get"<<coluna.var<<"() );\n\t\t}\n";
        }else
            file << "\t\t\tp.set"<<coluna.var<<"( v.template get<"<<coluna.type<<">(\""<<asColumn<<"\" ) );\n\t\t}\n";
    }
    file << "\t}\n";
    file << "\tstatic void to_base(const "<<entity<<" & p, values & v, indicator & ind)\n\t{\n";
    for(Column coluna: vecColumns)
    {
        string asColumn = entity+'_'+coluna.var;
        coluna.var[0] = toupper(coluna.var[0]);
        if(coluna.relation.size()){
            file << "\t\tif( p.get"<<coluna.var<<"() )\n";
            file << "\t\t\tv.set( \""<<asColumn<<"\", p.get"<<coluna.var<<"()->getId() );\n";
            file << "\t\telse\n";
            file << "\t\t\tv.set( \""<<asColumn<<"\", NULL, i_null);\n";
        }else{
            string cast;
            if(coluna.type == "float")
                cast = "(double)";
            else if(coluna.type == "bool")
                cast = "(int)";
            file << "\t\tv.set( \""<<asColumn<<"\", "<<cast<<"p.get"<<coluna.var<<"() );\n";
        }
    }
    file << "\t\tind = i_ok;\n\t}\n};\n}";
}

void RepositoryCreator::createCpp()
{
    string fileName = className;
    boost::algorithm::to_lower(fileName);
    ofstream file(DIR_REPOSITORY+fileName+".cpp");
    cout << "creating cpp: " << className << endl;

    file << "#include \"" << boost::algorithm::to_lower_copy( className ) << ".h\"\n\n";

    insertImplementationConstructor(file);
    insertImplementationSelect(file);
    insertImplementationInsert(file);
    insertImplementationRemove(file);
    insertImplementationUpdate(file);
}

void RepositoryCreator::insertImplementationSelect(ofstream &file)
{
#ifdef SELECT_UNICO
    file << entity << "Ptr " << className <<"::select(int id)\n{\n";
    file << "soci::row row;\n";
    file << '\t' << entity<<"Ptr "<<entityLower<<"(new "<<entity<<");\n";
    file << "\tdataBase << \"SELECT ";
    insertColumnsToSelectOfRelation(file, table);
    file << " \\\n\tFROM "<<table;
    insertLeftJoinsOfRelation(file, table);
    file <<" \\\n\tWHERE "<<table<<".id=:id\", into(row), use(id);\n";
    file << "\tif(!dataBase.got_data())\n\t\t"<<entityLower<<".reset();\n";
    file << "\telse\n\t\ttype_conversion<"<<entity<<">::from_base(row, i_ok, *"<<entityLower<<");\n";
    file << "\treturn "<<entityLower<<";\n";
    file << "}\n";
#endif

    file << entity << "List " << className <<"::select(const string& where)\n{\n";
    file << "\tsoci::rowset<row> rs = ";
    file << "\tdataBase.prepare << \"SELECT ";
    insertColumnsToSelectOfRelation(file, table);
    file << " \"\n\t\" FROM "<<table;
    insertLeftJoinsOfRelation(file, table);
    file << "\" \n\t<< (where.size()?\" WHERE \"+where:\"\");\n";
    file << '\t' << entity<<"List "<<entityLower<<"List;\n";
    file << "\tfor(row& r: rs)\n\t{\n";
    file << "\t\t" << entity<<"Ptr "<<entityLower<<"(new "<<entity<<");\n";
    file << "\t\ttype_conversion<"<<entity<<">::from_base(r, i_ok, *"<<entityLower<<");\n";
    file << "\t\t"<<entityLower<<"List.push_back("<<entityLower<<");\n\t}\n";
    file << "\treturn "<<entityLower<<"List;\n";
    file << "}\n\n";
}

void RepositoryCreator::insertImplementationUpdate(ofstream &file)
{
    file << "void "<<className<<"::update(const "<< entity <<"& "<< entityLower << ")\n{\n";
    file << "\tdataBase << \"update "<<table<<" set ";
    bool first=true;
    for(int i=0; i<vecColumns.size(); i++){
        if(vecColumns[i].var == "id")
            continue;

        string columName = vecColumns[i].var;
        if(vecColumns[i].relation.size())
            columName+="_id";
        string asColumn = entity+'_'+vecColumns[i].var;
        if(first)
            file << columName << "=:"<<asColumn;
        else
            file << ", "<< columName << "=:"<<asColumn;
        first=false;
    }
    file << " WHERE ";
    bool virgula = false;
    for(Column column: vecColumns)
    {
        if(column.key.size()){
            file << (virgula?" AND \" << ":"") << column.var << "=\"<<" << entityLower << ".get" << table2className(column.var) << "()";
            virgula=true;
        }
    }
    file << ", use("<<entityLower<<");\n";
    file << "}\n\n";
}

void RepositoryCreator::insertImplementationInsert(ofstream &file)
{
    file << "int "<<className<<"::insert(const "<< entity <<"& "<< entityLower << ")\n{\n";
    file << "\tdataBase << \"insert into "<<table<<'(';
    bool first=true;
    for(int i=0; i<vecColumns.size(); i++){
        string colunaNome = vecColumns[i].var ;
        if(vecColumns[i].relation.size())
            colunaNome+="_id";

        if(vecColumns[i].var == "id")
            continue;
        if(!first)
            file << ", ";
        file << colunaNome;

        first=false;
    }
    file << ")\\\nvalues(";
    first=true;
    for(int i=0; i<vecColumns.size(); i++)
    {
        if(vecColumns[i].var == "id")
            continue;
        string asColumn = entity+'_'+vecColumns[i].var;
        if(first)
            file << ':'<<asColumn;
        else
            file << ", :"<<asColumn;
        first=false;
    }
    file << ")\", use("<<entityLower<<");\n";
    file << "\tint id=0;\n\tdataBase << \"SELECT LAST_INSERT_ID()\", soci::into(id);\n\treturn id;\n";
    file << "}\n\n";
}

void RepositoryCreator::insertImplementationRemove(ofstream &file)
{
    file << "void "<<className<<"::remove(const "<< entity <<"& "<< entityLower << ")\n{\n";
    file << "\tdataBase << \"DELETE from "<<table<<" WHERE ";
    bool virgula = false;
    for(Column column: vecColumns)
    {
        if(column.key.size()){
            file << (virgula?" AND \" << ":"") << column.var << "=\"<<" << entityLower << ".get" << table2className(column.var) << "()";
            virgula=true;
        }
    }
    file << ";\n}\n\n";
}

void RepositoryCreator::insertDeclarationConstructor(ofstream &file)
{
    file << '\t' << className<<"(soci::session& dataBase);\n";
}

void RepositoryCreator::insertImplementationConstructor(ofstream &file)
{
    file << className << "::" << className<<"(soci::session& db) : dataBase(db)\n{\n}\n\n";
}

void RepositoryCreator::insertColumnsToSelectOfRelation(ofstream &file, string& table, bool virgula)
{
    string entity = table2className(table);
    vector<Column> colunas = getColumns(getColumnsDB(table, dataBase, tableSchema));
    for(Column& coluna: colunas)
    {
        string colunaNome = table+'.'+coluna.var ;
        if(coluna.relation.size()){
            colunaNome+="_id";
            if(table != coluna.relation)
                insertColumnsToSelectOfRelation(file, coluna.relation, true);
        }
        file << (virgula?", ":" ") << colunaNome << " as " << entity<<'_'<<coluna.var;

        virgula=true;
    }
}

void RepositoryCreator::insertLeftJoinsOfRelation(ofstream &file, string table)
{
    vector<Column> vecColumns = getColumns(getColumnsDB(table, dataBase, tableSchema));
    for(int i=0; i<vecColumns.size(); i++){
        if(vecColumns[i].relation.size()){
            if(table != vecColumns[i].relation){
                file << " \\\n\tLEFT OUTER JOIN "<<vecColumns[i].relation<<" ON("<<table<<'.'<<vecColumns[i].relation<<"_id="<<vecColumns[i].relation<<".id)";
                insertLeftJoinsOfRelation(file, vecColumns[i].relation);
            }
        }
    }
}

