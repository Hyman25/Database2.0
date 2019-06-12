#include "Command.h"
using namespace std;

vector<string> split(const string& str, const string& sep)
{
	vector<string> tmp;
	string::size_type pos[2] = { 0, str.find(sep) };
	while (string::npos != pos[1])
	{
		tmp.push_back(str.substr(pos[0], pos[1] - pos[0]));
		pos[0] = pos[1] + sep.size();
		pos[1] = str.find(sep, pos[0]);
	}
	if (pos[0] != str.length())
		tmp.push_back(str.substr(pos[0]));

	return tmp;
}

inline void trim(std::string& s)
{
	if (!s.empty())
	{
		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
	}
}

inline string getFirstSubstr(string& str, const string& sep) {
	string::size_type pos = str.find(sep);
	if (pos != string::npos) {
		string tmp = str.substr(0, pos);
		str.erase(0, pos + 1);
		return tmp;
	}
	else {//将输出字符串清空，返回原字符串
		string tmp = str;
		str.clear();
		return tmp;
	}
}

inline string toUpper(const string& str) {
	string tmp(str);
	for (int i = 0; i < (int)tmp.size(); ++i)
		tmp[i] = toupper(tmp[i]);
	return tmp;
}

void Command::operate() {
	FormatSQL();

	string order = toUpper(getFirstSubstr(buffer, " "));

	if (order == "CREATE")
		Create();
	else if (order == "DROP")
		Drop();
	else if (order == "DELETE")
		Delete();
	else if (order == "SELECT")
		Select();
	else if (order == "INSERT")
		Insert();
	else if (order == "UPDATE")
		Update();
	else if (order == "SHOW")
		Show();
	else if (order == "USE")
		Use();
	else if (order == "LOAD")
		Load();
	else if (order == "LINK")
		Link();
	else if (order == "SAVE")
		Save();
}

void Command::FormatSQL()
{
	/*换行,tab,",'回车用" "代替*/
	regex reg("[\r\n\t\"']");
	buffer = regex_replace(buffer, reg, " ");
	/*去除分号之后的内容*/
	reg = ";.*$";
	buffer = regex_replace(buffer, reg, "");
	//去除最前和最后的" "
	reg = "(^ +)|( +$)";
	buffer = regex_replace(buffer, reg, "");
	/*去除重复的空格*/
	reg = " +";
	buffer = regex_replace(buffer, reg, " ");
	/*将,()左右的空格去掉*/
	reg = " ?(\\)|\\(|,|=) ?";
	buffer = regex_replace(buffer, reg, "$1");
}

void Command::Create()
{
	string order = toUpper(getFirstSubstr(buffer, " "));

	if (order == "DATABASE") {
		string dbname = getFirstSubstr(buffer, " ");
		if (!dbname.empty()) DB.CreateDatabase(dbname);
	}
	else if (order == "TABLE") {
		string tableName = getFirstSubstr(buffer, "(");

		//将( )转换成空格
		regex reg("(\\(|\\))");
		buffer = regex_replace(buffer, reg, " ");
		//去除首尾的" "
		reg = "(^ +)|( +$)";
		buffer = regex_replace(buffer, reg, "");
		//去除,周围的空格
		reg = " ?, ?";
		buffer = regex_replace(buffer, reg, ",");

		vector<string> TableAttr = split(buffer, ",");
		vector<Attribute> attr;
		string PrimaryKey;

		for (int i = 0; i < (int)TableAttr.size(); i++) {
			vector<string> ColumnAttr = split(TableAttr[i], " ");

			string PRIMARY = toUpper(ColumnAttr[0]), KEY = toUpper(ColumnAttr[1]);

			if (PRIMARY == "PRIMARY" && KEY == "KEY" && ColumnAttr.size() == 3) {
				PrimaryKey = ColumnAttr[2];
				continue;
			}
			Attribute tmp;
			tmp.name = ColumnAttr[0];
			string type = toUpper(ColumnAttr[1]);
		
			if (type == "INT")	tmp.type = "int";
			else if (type == "CHAR") tmp.type = "char";
			else if (type == "DOUBLE") tmp.type = "double";
			else return;

			tmp.Null = !(((ColumnAttr.size() == 4) && 
				(toUpper(ColumnAttr[2]) + " " + toUpper(ColumnAttr[3]) == "NOT NULL")));
			
			attr.push_back(tmp);
		}

		for (int i = 0; i < (int)attr.size(); i++)
			if (PrimaryKey == attr[i].name) {
				attr[i].Key = true;
				break;
			}

		DB.CreateTable(tableName, attr, PrimaryKey);
	}
}

void Command::Use() {
	string dbname = getFirstSubstr(buffer, " ");
	DB.UseDatabase(dbname);
}

void Command::Drop() {
	vector<string> orders = split(buffer, " ");
	if (orders.size() == 2) {
		orders[0] = toUpper(orders[0]);
		string name = orders[1];

		if (orders[0] == "DATABASE")
			DB.DropDatabase(name);
		else if (orders[0] == "TABLE") {
			DB.DropTable(name);
		}
	}
}

void Command::Show() {
	vector<string> orders = split(buffer, " ");
	if (orders.size() == 1) {
		orders[0] = toUpper(orders[0]);
		if (orders[0] == "DATABASES")
			DB.ShowDatabases();
		else if (orders[0] == "TABLES") {
			DB.ShowTables();
		}
	}
	else if (orders.size() == 3) {
		orders[0] = toUpper(orders[0]);
		orders[1] = toUpper(orders[1]);
		if (orders[0] + " " + orders[1] == "COLUMNS FROM") {
			DB.ShowColumns(orders[2]);
		}
	}
	else {
		std::cout << "ERROR!\n";
	}
}

void Command::Update() {
	string table_name = getFirstSubstr(buffer, " ");

	string SET = toUpper(getFirstSubstr(buffer, " "));
	if (SET != "SET") return;  //输入异常

	vector<std::pair<string, string> > UpdateAttr;

	vector<string> UpdateTmp = split(getFirstSubstr(buffer, " "), ",");
	for (int i = 0; i < (int)UpdateTmp.size(); i++)
	{
		vector<string> Attr_tmp = split(UpdateTmp[i], "=");
		if (Attr_tmp.size() != 2) return;

		std::pair<string, string> tmp(Attr_tmp[0], Attr_tmp[1]);
		UpdateAttr.push_back(tmp);
	}

	string WHERE = toUpper(getFirstSubstr(buffer, " "));
	if (WHERE != "WHERE") return;

	std::set<Data> key_of_rows = where_clause(table_name, buffer);
	int k = UpdateAttr.size();
	for (auto i = key_of_rows.begin(); i != key_of_rows.end(); i++)
		for (int j = 0; j < k; j++)
			DB.Set(table_name, UpdateAttr[j].first, UpdateAttr[j].second, (*i).value);
}

void Command::Insert() {
	string INTO = toUpper(getFirstSubstr(buffer, " "));
	if (INTO != "INTO") return;  //输入异常

	string table_name = getFirstSubstr(buffer, "(");

	vector<string> attr_list = split(getFirstSubstr(buffer, ")"), ",");

	string VALUES = toUpper(getFirstSubstr(buffer, "("));
	if (VALUES != "VALUES") return; //输入异常

	vector<string> value_list = split(getFirstSubstr(buffer, ")"), ",");

	DB.InsertInto(table_name, attr_list, value_list);
}

void Command::Delete() {
	string FROM = toUpper(getFirstSubstr(buffer, " "));
	if (FROM != "FROM") return;  //输入异常

	string table_name = getFirstSubstr(buffer, " ");

	string WHERE = toUpper(getFirstSubstr(buffer, " "));
	if (WHERE != "WHERE") return;

	std::set<Data> key_of_rows = where_clause(table_name, buffer);
	for (auto i = key_of_rows.begin(); i != key_of_rows.end(); i++) {
		DB.DeleteRow(table_name, (*i).value);
	}
}

void Command::Select() {
	string CountAttr = "", FileName = "", orderby = "", orderbyCount = "", whereclause = "";
	vector<string> Columns, groupbyAttr;
	int countpos = -1;

	string tmp = toUpper(buffer), str;
	string::size_type pos = tmp.find("FROM");//将select语句按照from关键词分开
	if (pos != string::npos && tmp[pos - 1] == ' ')
		str = buffer.substr(0, pos - 1);
	else str = buffer.substr(0, pos);
	buffer.erase(0, pos);

	if (buffer.empty()) {//没有form，必定是算术表达式
		vector<string> expressions = split(str, ",");
		DB.ALUprocess(expressions);
		return;
	}

	string FROM = toUpper(getFirstSubstr(buffer, " "));
	if (FROM != "FROM") return;  //输入异常
	string table_name = getFirstSubstr(buffer, " ");

	if (!DB.existTable(table_name)) return;
	Table& table = DB.getDatabase().getTable(table_name);

	regex reg(" ?INTO OUTFILE ?", regex::icase);//不区分大小写

	if (regex_search(str, reg)) {
		str = regex_replace(str, reg, " ");
		pos = str.find_last_of(" ");
		FileName = str.substr(pos + 1, str.size() - pos - 1);
		str.erase(pos, str.size() - pos);
	}
	string tmpColumns = str;

	vector<string> AttrNewName;
	if (tmpColumns == "*") {
		for (int i = 0; i < (int)table.attr_list.size(); ++i) {
			Columns.push_back(table.attr_list[i].name);
		}
		AttrNewName = Columns;
	}

	else {
		Columns = split(tmpColumns, ",");
		AttrNewName = Columns;
		for (int i = 0; i < (int)Columns.size(); ++i) {
			//判断count(expression)
			string tmp = Columns[i];
			string COUNT = getFirstSubstr(tmp, "(");
			if (toUpper(COUNT) == "COUNT") {
				countpos = i;
				CountAttr = getFirstSubstr(tmp, ")");
			}
			//判断有没有as

			regex as("( |\\))(as)( |\\()", regex::icase);
			if (regex_search(Columns[i], as)) {
				Columns[i] = regex_replace(Columns[i], as, "$1 $2 $3");
				as = " +";
				Columns[i] = regex_replace(Columns[i], as, " ");

				string tmp = toUpper(Columns[i]);
				auto pos = tmp.find(" AS");
				AttrNewName[i] = Columns[i].substr(pos + 4, Columns[i].size() - (pos + 4));
				Columns[i] = Columns[i].substr(0, pos);
			}
		}
	}
	//提取where子句
	tmp = toUpper(buffer);
	pos = tmp.find("GROUP");
	if (pos == string::npos)
		pos = tmp.find("ORDER");
	whereclause = buffer.substr(0, pos); trim(whereclause);
	buffer.erase(0, pos);
	if (!whereclause.empty()) {
		string WHERE = toUpper(getFirstSubstr(whereclause, " "));
		if (WHERE != "WHERE") return;
	}

	//提取group子句
	tmp = toUpper(buffer);
	pos = tmp.find("ORDER");
	string tmp_group = buffer.substr(0, pos); trim(tmp_group);
	buffer.erase(0, pos);
	if (!tmp_group.empty()) {
		string GROUP = toUpper(getFirstSubstr(tmp_group, " "));
		if (GROUP != "GROUP") return;
		string BY = toUpper(getFirstSubstr(tmp_group, " "));
		if (BY != "BY") return;
	}
	groupbyAttr = split(tmp_group, ",");

	//提取order子句
	if (!buffer.empty()) {
		string ORDER = toUpper(getFirstSubstr(buffer, " "));
		if (ORDER != "ORDER") return;
		string BY = toUpper(getFirstSubstr(buffer, " "));
		if (BY != "BY") return;
	}
	orderby = buffer; trim(orderby);
	tmp = orderby;
	string COUNT = getFirstSubstr(tmp, "(");
	if (toUpper(COUNT) == "COUNT")
		orderbyCount = getFirstSubstr(tmp, ")");

	table.SelectData(Columns, AttrNewName, countpos, CountAttr, 
		groupbyAttr, orderby, orderbyCount, whereclause, FileName);
}


void Command::Load()
{
	vector<string> orders = split(buffer, " ");
	string fileName;
	bool error = true;
	if (orders.size() == 6 &&
		toUpper(orders[0]) == "DATA" &&
		toUpper(orders[1]) == "INFILE" &&
		toUpper(orders[3]) == "INTO" &&
		toUpper(orders[4]) == "TABLE") {
		fileName = orders[2];
		error = false;
	}
	else if (orders.size() == 7 &&
		toUpper(orders[0]) == "DATA" &&
		toUpper(orders[1]) == "LOCAL" &&
		toUpper(orders[2]) == "INFILE" &&
		toUpper(orders[4]) == "INTO" &&
		toUpper(orders[5]) == "TABLE") {
		fileName = orders[3];
		error = false;
	}
	if (error) return; //报错模块

	string tableAttr = orders[orders.size() - 1],
		tableName = getFirstSubstr(tableAttr, "(");

	if (!DB.existTable(tableName))	return;

	Table& table = DB.getDatabase().getTable(tableName);

	vector<string> Columns;
	if (!tableAttr.empty()) {
		Columns = split(getFirstSubstr(tableAttr, ")"), ",");
	}
	else {
		for (auto i : table.attr_list)
			Columns.push_back(i.name);
	}
	table.LoadFile(fileName, Columns);
}

void Command::Link()
{
#ifdef INTERNET
	string order = toUpper(getFirstSubstr(buffer, " "));
	if (order == "CLIENT")
		LinkAsServer();
	else if (order == "SERVER")
		LinkAsClient();
#endif // WIN32
}

void Command::Save()
{
	DB.Saving();
}

std::set<Data> where_clause(std::string table_name, std::string clause) {
	//注意：这里默认每个条件句之内没有空格
	regex reg(" ?(>|<|=) ?");
	clause = regex_replace(clause, reg, "$1");

	std::stringstream ss(clause);
	std::vector<Clause> conditions;
	std::vector<std::string> operators;
	std::vector<std::set<Data>> keys;
	while (true) {
		Clause c;
		std::string str;
		ss >> str;
		int i = 0;
		for (; i < (int)str.length(); i++) {
			if (str[i] == '=' || str[i] == '<' || str[i] == '>') {
				c.cmp_op = str[i];
				break;
			}

		}
		c.left = str.substr(0, i);
		if (c.left[0] == '"' || c.left[0] == '\'') {
			c.left = c.left.substr(1, c.left.length() - 2);
		}
		std::string right = str.substr(i + 1);
		//c.right = str.substr(i + 1);//错误，右值字符串为"\"\a""
		if (right[0] == '"' || right[0] == '\'') {
			c.right = right.substr(1, right.length() - 2);
		}
		else
			c.right = str.substr(i + 1);
		conditions.push_back(c);
		keys.push_back(DB.KeyWhereCluase(table_name, c));
		std::string op;
		if (ss >> op) {
			operators.push_back(op);
		}
		else {
			break;
		}
	}
	auto it1 = keys.begin();
	auto it2 = operators.begin();
	while (it2 < operators.end()) {
		if ((*it2) == "AND" || (*it2) == "and") {
			std::set<Data> s;
			std::set_intersection((*it1).begin(), (*it1).end(), (*(it1 + 1)).begin(), (*(it1 + 1)).end(), std::inserter(s, s.begin()));
			*(it1 + 1) = s;
			it1 = keys.erase(it1);
			it2 = operators.erase(it2);
		}
		else if ((*it2) == "OR" || (*it2) == "or") {
			it1++;
			it2++;
		}
		else {
			std::cout << "ERROR!\n";
		}
	}
	it1 = keys.begin();
	while (keys.size() > 1) {
		std::set<Data> s;
		std::set_union((*it1).begin(), (*it1).end(), (*(it1 + 1)).begin(), (*(it1 + 1)).end(), std::inserter(s, s.begin()));
		(*it1) = s;
		keys.erase(it1 + 1);
	}
	return keys[0];
}

#ifdef INTERNET
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <Winsock2.h>
#pragma comment(lib,"ws2_32.lib")

WSADATA wsaData;
SOCKET sockServer;
SOCKADDR_IN addrServer;
SOCKET sockClient;
SOCKADDR_IN addrClient;

void LinkAsServer()
{
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	sockServer = socket(AF_INET, SOCK_STREAM, 0);
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//INADDR_ANY表示任何IP
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(6000);//绑定端口6000
	bind(sockServer, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));

	//  存放主机名的缓冲区
	char szHost[256];
	//  取得本地主机名称
	::gethostname(szHost, 256);
	//  通过主机名得到地址信息，一个主机可能有多个网卡，多个IP地址
	hostent* pHost = ::gethostbyname(szHost);
	in_addr addr;
	for (int i = 0;; i++)
	{
		//获得地址（网络字节序）
		char* p = pHost->h_addr_list[i];
		if (p == NULL)
		{
			break;
		}
		//  将地址拷贝到in_addr结构体中
		memcpy(&addr.S_un.S_addr, p, pHost->h_length);
		//  将in_addr转换为主机字节序
		char* strIp = ::inet_ntoa(addr);
		//  输出
		cout << "————————————————" << endl;
		printf("本机IP[%d]:%s\n", i + 1, strIp);
	}

	cout << "使用端口：6000" << endl;
	cout << "————————————————" << endl;

	//Listen监听端
	listen(sockServer, 5);//5为等待连接数目
	printf("服务器已启动:\n监听中...\n");
	int len = sizeof(SOCKADDR);
	char sendBuf[1000] = {};//发送至客户端的字符串
	char recvBuf[1000] = {};//接受客户端返回的字符串
	//会阻塞进程，直到有客户端连接上来为止
	sockClient = accept(sockServer, (SOCKADDR*)& addrClient, &len);
	cout << "————连接客户端————" << endl;

	while (true)
	{
		//接收并打印客户端数据
		if (recv(sockClient, recvBuf, 1000, 0) <= 0)
			break;
		cout << recvBuf << endl;
		string buf(recvBuf);

		regex reg("\n");//不知道为什么有时候exit前面会有\n
		buf = regex_replace(buf, reg, "");
		if (toUpper(buf) == "EXIT")
			break;

		streambuf* coutBuf = cout.rdbuf();

		ofstream of("out.txt");
		// 获取文件out.txt流缓冲区指针
		streambuf* fileBuf = of.rdbuf();

		// 设置cout流缓冲区指针为out.txt的流缓冲区指针
		cout.rdbuf(fileBuf);
		string s(recvBuf);
		Command com(s);
		com.operate();
		of.flush();
		of.close();
		cout.rdbuf(coutBuf);

		ifstream fin;
		fin.open("out.txt");
		string tmp, contents;
		int i = 0;
		while (!fin.eof())
		{
			getline(fin, tmp);
			if (!tmp.empty()) {
				cout << tmp << endl;
				contents += tmp + "\n";
			}
		}
		fin.close();

		send(sockClient, contents.c_str(), contents.size() + 1, 0);
	}
	cout << "————已断开连接————" << endl;
	//关闭socket
	closesocket(sockClient);
	WSACleanup();
}

void LinkAsClient() {
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	//新建客户端socket
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	//定义要连接的服务端地址
	std::string s;
	int port;
	cout << "————————————————" << endl;
	std::cout << "输入要连接的Ip地址与端口号：" << std::endl;
	std::cin >> s >> port; getchar();
	addrServer.sin_addr.S_un.S_addr = inet_addr(s.c_str());//目标IP(127.0.0.1是回送地址)
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(port);//连接端口6000
	//连接到服务端
	int error = connect(sockClient, (SOCKADDR*)& addrServer, sizeof(SOCKADDR));

	if (!error) cout << "Connected!\n";
	else if (error == SOCKET_ERROR) {
		cout << "ERROR!\n";
		return;
	}

	//发送数据
	cout << "————连接服务器————" << endl;
	string message;
	while (true)
	{
		getline(cin, message, ';');

		char* tmp = const_cast<char*>(message.c_str());
		send(sockClient, message.c_str(), message.size() + 1, 0);

		regex reg("\n");//不知道为什么有时候exit前面会有\n
		message = regex_replace(message, reg, "");
		if (toUpper(message) == "EXIT") break;

		char buf[1000] = {};
		error = recv(sockClient, buf, 1000, 0);

		if (error == SOCKET_ERROR) {
			cout << "ERROR!" << endl;
			break;
		}
		else {
			cout << message << " Done!" << endl;
			cout << buf; //<< std::endl;
		}

	}
	//关闭socket
	closesocket(sockClient);
	cout << "————已断开连接————" << endl;
	WSACleanup();
}

#endif // DEBUG