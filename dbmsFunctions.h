
#include "Table.h"
#include <iostream>

using namespace std;

namespace dbmsFunctions{

	void setUnion(Table& tableA, Table& tableB){
		
		Table result(tableA.attributes);

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



};