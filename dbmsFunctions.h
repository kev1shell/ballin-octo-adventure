
#include "Table.h"
#include <iostream>
#include <sstream>

using namespace std;

//all of our lowest level functions that operate directly on tables are contained in this namespace
namespace dbmsFunctions{
	//This Boolean Value is used to control the testing process 
	bool testing = false;

	//This function takes in an integer value and returns a string containing the same number
	string int2string(int number) {
		stringstream ss;//create a stringstream
		ss << number;//add number to the stream
		return ss.str();//return a string with the contents of the stream
	}

	//This function takes in 2 tables, and then returns a table containing the union of these 2 tables (if there attribute lists match)
	Table setUnion(Table& tableA, Table& tableB){

		Table result(tableA.attributes, "setUnion of " + tableA.name + tableB.name);

		Table* longTable;
		Table* shortTable;

		//ensure both tables have equal number of attributes
		if (tableA.attributes.size() != tableB.attributes.size()){
			cout << "Set Union Failed: attribute mismatch" << endl;
			return result;
		}

		//if table sizes are unequal, find larger table
		if (tableA.rows.size() < tableB.rows.size()){
			longTable = &tableB;
			shortTable = &tableA;
		}
		else{
			longTable = &tableA;
			shortTable = &tableB;
		}

		if (longTable == NULL || shortTable == NULL){
			cout << "Set Union Failed: Table pointer error" << endl;
			return result;
		}

		for (int i = 0; i < longTable->rows.size(); i++){

			result.pushBackRow(longTable->rows[i]);

		}


		for (int i = 0; i < shortTable->rows.size(); i++){

			vector<string> currentRow = shortTable->rows[i];

			if (result.rows.size() == 0){

				result.rows.push_back(currentRow);
				continue;
			}

			for (int row = 0; row < result.rows.size(); row++){

				int similarAttributes = 0;
				int numAttributes = currentRow.size();

				for (int col = 0; col < currentRow.size(); col++){

					if (currentRow[col] == result.rows[row][col]){

						similarAttributes++;

						if (similarAttributes == numAttributes){

							col = currentRow.size();
							row = result.rows.size();
							continue;
						}
					}
				}

				if (row == result.rows.size() - 1){

					result.rows.push_back(currentRow);
					continue;
				}
			}

		}

		return result;

	}

	//This function takes in 2 tables, and an attribute that they share, it returns a table containing the natural join of these tables
	Table naturalJoin(Table& tableA, Table& tableB, Attribute attribute){

		vector<Attribute> attributeList;

		Attribute NJkey("NJID", "int", "primary key");	//this will be the key of the resulting table

		attributeList.push_back(NJkey);
		attributeList.push_back(attribute);

		int atrbIndexA = 0;
		int atrbIndexB = 0;

		//add attributes of tableA and tableB to result table
		for (int i = 0; i < tableA.attributes.size(); i++){

			if (tableA.attributes[i].name != attribute.name){
				attributeList.push_back(tableA.attributes[i]);
			}
			else{
				atrbIndexA = i;
			}

		}

		for (int i = 0; i < tableB.attributes.size(); i++){

			if (tableB.attributes[i].name != attribute.name){
				attributeList.push_back(tableB.attributes[i]);
			}
			else{
				atrbIndexB = i;
			}

		}
		//-------

		Table result(attributeList, "naturalJoin of " + tableA.name + tableB.name);

		for (int i = 0; i < tableA.rows.size(); i++){

			vector<string> currentRowA = tableA.rows[i];

			for (int j = 0; j < tableB.rows.size(); j++){

				vector<string> currentRowB = tableB.rows[j];

				if (currentRowA[atrbIndexA] == currentRowB[atrbIndexB]){
					//match found pushback new row into result and continue
					vector<string> newRow;
					newRow.push_back(int2string(i));
					newRow.push_back(currentRowA[atrbIndexA]);

					for (int k = 0; k < currentRowA.size(); k++){
						if (k != atrbIndexA){
							newRow.push_back(currentRowA[k]);
						}
					}

					for (int k = 0; k < currentRowB.size(); k++){
						if (k != atrbIndexB){
							newRow.push_back(currentRowB[k]);
						}
					}

					result.pushBackRow(newRow);

				}
			}

		}

		return result;
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


	// This function takes in a table and a vector of desired attributes, it then returns a table containing every value associated 
	//with these attributes from the original table
	Table projection(Table _table, vector<Attribute> _attrVec){
		Table result(_attrVec, "projection from " + _table.name);
		vector<int> desired;
		bool match = false;
		vector<vector<string>> rowList;
		vector<Attribute> atts;
		vector<string> names;
		vector<string> _attrVecName;
		vector<string> currentRow;
		atts = _table.attributes;
		rowList = _table.rows;
		int numRows = rowList.size();
		int attSize = atts.size();
		for (int nameIt = 0; nameIt < _attrVec.size(); nameIt++){
			_attrVecName.push_back(_attrVec[nameIt].name);
		}
		for (int x = 0; x < attSize; x++){
			names.push_back(atts[x].name);
		}
		for (int attIt = 0; attIt < _attrVecName.size(); attIt++){
			for (int y = 0; y < attSize; y++){
				if (_attrVecName[attIt] == names[y]){
					desired.push_back(y);
					match = true;
				}
			}
		}
		if (match = true){
			for (int z = 0; z < numRows; z++){
				for (int iter = 0; iter < desired.size(); iter++){
					currentRow.push_back(rowList[z][desired[iter]]);
				}
				result.pushBackRow(currentRow);
				currentRow.clear();
			}
		}
		else cout << "desired attribute not in table" << endl;
		return result;

	}


	// this function takes in 2 tables and returns a table holding all the values that table 1 has and table 2 doesnt
	Table setDifference(Table _tab1, Table _tab2){
		Table result(_tab1.attributes, "set difference of " + _tab1.name + _tab2.name);
		bool match = true;
		bool rowMatch = false;
		int numAtt = _tab1.attributes.size();
		int numAtt2 = _tab2.attributes.size();
		int numRow1 = _tab1.rows.size();
		int numRow2 = _tab2.rows.size();
		vector<vector<string>> RowN = _tab1.rows;
		vector<vector<string>> RowN2 = _tab2.rows;
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
				result.pushBackRow(RowN[r]);
			}
		}
		return result;
	}

	// this function takes in a table, and a list of attributes, it returns a table with the attribute list changed to match the input
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
	vector<Table> createTable(vector<Table> &_tablist, vector<Attribute> _attr, string _name){
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

	 
	//It returns a table containing all rows that satisfy the input condition
	Table select(Table table, string operand1, string operation, string operand2){
		
		Table result(table.getAttributes(), "SelectedFrom" + table.getTableName());

		Attribute attTemp1("", "");
		Attribute attTemp2("", "");

		Attribute* attribute1 = NULL;
		Attribute* attribute2 = NULL;

		string literal = "";

		int att1Index = 0;
		int att2Index = 0;

		for (int i = 0; i < table.getAttributes().size(); i++)
		{
			if (attribute1 == NULL && table.getAttributes()[i].getName() == operand1)
			{
				attTemp1.setName(table.getAttributes()[i].getName());
				attTemp1.setType(table.getAttributes()[i].getType());
				attribute1 = &attTemp1;
				att1Index = i;
			}
			else if (attribute2 == NULL && table.getAttributes()[i].getName() == operand2)
			{
				attTemp2.setName(table.getAttributes()[i].getName());
				attTemp2.setType(table.getAttributes()[i].getType());
				attribute2 = &attTemp1;
				att2Index = i;
			}

			if (attribute1 != NULL && attribute2 != NULL)
			{
				break;
			}
		}

		if (attribute1 == NULL && attribute2 == NULL)
		{
			return result;
		}

		if (attribute2 == NULL)
		{
			literal = operand2;
		}
		else if (attribute1 == NULL)
		{
			literal = operand1;
		}

		for (int i = 0; i < table.getRows().size(); i++)
		{
			if (attribute1 != NULL && attribute2 != NULL)
			{
				if (operation == "==")
				{
					if (table.getRows()[i][att1Index] == table.getRows()[i][att2Index])
					{
						result.pushBackRow(table.getRows()[i]);
					}
				}
				else if (operation == "!=")
				{
					if (table.getRows()[i][att1Index] != table.getRows()[i][att2Index])
					{
						result.pushBackRow(table.getRows()[i]);
					}
				}
				else if (attribute1->getType() == "int" && attribute2->getType() == "int")
				{
					if (operation == "<=")
					{
						if (atoi(table.getRows()[i][att1Index].c_str()) <= atoi(table.getRows()[i][att2Index].c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
					else if (operation == ">=")
					{
						if (atoi(table.getRows()[i][att1Index].c_str()) >= atoi(table.getRows()[i][att2Index].c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
					else if (operation == "<")
					{
						if (atoi(table.getRows()[i][att1Index].c_str()) < atoi(table.getRows()[i][att2Index].c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
					else if (operation == ">")
					{
						if (atoi(table.getRows()[i][att1Index].c_str()) > atoi(table.getRows()[i][att2Index].c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
				}
			}
			else if (attribute1 != NULL)
			{
				if (operation == "==")
				{
					if (table.getRows()[i][att1Index] == literal)
					{
						result.pushBackRow(table.getRows()[i]);
					}
				}
				else if (operation == "!=")
				{
					if (table.getRows()[i][att1Index] != literal)
					{
						result.pushBackRow(table.getRows()[i]);
					}
				}
				else if (attribute1->getType() == "int")
				{
					if (operation == "<=")
					{
						if (atoi(table.getRows()[i][att1Index].c_str()) <= atoi(literal.c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
					else if (operation == ">=")
					{
						if (atoi(table.getRows()[i][att1Index].c_str()) >= atoi(literal.c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
					else if (operation == "<")
					{
						if (atoi(table.getRows()[i][att1Index].c_str()) < atoi(literal.c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
					else if (operation == ">")
					{
						if (atoi(table.getRows()[i][att1Index].c_str()) > atoi(literal.c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
				}
			}
			else if (attribute2 != NULL)
			{
				if (operation == "==")
				{
					if (literal == table.getRows()[i][att2Index])
					{
						result.pushBackRow(table.getRows()[i]);
					}
				}
				else if (operation == "!=")
				{
					if (literal != table.getRows()[i][att2Index])
					{
						result.pushBackRow(table.getRows()[i]);
					}
				}
				else if (attribute2->getType() == "int")
				{
					if (operation == "<=")
					{
						if (atoi(literal.c_str()) <= atoi(table.getRows()[i][att2Index].c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
					else if (operation == ">=")
					{
						if (atoi(literal.c_str()) >= atoi(table.getRows()[i][att2Index].c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
					else if (operation == "<")
					{
						if (atoi(literal.c_str()) < atoi(table.getRows()[i][att2Index].c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
					else if (operation == ">")
					{
						if (atoi(literal.c_str()) > atoi(table.getRows()[i][att2Index].c_str()))
						{
							result.pushBackRow(table.getRows()[i]);
						}
					}
				}
			}
		}

		return result;
	}
	

	//udates a value in a single cell in a table
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

	//inserts one row into a table
	void insertRow(Table &_table, vector <string> addRow)
	{
		_table.pushBackRow(addRow);
	}

	//deletes a row from a table 
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

};
