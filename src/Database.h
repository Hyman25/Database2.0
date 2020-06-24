#pragma once
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include "Table.h"

class DatabaseMap;

class Database {
private:
	std::string db_name;     //数据库的名字，不知道需不需要
	std::map<std::string,Table> table_list;     //该数据库中表的集合,map<表名，表对象>
public:
	Database() = default;
	Database(std::string dbname) :db_name(dbname) {}

	Table& getTable(std::string tablename) { return table_list[tablename]; }

	void CreateTable(std::string table_name, std::vector<Attribute> attr, std::string _key);//更新1：建表
	void DropTable(std::string tablename);
	void ShowTables();
	void set(const std::string table_name, const std::string attr_name, const std::string value, const std::string key);
	void ShowColumns(std::string tablename);
	void INSERTinto(std::string table_name, std::vector<std::string> attr_name, std::vector<std::string> value);
	void deleterow(const std::string table_name, const std::string key);
	void output(const std::string table_name);
	void output(const std::string table_name, const std::string key);
	std::string getkeytype(const std::string table_name);

	friend class DatabaseMap;
};

