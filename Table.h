#ifndef Table_H
#define Table_H

#include <string.h>
#include <vector>

#include "attribute.h"


using namespace std;
class Table{
public:
	vector<Attribute> attributes;
	vector<vector<string>> rows;
	string name; 
	Table(vector<Attribute> _attributes, string NAME);

	//insert a new row onto end of the table
	void pushBackRow(vector<string> _newRow);

	//Accessors
	vector<vector<string>> getRows();
	string getTableName();
	int getNumRows();
	int getNumAttrs();
	string getRowAttr(int rowLoc, int attrLoc);
	string attrNameAt(int loc);
	string attrKeyAt(int loc);

	//Modifiers
	void setAttrNameAt(int loc, string _newname);

	//Prints
	void printTable(Table _table);
	void printAttVec(Table _table);
	void printRow(vector<string> _row);

	//copy constructor
	Table& operator=(Table& table);

};

#endif