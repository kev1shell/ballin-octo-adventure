// TableClass1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Table.h"

int main(int argc, _TCHAR* argv[])
{

	Attribute user_id("user_id", "int");
	Attribute user_name("user_name", "string");
	Attribute privilege_level("privilege_level", "string");

	vector<Attribute> attributeList = { user_id, user_name, privilege_level };

	Table table(attributeList);

	vector<string> newRow = {"0","Bobby","admin"};
	vector<string> newer = { "1", "Will", "guest" };

	table.pushBackRow(newRow);
	table.pushBackRow(newer);
	table.printTable(table);

	return 0;
}



