// TableClass1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Table.h"
#include <iostream>

bool testing = true;

void crossProduct(Table _table1, Table _table2);
vector<Table> createTable(vector<Table> _tablist, vector<Attribute> _attr, string _name);
vector<Table> dropTable(vector<Table> _tablist, string _name);
void renameAttr(Table &_table, Attribute _attr, string _newName);

int main(int argc, _TCHAR* argv[])
{
	vector<Table> tableList;
	//Table 1 Test: ORDER TABLE
	Attribute order_id("order_id", "int", "primary key");
	Attribute order_paymentMethod("order_paymentMethod", "string");
	
	vector<Attribute> aList = {order_id, order_paymentMethod};

	Table order_table(aList, "Order Table");
	
	vector <string> rowrow = { "0", "credit" };
	vector <string> rowyourboat = { "1", "counterfit?" };

	order_table.pushBackRow(rowrow);
	order_table.pushBackRow(rowyourboat);
	
	order_table.printTable(order_table);

	//Table 2 Test: USER Table
	Attribute user_id("user_id", "int", "primary key");
	Attribute user_name("user_name", "string");
	Attribute privilege_level("privilege_level", "string");

	vector<Attribute> user_attributes = { user_id, user_name, privilege_level };

	Table user_table(user_attributes, "User Table");

	vector<string> newRow = { "0", "Bobby", "admin" };
	vector<string> newer = { "1", "Will", "guest" };
	vector<string> newest = { "2", "John", "guest" };

	user_table.pushBackRow(newRow);
	user_table.pushBackRow(newer);
	user_table.pushBackRow(newest);
	user_table.printTable(user_table);
	
	renameAttr(user_table, privilege_level, "access_level");

	user_table.printTable(user_table);

	crossProduct(order_table, user_table);
	tableList.push_back(user_table);
	tableList.push_back(order_table);
	cout << "table List size: " << tableList.size() << endl;
	tableList = createTable(tableList, user_attributes, "newTable");
	cout << "new tablist size: " << tableList.size() << endl;


	

	return 1;
}

//print out every possible combination of rows in table 1 with rows in table 2.
void crossProduct(Table _table1, Table _table2)
{
	std::cout << "Cross Product Function will print all possible combinations of rows in first column"
		<< " with rows in second column." << endl << endl;
	for (int x = 0; x < _table1.getNumRows(); x++)
	{
		vector <string> table1RowPrimaryKeys; //stores primary key values for row 
		vector <string> table1RowPKnames; //stores primary key names for table 1

		for (int z = 0; z < _table1.getNumAttrs(); z++)
		{
			if (_table1.attrKeyAt(z) == "primary key")
			{
				table1RowPrimaryKeys.push_back(_table1.getRowAttr(x, z));
				table1RowPKnames.push_back(_table1.attrNameAt(z));
			}
		}

		for (int y = 0; y < _table2.getNumRows(); y++)
		{
			//print Table 1 name and primary keys || table 2 name and primary keys of the specific row we are in right now

			vector <string> table2RowPrimaryKEys; //stores primary key values for row
			vector <string> table2RowsPKnames; //stores primary key names for table 2
			for (int a = 0; a < _table2.getNumAttrs(); a++)
			{
				if (_table2.attrKeyAt(a) == "primary key");
				{
					table2RowPrimaryKEys.push_back(_table2.getRowAttr(y, a));
					table2RowsPKnames.push_back(_table2.attrNameAt(a));
				}
			}
			
			std::cout << _table1.getTableName() << ": ";
			for (int b = 0; b < table1RowPKnames.size(); b++)
			{
				std::cout << table1RowPKnames[b] << " = " << table1RowPrimaryKeys[b];
			}

			std::cout << endl << "	" << _table2.getTableName();

			for (int c = 0; c < table2RowPrimaryKEys.size(); c++)
			{
				std::cout << ": " << table2RowsPKnames[c] << " = " << table2RowPrimaryKEys[c] <<" ";
			}
			cout << endl;
		}
	}
}

void renameAttr(Table &_table, Attribute _attr, string _newName)
{
	bool found = false;
	for (int x = 0; x < _table.getNumAttrs(); x++)
	{
		if (_table.attrNameAt(x) == _attr.getName())
		{
			_table.setAttrNameAt(x, _newName);
			found = true;
		}
	}
	if (!found)
	{
		std::cout << "Attribute was not found in renameAttr() function." << endl;
	}
	else if (testing)
	{
		std::cout << "Successfully changed name of attribute." << endl;
	}
}
//create table function in thje list of operating functions 
vector<Table> createTable(vector<Table> _tablist, vector<Attribute> _attr, string _name){
	Table newTab(_attr, _name);
	_tablist.push_back(newTab);
	return _tablist;

}

//function to remove a table from the list of operating tables
vector<Table> dropTable(vector<Table> _tablist, string _name){
	int listSize = _tablist.size();
	if (listSize == 0){
		cout << "dropTable called on empty Table vector!" << endl; 
		return _tablist;
	}
	else{
		string tableName;
		int perp = 0;
		bool found = false;
		cout << "size of list: " << listSize << endl;
		for (int x = 0; x < listSize; x++){
			tableName = _tablist[x].name;
			if (_name == tableName)
				perp = x;
				found = true;
		}
		if (found == true){
		_tablist.erase(_tablist.begin() + perp);
		cout << "size after erase: " << _tablist.size();
		}
		return _tablist;
	}
}