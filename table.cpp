
#include <stdio.h>
#include <tchar.h>
#include "Table.h"
#include <stdio.h>
#include <iostream>
#include "attribute.h"


Table::Table(vector<Attribute> _attributes, string _NAME)
{

	attributes = _attributes;
	name = _NAME;

}


void Table::pushBackRow(vector<string> _newRow)
{

	if (attributes.size() == _newRow.size()){

		rows.push_back(_newRow);
	}

}

void Table::pushBackCommand(string _newCmd){

	commandHistory.push_back(_newCmd);
}

//***************************************************** ACCESSORS **********************************************************

string Table::getTableName()
{
	return name;
}

int Table::getNumRows()
{
	return rows.size();
}

int Table::getNumAttrs()
{
	return attributes.size();
}

string Table::getPrimaryKey(int rowLoc)
{
	//IF TABLE HAS MULTIPLE PRIMARY KEYS THIS APPENDS THE KEYS
	string primaryKey = "";
	for (int x = 0; x < getNumAttrs(); x++)
	{
		if (attributes[x].getKeyType() == "primary key")
		{
			primaryKey += rows[rowLoc][x];
		}
	}
	return primaryKey;
}

string Table::attrNameAt(int loc)
{
	return attributes[loc].getName();
}

string Table::getRowAttr(int rowLoc, int attrLoc)
{
	return rows[rowLoc][attrLoc];
}

string Table::attrKeyAt(int loc)
{
	return attributes[loc].getKeyType();
}

string Table::attrTypeAt(int loc)
{
	return attributes[loc].getType();
}

vector<string> Table::getRow(int loc)
{
	return rows[loc];
}

vector<vector<string>> Table::getRows()
{
	return rows;
}

vector<Attribute> Table::getAttributes()
{
	return attributes;
}
//**************************************************** MODIFIERS ***********************************************************
void Table::setAttrNameAt(int loc, string _newname)
{
	attributes[loc].setName(_newname);
}

void Table::updateVal(int rowLoc, int attrLoc, string newValue)
{
	rows[rowLoc][attrLoc] = newValue;
}

void Table::deleteRowAtLoc(int rowloc)
{
	rows.erase(rows.begin() + rowloc);
}

void Table::setName(string _name)
{
	name = _name;
}

//**************************************************** PRINT FUNCTIONS *****************************************************
void Table::printTable(Table _table){

	int numRows;
	int x;
	vector<vector<string>> row = _table.rows;
	numRows = row.size();
	_table.printAttVec(_table);
	cout << endl << endl;
	for (int w = 0; w < numRows; w++){
		_table.printRow(row[w]);
		cout << endl;
	}

}

void Table::printRow(vector<string> _row){
	int numElems;
	vector<string> rower = _row;
	numElems = rower.size();
	for (int q = 0; q < numElems; q++){

		cout << rower[q];

		if (attributes[q].getName().size() > rower[q].size())
		{
			for (int i = rower[q].size(); i < attributes[q].getName().size(); i++)
			{
				cout << " ";
			}
		}

		cout << "     ";

	}

}

void Table::printAttVec(Table _table){

	int numbRows;
	vector<Attribute> att = _table.attributes;
	numbRows = att.size();
	for (int x = 0; x < numbRows; x++){
		att[x].printAttribute(att[x]);
		cout << "     ";
	}

}

Table& Table::operator=(Table& table){

	name = table.getTableName();
	attributes = table.getAttributes();
	rows = table.getRows();

	return *this;

}