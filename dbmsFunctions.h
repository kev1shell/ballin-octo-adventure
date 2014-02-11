
#include "Table.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace dbmsFunctions{

	bool testing = true;


	string int2string(int number) {
		stringstream ss;//create a stringstream
		ss << number;//add number to the stream
		return ss.str();//return a string with the contents of the stream
	}

	
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


	Table naturalJoin(Table& tableA, Table& tableB, Attribute attribute){

		vector<Attribute> attributeList;

		Attribute NJkey("NJID", "int");	//this will be the key of the resulting table

		attributeList.push_back(NJkey);	
		attributeList.push_back(attribute);

		int atrbIndexA = 0;
		int atrbIndexB = 0;

		//add atributes of tableA and tableB to result table
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

	void select(Table table, Attribute attribute, string findthis)
	{
		int i = 0;
		int q = 0;
		for (int j = 0; j<table.attributes.size(); j++)
		{
			if (table.attributes[j].name == attribute.name&&table.attributes[j].type == attribute.type)
			{
				q = j;
			}
		}
		while (i<table.rows.size())
		{

			if (table.rows[i][q] == findthis)
			{
				table.printRow(table.rows[i]);
			}
			i++;
		}
	}

};