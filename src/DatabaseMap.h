#pragma once
#include <map>
#include <string>
#include <utility>

#include "Database.h"
#include "ALU.h"

class DatabaseMap {
private:
	DatabaseMap(const DatabaseMap&) = delete;
	void operator= (const DatabaseMap&) = delete;

	std::map<std::string, Database> dbs;
	std::string current_name;
	Database* current_db = nullptr;

public:
	DatabaseMap() = default;
	~DatabaseMap() = default;

	Database& getDatabase() { return *current_db; }
	bool existTable(std::string tablename);
	void CreateDatabase(std::string db_name);
	void DropDatabase(std::string db_name);
	void UseDatabase(std::string db_name);
	void ShowDatabases();
	void InsertInto(std::string table_name, std::vector<std::string> attr_name, std::vector<std::string> value);
	void CreateTable(std::string table_name, std::vector<Attribute> attr, std::string _key);
	void DropTable(std::string table_name);
	void ShowTables();
	void ShowColumns(std::string table_name);

	void ALUprocess(std::vector<std::string> expressions);
	void Saving();
	void Loading();

	//返回数据表table_name里所有行的主键对应数据的集合
	std::set<Data> GetAllKeys(const std::string table_name);
	//查找给定表中符合条件c的所有Row，返回这些Row主键的集合
	std::set<Data> KeyWhereCluase(const std::string table_name, const Clause c);
	//删除指定表中主键为key的行
	void DeleteRow(const std::string table_name, const std::string key); 
	//将表tablename中主键为key的行中的attr属性值更新为value
	void Set(const std::string table_name, const std::string attr_name, const std::string value, const std::string key);
	//返回指定表，key对应行，attr对应列的数据
	std::string GetValue(const std::string table_name, const std::string attr, const std::string key);
	//返回指定表attr列的类型
	std::string GetType(const std::string table_name, const std::string attr); 
	//调用指定表的OutputAttr
	void OutputAttr(const std::string table_name);
	//找到指定表中主键key对应Row，调用Row的output
	void OutputRow(const std::string table_name, const std::string key); 
	std::string getkeytype(const std::string table_name);

};
