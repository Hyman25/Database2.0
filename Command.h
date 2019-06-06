#pragma once
#include <string>
#include <sstream>
#include <regex>
#include "DatabaseMap.h"
void Link();
extern DatabaseMap DB;

using std::string;

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

	friend std::vector<string> split(const string& str, const string& sep);
	friend inline string toUpper(const string& str);
	friend inline string getFirstSubstr(string& str, const string& sep);
};

std::set<Data> where_clause(std::string table_name, std::string clause);