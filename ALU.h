#pragma once

#include <stack>
#include <string>
#include <regex>
#include <map>
#include <set>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>

#include "Table.h"

class ALU
{
public:
	ALU(std::string str) :expression(str) ,table(nullptr){}
	ALU(std::string _expression, Table* _table) :expression(_expression) { table = _table; }
	~ALU() = default;

	void process();
	friend class Command;

private:
	std::string expression;
	static std::map<std::string, int> priority;
	static std::set<std::string> function;
	static std::regex operators;
	Table* table;

	std::vector<std::string> Transfer(std::vector<std::string>&);

	std::string Calculate(std::vector<std::string>&);
	
	void output(std::vector<std::string>& expression);
	void ALUformat(std::string& expression);
};

inline std::string toUpper(const std::string& str);
bool IsDouble(const std::string& result);
std::vector<std::string> split(const std::string& str, const std::string& sep);
