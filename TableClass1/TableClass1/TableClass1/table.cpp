#include "stdafx.h"
#include "Table.h"

Table::Table(vector<Attribute> _attributes){
	
	attributes = _attributes;

}

void Table::pushBackRow(vector<string> _newRow){
	
	if (attributes.size() == _newRow.size()){
	
		rows.push_back(_newRow);
	}

}