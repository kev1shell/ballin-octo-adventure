// Tableclass1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <vector>
#include "attribute.h"
#include "Table.h"
using namespace std;
void select(Table table,Attribute attribute,string findthis);
int _tmain(int argc, _TCHAR* argv[])
{
	Attribute user_id("user_id", "int");
	Attribute user_name("user_name", "string");
	Attribute privilege_level("privilege_level", "string");

	vector<Attribute> attributeList;
	attributeList.push_back(user_id);
	attributeList.push_back(user_name);
	attributeList.push_back(privilege_level);
	Table table(attributeList);
	vector<string> newRow ;
	newRow.push_back("0");
	newRow.push_back("Bobby");
	newRow.push_back("admin");
	vector<string> newer;
	newer.push_back("1");
	newer.push_back("Will");
	newer.push_back("guess");
	string privilege="Will";
	table.pushBackRow(newRow);
	table.pushBackRow(newer);
	table.printTable(table);
	select(table,privilege_level,privilege);
	system("pause");
	return 0;
}
void select(Table table,Attribute attribute,string findthis)
{
  int i=0;
  int q=0;
  for(int j=0;j<table.attributes.size();j++)
  {
	if(table.attributes[j].name==attribute.name&&table.attributes[j].type==attribute.type)
	{
        q=j;	
	}
  }
  while(i<table.rows.size())
  {

	  if(table.rows[i][q]==findthis)
	  {
      table.printRow(table.rows[i]);
	  }
	  i++;
  }
}
