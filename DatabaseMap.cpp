#include "DatabaseMap.h"


bool DatabaseMap::existTable(std::string tablename)
{
	if (!current_db)
		return false;
	return current_db->table_list.count(tablename);
}

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

void DatabaseMap::ALUprocess(std::vector<std::string> expressions)
{
	std::vector<std::vector<std::string> > results;
	for (auto i : expressions) {
		ALU expression(i);
		results.push_back(expression.process());
		std::cout << i << "\t";
	}
	std::cout << std::endl;

	for (auto j : results) {
		std::cout << j[0] << "\t";
	}
	std::cout << std::endl;
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


//初步实现数据的加载和读取，未实现：对于数据库的增删操作，应先删除数据库所包含的表格；不同数据库创建表格的时候可能存在与其他数据库的表格重名，导致存档时生成的文件重名，考虑给表名加前缀这个解决方案

void DatabaseMap::Saving() {
	std::ofstream fout0;
	fout0.open("DatabaseNames");//依次存入一个整数和若干字符串，用空格隔开，整数表示数据库
	fout0 << dbs.size() << ' ';     //数量，字符串均为数据库名称

	for (auto db : dbs)
	{
		fout0 << db.first << ' ';

		std::ofstream fout1;
		fout1.open(db.first);//以数据库名称为文件名，便于读取，此文件存表的数量和各个表名
		fout1 << db.second.table_list.size() << ' ';

		for (auto tab : db.second.table_list)
		{
			fout1 << tab.first << ' ';

			std::ofstream fout2;
			fout2.open(tab.first);//以表的名称作为文件名，存属性及表格数据

			fout2 << tab.second.attr_list.size() << ' '
				<< tab.second.row_map.size() << std::endl;

			for (auto attr : tab.second.attr_list)
			{
				fout2 << attr.name << ' ' << attr.type << ' ';
				if (attr.Null) fout2 << "1 ";
				else fout2 << "0 ";
				if (attr.Key) fout2 << "1\n";
				else fout2 << "0\n";
			}

			for (auto row : tab.second.row_map)
				for (auto val : row.second.data)
				{
					fout2 << val.second << ' ';
				}

			fout2.close();
		}

		fout1.close();
	}

	fout0.close();
}

void DatabaseMap::Loading() {
	std::cout << "————Loading————" << std::endl;
	std::ifstream fin0;
	fin0.open("DatabaseNames");

	int numOfdbs;
	fin0 >> numOfdbs;//读取数据库的数量
	for (int i = 0; i < numOfdbs; i++) {
		std::string databaseName = "";
		fin0 >> databaseName;
		CreateDatabase(databaseName);//创建数据库
		current_db = &dbs[databaseName];//制定该数据库为当前使用的数据库

		std::ifstream fin1;
		fin1.open(databaseName);

		int numOftabs;
		fin1 >> numOftabs;//读取表的数量
		for (int i = 0; i < numOftabs; i++) {
			std::string tableName = "";
			fin1 >> tableName;

			std::ifstream fin2;
			fin2.open(tableName);
			int numOfcols, numOfrows;
			fin2 >> numOfcols >> numOfrows;

			std::vector<Attribute> attrs;
			std::string KEY = "";
			for (int i = 0; i < numOfcols; i++) {
				Attribute attr;
				std::string attrName = "", attrType = "";
				fin2 >> attrName >> attrType;

				int null, key;
				fin2 >> null >> key;

				if (key) KEY = attrName;

				attr.name = attrName;
				attr.type = attrType;
				attr.Null = null;
				attr.Key = key;

				attrs.push_back(attr);
			}

			CreateTable(tableName, attrs, KEY);

			//由于Table类使用Map来存储每一行数据，导致各列的数据经过列名的排序，可能与vector的顺序不同，故这里先用Set来排序，使列名的顺序和存档时的各个属性数据的顺序一致
			std::set<std::string> attrNames;
			for (auto x : attrs)
				attrNames.insert(x.name);

			std::vector<std::string> colNames;
			for (auto x : attrNames)
				colNames.push_back(x);

			for (int i = 0; i < numOfrows; i++) {
				std::vector<std::string> values;
				for (int j = 0; j < numOfcols; j++) {
					std::string val = "";
					fin2 >> val;
					values.push_back(val);
				}
				InsertInto(tableName, colNames, values);
			}

			fin2.close();
		}

		fin1.close();
	}

	fin0.close();
	std::cout << "————Complete————" << std::endl;
}