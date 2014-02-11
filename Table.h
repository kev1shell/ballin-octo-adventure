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
	string getTableName();
	int getNumRows();
	int getNumAttrs();
	string getPrimaryKey(int rowLoc);	//IMPORTANT!!: if table has multiple primary keys the string returns appends the two keys in the order they show up in the table (left to right)
	string getRowAttr(int rowLoc, int attrLoc);
	string attrNameAt(int loc);
	string attrKeyAt(int loc);
	string attrTypeAt(int loc);
	vector<vector<string>> getRows();

	//Modifiers
	void setAttrNameAt(int loc, string _newname);
	void updateVal(int rowLoc, int attrLoc, string newValue);
	void deleteRowAtLoc(int rowloc);

	//Prints
	void printTable(Table _table);
	void printAttVec(Table _table);
	void printRow(vector<string> _row);

	//copy constructor
	Table& operator=(Table& table);

};

#endif