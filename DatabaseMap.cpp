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
    dbs.erase(db_name);//删除数据库
    
    int countOfTab = 0;
    std::fstream fin;

    fin.open(db_name);
    fin>>countOfTab;
    
    std::string tabName;
    for (int i=0; i<countOfTab; i++) {
        tabName = "";
        fin>>tabName;
    
        const char* tab = tabName.c_str();
        remove(tab);
    }
    fin.close();
    const char* DatabaseName = db_name.c_str();
    remove(DatabaseName);
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
    
    const char *tabName = table_name.c_str();
    remove(tabName);
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


void DatabaseMap::Saving(){
    //文件 DatabaseName 存储一个整数，表示数据库的数目；存储若干个字符串，表示数据库名称；整数和各个字符串之间用空格分开
    std::fstream fout_dbname;
    fout_dbname.open("DatabaseNames", std::fstream::out);
    //freopen("/Users/chenyuanyong/Desktop/Database2.0-master/DatabaseNames.out", "w", stdout);
    
    //如果数据库数目为0，记录这个数据后就可以直接退出存档了
    fout_dbname<<dbs.size()<<' ';
    if (dbs.size() == 0) {
        fout_dbname.close();
        return;
    }
    
    //每次向 DatabaseName 写入一个数据库的名称，都同时以该名称创建一个文件，该文件存储一个整数，表示该数据库包含表格的数目；存储若干个字符串，表示表格名称;整数和各个字符串之间用空格分开
    for (auto db : dbs)
    {
        fout_dbname<<db.first<<' ';
        std::fstream fout_db;
        fout_db.open(db.first, std::fstream::out);
        
        //如果表格数目为0，记录这个数据，则该数据库存档完毕，立刻进行下一个数据库的存档
        fout_db<<db.second.table_list.size()<<' ';
        if (db.second.table_list.size() == 0) {
            fout_db.close();
            continue;
        }
        //每次向该存储数据库的文件中写入一个表格的名称，都同时以该名称创建一个文件，该文件存储两个整数，依次表示属性个数(列数，countOfCol)，数据条数(行数，countOfRow)，两个整数用空格分开；接下来存存储若干行字符串，每一行依次为 属性名称 数据类型 是否非空 是否主键，字符串之间用空格分开；最后按行记录表格内容
        for (auto tab : db.second.table_list)
        {
            fout_db<<tab.first<<' ';
            std::fstream fout_tab;
            fout_tab.open(tab.first, std::fstream::out);
            
            fout_tab<<tab.second.attr_list.size()
            <<' '<<tab.second.row_map.size()<<std::endl;
            
            for (auto attr : tab.second.attr_list)
            {
                fout_tab<<attr.name<<' '<<attr.type<<' ';
                
                if (attr.Null) fout_tab<<"y ";
                else fout_tab<<"n ";
                
                if (attr.Key) fout_tab<<"y\n";
                else fout_tab<<"n\n";
            }
            
            //如果数据条数为0，则该表格到此存档完毕，立即开始下一张表格的存档
            if (tab.second.row_map.size() == 0) {
                fout_tab.close();
                continue;
            }
            
            //每一行的数据按照属性名字在map中的排序存储
            for (auto row : tab.second.row_map)
                for (auto value : row.second.data)
                {
                    fout_tab<<value.second<<' ';
                }
            
            fout_tab.close();
        }
        
        fout_db.close();
    }
    
    fout_dbname.close();
}

void DatabaseMap::Loading(){
 //   std::cout<<"————Loading————"<<std::endl;
    
    std::fstream fin_dbname;
    fin_dbname.open("DatabaseNames", std::fstream::in);
    
    //如果数据库数目为零，则加载到此完毕
    int countOfDB = 0;
    fin_dbname>>countOfDB;
    if (countOfDB == 0) {
        fin_dbname.close();
       // std::cout<<"————Complete————"<<std::endl; 
		return;
    }
    
    std::string DatabaseName;
    for (int i=0; i < countOfDB; i++) {
        DatabaseName = "";
        fin_dbname >> DatabaseName;
        CreateDatabase(DatabaseName);
        UseDatabase(DatabaseName);
        
        std::fstream fin_db;
        fin_db.open(DatabaseName, std::fstream::in);
        
        //如果表格数目为0，则该数据库的加载完毕，立即进行下一个数据库的加载
        int countOfTab = 0;
        fin_db>>countOfTab;
        if (countOfTab == 0) {
            fin_db.close();
            continue;
        }
        
        std::string tabName;
        for (int i=0; i<countOfTab; i++) {
            tabName = "";
            fin_db >> tabName;
            
            std::fstream fin_tab;
            fin_tab.open(tabName, std::fstream::in);
            
            std::vector<Attribute> attrs;
            
            int countOfcol = 0, countOfRow = 0;
            fin_tab >> countOfcol >> countOfRow;
            
            std::string attrName, type, isnull, iskey, Key = "";
            for (int i=0; i<countOfcol; i++) {
                attrName = type = isnull = iskey = "";
                fin_tab >> attrName >> type
                >> isnull >> iskey;
                
                bool null = false, key = false;
                if (isnull == "y") null = true;
                if (iskey == "y") {
                    key = true;
                    Key = attrName;
                }
                
                Attribute tmp;
                tmp.name = attrName;
                tmp.type = type;
                tmp.Null = null;
                tmp.Key = key;
                
                attrs.push_back(tmp);
            }
            
            CreateTable(tabName, attrs, Key);
            
            //如果数据条数为0，则该表格到此加载完毕，立刻进行下一张表格的加载
            if (countOfRow == 0) {
                fin_tab.close();
                continue;
            }
            //数据要按照属性的名称在map中的排序插入，利用set的自动排序功能给上面的vector attrs排序
            std::set<std::string> attrs_set;
            std::vector<std::string> attrs_vec;
            for (auto x : attrs)
                attrs_set.insert(x.name);
            
            for (auto x : attrs_set)
                attrs_vec.push_back(x);
            
            
            for (int i=0; i<countOfRow; i++) {
                std::vector<std::string> values;
                std::string val;
                for (int j=0; j<countOfcol; j++) {
                    val = "";
                    fin_tab>>val;
                    values.push_back(val);
                }
                InsertInto(tabName, attrs_vec, values);
                values.clear();
            }
            
            fin_tab.close();
        }
        
        fin_db.close();
    }
    
    fin_dbname.close();
    
    //std::cout<<"————Complete————"<<std::endl;
}
