// TableClass1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <tchar.h>
#include "Table.h"
#include <iostream>

bool testing = true;

void crossProduct(Table _table1, Table _table2);
void projection(Table _table, string _attrName);
void setDifference(Table _tab1, Table _tab2);
vector<Table> createTable(vector<Table> _tablist, vector<Attribute> _attr, string _name);
vector<Table> dropTable(vector<Table> _tablist, string _name);
void renameAttr(Table &_table, Attribute _attr, string _newName);
void updateValue(Table &_table, string rowPrimaryKey, Attribute _attr, string newValue);
void deleteRow(Table &_table, string primaryKey);
void insertRow(Table &_table, vector <string> addRow);

int main(int argc, _TCHAR* argv[])
{
	vector<Table> tableList;
	//Table 1 Test: ORDER TABLE
	Attribute order_id("order_id", "int", "primary key");
	Attribute order_paymentMethod("order_paymentMethod", "string");

	vector<Attribute> aList = { order_id, order_paymentMethod };

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
	Table user_table2(user_attributes, "User Table 2");
	vector<string> newRow = { "0", "Bobby", "admin" };
	vector<string> newer = { "1", "Will", "guest" };
	vector<string> newest = { "2", "John", "guest" };

	user_table.pushBackRow(newRow);
	user_table.pushBackRow(newer);
	user_table.pushBackRow(newest);
	user_table.printTable(user_table);

	renameAttr(user_table, privilege_level, "access_level");

	user_table.printTable(user_table);
	
	cout << endl;

	crossProduct(order_table, user_table);
	projection(user_table, "user_id");
	projection(user_table, "user_name");
	setDifference(user_table, order_table);
	setDifference(order_table, user_table);

	cout << endl;
	cout << "UPDATE VALUE" << endl;
	updateValue(user_table, "0", user_id, "100");
	user_table.printTable(user_table);
	
	cout << endl;
	vector <string> newuserrowwww = { "0", "!xobile", "guest" };
	insertRow(user_table, newuserrowwww);
	user_table.printTable(user_table);

	cout << endl;
	deleteRow(user_table, "0");
	user_table.printTable(user_table);
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
				std::cout << ": " << table2RowsPKnames[c] << " = " << table2RowPrimaryKEys[c] << " ";
			}
			cout << endl;
		}
	}
}

// print the same attribute from every row of a table
void projection(Table _table, string _attrName){
	int desired;
	bool match = false;
	vector<vector<string>> rowList;
	vector<Attribute> atts;
	vector<string> names;
	atts = _table.attributes;
	rowList = _table.rows;
	int numRows = rowList.size();
	int attSize = atts.size();
	for (int x = 0; x < attSize; x++){
		names.push_back(atts[x].name);
	}
	for (int y = 0; y < attSize; y++){
		if (_attrName == names[y]){
			desired = y;
			match = true;
		}
	}
	if (match = true){
		cout << "desired attribute: " << _attrName << endl;
		for (int z = 0; z < numRows; z++){
			cout << rowList[z][desired] << endl;
		}
	}
	else cout << "desired attribute not in table" << endl;

}

// What does table 1 hold, that table 2 is missing
void setDifference(Table _tab1, Table _tab2){
	bool match = true;
	bool rowMatch = false;
	int numAtt = _tab1.attributes.size();
	int numAtt2 = _tab2.attributes.size();
	int numRow1 = _tab1.rows.size();
	int numRow2 = _tab2.rows.size();
	vector<vector<string>> RowN = _tab1.rows;
	vector<vector<string>> RowN2 = _tab2.rows;
	vector<vector<string>> out;
	vector<string> temp;
	int outSize;
	int rowSize;
	int tempSize;

	for (int r = 0; r < numRow1; r++){
		for (int q = 0; q < numRow2; q++){
			if (RowN[r] == RowN2[q]){
				rowMatch = true;
			}
		}
		if (rowMatch == false){
			out.push_back(RowN[r]);
		}
	}
	outSize = out.size();
	cout << "Set Difference between: " << _tab1.name << " & " << _tab2.name << endl;
	for (int p = 0; p < outSize; p++){
		temp = out[p];
		tempSize = temp.size();
		for (int h = 0; h < tempSize; h++){
			cout << temp[h] << "     ";
		}
		cout << endl;
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

//create table function in the list of operating functions 
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

void updateValue(Table &_table, string rowPrimaryKey, Attribute _attr, string newValue)
{
	int rowloc, attrloc;
	for (int x = 0; x < _table.getNumRows(); x++)
	{
		//find location in row vector of item requested using primary key
		if (_table.getPrimaryKey(x) == rowPrimaryKey)
		{
			rowloc = x;
			break;
		}
	}

	for (int x = 0; x < _table.getNumAttrs(); x++)
	{
		if (_attr.getKeyType() == _table.attrKeyAt(x) && _attr.getName() == _table.attrNameAt(x) && _attr.getType() == _table.attrTypeAt(x))
		{
			attrloc = x;
			break;
		}
	}

	_table.updateVal(rowloc, attrloc, newValue);
}

void insertRow(Table &_table, vector <string> addRow)
{
	_table.pushBackRow(addRow);
}

void deleteRow(Table &_table, string primaryKey)
{
	int rowloc;
	for (int x = 0; x < _table.getNumRows(); x++)
	{
		//find location in row vector of item requested using primary key
		if (_table.getPrimaryKey(x) == primaryKey)
		{
			rowloc = x;
			break;
		}
	}
	_table.deleteRowAtLoc(rowloc);
}