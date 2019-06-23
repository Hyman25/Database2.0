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

struct Data;

class ALU
{
public:
	ALU(std::string str) :expression(str){}
	~ALU() = default;

	std::vector<std::string> process();
	std::vector<std::string> process(Table* table, const std::set<Data>& keys);

	static bool IsALU(std::string str);
	static std::regex operators;

private:
	std::string expression;
	static std::map<std::string, int> priority;
	static std::set<std::string> function;


	static std::vector<std::string> Transfer(std::vector<std::string>&);
	static std::string Calculate(std::vector<std::string>&);
	static std::string DoubleToString(const double value);
	void ALUformat(std::string& expression);
};

inline std::string toUpper(const std::string& str);
bool IsDouble(const std::string& result);

std::vector<std::string> split(const std::string& str, const std::string& sep);
