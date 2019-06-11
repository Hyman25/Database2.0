#include"Row.h"
#include"Table.h"
#include "Database.h"
#include "DatabaseMap.h"
#include "Command.h"

DatabaseMap DB;       

int main()
{
    DB.Loading();
	std::string command;
	while (std::getline(std::cin, command, ';')){
		Command a(command);
		a.operate();
	} 
}
