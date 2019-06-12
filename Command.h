#pragma once
#include <string>
#include <sstream>
#include <regex>

#include "ALU.h"
#include "DatabaseMap.h"

extern DatabaseMap DB;

using std::string;

class ALU;

class Command {
public:
	Command(std::string c) :buffer(c) {}
	void operate();

private:
	std::string buffer;

	void FormatSQL();
	void Create();
	void Use();
	void Drop();
	void Show();
	void Update();
	void Insert();
	void Delete();
	void Select();
	void Load();
	void Link();
};

void LinkAsClient();
void LinkAsServer();
inline string toUpper(const string& str);
inline void trim(std::string& s);
inline string getFirstSubstr(string& str, const string& sep);
std::vector<string> split(const string& str, const string& sep);
std::set<Data> where_clause(std::string table_name, std::string clause);