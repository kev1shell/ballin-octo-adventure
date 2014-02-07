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
	
	Table(vector<Attribute> _attributes);
	
	//insert a new row onto end of the table
	void pushBackRow(vector<string> _newRow);

	
};

#endif