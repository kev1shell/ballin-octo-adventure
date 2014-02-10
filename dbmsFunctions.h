
#include "Table.h"
#include <iostream>
#include <sstream>

using namespace std;

namespace dbmsFunctions{

	string int2string(int number) {
		stringstream ss;//create a stringstream
		ss << number;//add number to the stream
		return ss.str();//return a string with the contents of the stream
	}
	
	void setUnion(Table& tableA, Table& tableB){
		
		Table result(tableA.attributes, "setUnion of " + tableA.name + tableB.name);

		Table* longTable;
		Table* shortTable;
		
		//ensure both tables have equal number of attributes
		if (tableA.attributes.size() != tableB.attributes.size()){
			cout << "Set Union Failed: attribute mismatch" << endl;
			return;
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
			return;
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


		

	}

	void naturalJoin(Table& tableA, Table& tableB, Attribute attribute){

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

	}

};