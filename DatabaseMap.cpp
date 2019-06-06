#include "DatabaseMap.h"


void DatabaseMap::CreateDatabase(std::string db_name) {
	dbs[db_name] = Database(db_name);           //创立数据库
}

void DatabaseMap::UseDatabase(std::string db_name) {
	current_name = db_name;
	current_db = &dbs[db_name];                 //切换到当前的数据库；
}

void DatabaseMap::DropDatabase(std::string db_name) {
	dbs.erase(db_name);                        //删除数据库
}

void DatabaseMap::ShowDatabases() {
	for (std::map<std::string, Database>::iterator iter = dbs.begin(); iter != dbs.end(); iter++)
	{
		if (iter == dbs.begin())
			std::cout << "Database" << "\n" ;
		std::cout << iter->first << "\n" ;//数据库名称
	}
}

void DatabaseMap::CreateTable(std::string table_name, std::vector<Attribute> attr, std::string _key)
{
	current_db->CreateTable(table_name, attr, _key);
}

void DatabaseMap::DropTable(std::string table_name)
{
	current_db->DropTable(table_name);
}

void DatabaseMap::ShowTables()
{
	current_db->ShowTables();
}

void DatabaseMap::ShowColumns(std::string table_name)
{
	current_db->ShowColumns(table_name);
}

void DatabaseMap::InsertInto(std::string table_name, std::vector<std::string> attr_name, std::vector<std::string> value)
{
	current_db->INSERTinto(table_name, attr_name, value);
}

void DatabaseMap::DeleteRow(const std::string table_name, const std::string key)
{
	current_db->deleterow(table_name, key);
}

void DatabaseMap::Set(const std::string table_name, const std::string attr_name, const std::string value, const std::string key)
{
	current_db->set(table_name, attr_name, value, key);
}

void DatabaseMap::OutputAttr(const std::string table_name)
{
	current_db->output(table_name);
}

void DatabaseMap::OutputRow(const std::string table_name, const std::string key)
{
	current_db->output(table_name, key);
}

std::string DatabaseMap::GetValue(const std::string table_name, const std::string attr, const std::string key)
{
	Data a;
	a.value = key;
	a.type = getkeytype(table_name);
	return current_db->table_list[table_name].row_map[a].data[attr];
}

std::string DatabaseMap::GetType(const std::string table_name, const std::string attr) 
{
	for (auto iter = current_db->table_list[table_name].attr_list.begin(); iter != current_db->table_list[table_name].attr_list.end(); iter++)
	{
		if (iter->name == attr)
			return iter->type;
	}
	return "";
}

std::set<Data> DatabaseMap::GetAllKeys(const std::string table_name)
{
	return current_db->table_list[table_name].getallkeys();
}

std::set<Data> DatabaseMap::KeyWhereCluase(const std::string table_name, const Clause c)
{
	return current_db->table_list[table_name].keywherecluase(c);
}

std::string DatabaseMap::getkeytype(const std::string table_name) {
	return current_db->getkeytype(table_name);
}