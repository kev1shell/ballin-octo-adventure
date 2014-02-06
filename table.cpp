#include "stdafx.h"
#include "Table.h"
#include <stdio.h>
#include <iostream>
#include "attribute.h"

Table::Table(vector<Attribute> _attributes){
	
	attributes = _attributes;

}

void Table::pushBackRow(vector<string> _newRow){
	
	if (attributes.size() == _newRow.size()){
	
		rows.push_back(_newRow);
	}

}
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
			cout << rower[q] << "     ";

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