#include "Database.h"

void Database::CreateTable(std::string table_name, std::vector<Attribute> attr, std::string _key) {
	table_list[table_name] = Table(attr, _key);
}
void Database::DropTable(std::string tablename) {
	table_list.erase(tablename);
}

void Database::ShowTables() {
	std::cout << "Tables_in_" << db_name << "\n";
	for (auto iter = table_list.begin(); iter != table_list.end(); iter++)
		std::cout << iter->first << "\n";
}

void Database::ShowColumns(std::string tablename) {
	std::cout << "Field" << "\t" << "Type" << "\t" << "Null" << "\t" << "Key" << "\t" << "Default" << "\t" << "Extra"<<"\n";

	for (auto it = table_list[tablename].attr_list.begin() ; it != table_list[tablename].attr_list.end(); it++)
	{
		std::cout << it->name << "\t";
		if (it->type == "int")
			std::cout << "int(11)" << "\t";
		else if (it->type == "char")
			std::cout << "char(1)" << "\t";
		else if (it->type == "double")
			std::cout << "double" << "\t";
		if (it->Null == true)
			std::cout << "YES" << "\t";
		else
			std::cout << "NO" << "\t";

		if (it->Key == true)
			std::cout << "PRI" << "\t";
		else
			std::cout << "\t";

		std::cout << "NULL" << "\t";
		std::cout << "\n";
	}
}

void Database::INSERTinto(std::string table_name, std::vector<std::string> attr_name, std::vector<std::string> value)
{
	table_list[table_name].insert(attr_name, value);
}

void Database::deleterow(const std::string table_name, const std::string key)
{
	Data a;
	a.type = table_list[table_name].getkeytype();
	a.value = key;
	table_list[table_name].row_map.erase(a);
}

void Database::set(const std::string table_name, const std::string attr_name, const std::string value, const std::string key)
{
	Data a;
	a.type = table_list[table_name].getkeytype();
	a.value = key;

	table_list[table_name].row_map[a].data[attr_name] = value;
	if (attr_name == table_list[table_name].getkey())
	{
		Row b = table_list[table_name].row_map[a] ;
		table_list[table_name].row_map.erase(a);
		a.value = value;
		table_list[table_name].row_map.insert(std::pair<Data,Row>(a, b));

	}
}

void Database::output(const std::string table_name)//调用指定表的OutputAttr
{
	table_list[table_name].OutputAttr();
}

void Database::output(const std::string table_name, const std::string key)
{
	table_list[table_name].find(key);
}

std::string Database:: getkeytype(const std::string table_name) {
	return table_list[table_name].getkeytype();
}