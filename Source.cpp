
#include "dbmsFunctions.h"
#include "Table.h"
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace dbmsFunctions;



//notes for parser_CREATETABLE.
#define identifier (1)
#define number (2)
#define op (3)				// < (when not used as an arrow), >, &, |, +, - (when not used as an arrow), *, !, =
#define punc (4)			// < (when not used as an arrow), comma, (, ), - (when not used as an arrow), ;
#define quotes (5)

//testing booleans
#define testing (false)		//used for testing tokenizer function
#define testingMain (false)	//used for testing main function
#define testingParserCreateTable (false)
#define testingParserInsertInto (false)
#define testingParserDelete (false)
#define testingUpdateValue (true)
#define testingParserProject (false)

using namespace std;


//TABLES STORED IN VECTOR
vector <Table> allTables;

struct Token {
	string content;

	int type;   // type can be: identifier, number, op (!, <, >, etc.), punc, quotes

	Token(){}

	Token(string _content, int _type)
	{
		content = _content;
		type = _type;
	}


};

//*********************************************************************Menu INPUTS********************************************************

int userType = 0;


//*********************************************************************FUNCTION HEADERS***************************************************
vector<Token> tokenizer(int startLoc, string s);

//Commands
void parser_CreateTable(vector <Token> cmd);
void parser_insertInto(vector <Token> cmd);
void parser_Delete(vector <Token> cmd);
void paser_updateValue(vector<Token> cmd);
void parser_Write(Table _table);
void parser_Close(Table _table, int index);
void parser_Open(string _name);
void inputDecoder(vector<string> testStrings);

//add cmd to tableName's commandHistory vector 
void addToCommandHistory(string tableName, string cmd);

//Queries
Table parser_select(vector <Token> cmd);
Table parser_project(vector <Token> cmd);
Table parser_rename(vector <Token> cmd);
Table parser_union(vector <Token> cmd);
Table parser_difference(vector <Token> cmd);
Table parser_product(vector <Token> cmd);
Table parser_natural_join(vector <Token> cmd);

//atomic expression resolver
Table atomicExpResolver(vector<Token> tokenList);

//menu functions
void customerMenu();
void manufacturerMenu();
void welcome();
void logIn();
void registerUser();

int main()
{	
	welcome();
	return 0;
}
//Menu Functions
void welcome(){
	cout << "welcome to Bazaar 1.0" << endl;
	cout << "Press 1 to access our menus as a customer" << endl;
	cout << "press 2 to access our menus as a manufacturer" << endl;
	cin >> userType;
	if (userType == 1){
		customerMenu();
	}
	else if (userType == 2){
		manufacturerMenu();
	}
}

void customerMenu(){
	int custMen = 0;
	cout << "welcome customer" << endl;
	cout << "press 1 to log in as an existing user" << endl;
	cout << "press 2 to register as a new user" << endl;
	cout << "press 3 to return to main menu" << endl;
	cin >> custMen;
	if (custMen == 1){
		logIn();
	}
	else if (custMen == 2){
		registerUser();
	}
	else if (custMen == 3){
		welcome();
	}
	else {
		cout << "invalid input" << endl;
		welcome();
	}
}
void logIn(){
	cout << "log in" << endl;
}
void registerUser(){
	cout << "register user" << endl;
}

void manufacturerMenu(){
	cout << "Press 1 for inventory Menu Access"
}



//Parser Functions
void inputDecoder(vector<string> testStrings){
	for (int iter = 0; iter < testStrings.size(); iter++)
	{

		//if the input is a query (idk how to spell tht) it includes a "<-" in it.
		size_t functionType = testStrings[iter].find("<-");
		if (functionType != string::npos)
		{
			//if "<-" is found go thru query calls (e.g. select, project, etc.)
			vector <Token> _tokenList = tokenizer(0, testStrings[iter]);
			string viewName = _tokenList[0].content;
			string _query = testStrings[iter].substr(functionType + 2);
			if (testStrings[iter].substr(functionType + 3, 6) == "select")
			{
				vector <Token> select;
				cout << "SELECT" << endl;
				select = tokenizer(functionType + 9, testStrings[iter]);
				if (testingMain)
				{
					for (int x = 0; x < select.size(); x++)
					{
						cout << select[x].content;
					}
					cout << endl;
				}
				Table result = parser_select(select);
				result.printTable(result);
				
				//add the new view into all tables
				Table newView = result;
				newView.setName(viewName);
				allTables.push_back(newView);
			}
			else if (testStrings[iter].substr(functionType + 3, 6) == "rename")
			{
				string tablename = testStrings[iter].substr(0, functionType - 1);
				vector <Token> rename;
				rename = tokenizer(functionType + 9, testStrings[iter]);
				if (testingMain)
				{
					cout << testStrings[iter].substr(functionType + 3, 6) << endl;
					for (int x = 0; x < rename.size(); x++)
					{
						cout << rename[x].content << " " << rename[x].type << endl << endl;
					}
					cout << endl;
				}
				Table addthistable = parser_rename(rename);
				addthistable.setName(tablename);
				allTables.push_back(addthistable);
				cout << "Rename call created the following table stored in " << tablename << "." << endl;
				allTables[allTables.size() - 1].printTable(allTables[allTables.size() - 1]);
			}
			else if (testStrings[iter].substr(functionType + 3, 7) == "project")
			{
				cout << "Calling project: " << endl;
				vector <Token> project;
				project = tokenizer(0, testStrings[iter]);

				if (testingMain)
				{
					for (int x = 0; x < project.size(); x++)
					{
						cout << project[x].content;
					}
					cout << endl;
				}
				Table newtable = parser_project(project);
				newtable.printTable(newtable);
				
				//add the new view into all tables
				Table newView = newtable;
				newView.setName(viewName);
				allTables.push_back(newView);
			}
			else if (_query.find("*") != string::npos || _query.find("+") != string::npos ||
				_query.find("-") != string::npos || _query.find("JOIN") != string::npos)
			{
				string tablename = testStrings[iter].substr(0, functionType - 1);
				vector <Token> crossprod;
				crossprod = tokenizer(functionType + 3, testStrings[iter]);

				if (crossprod.size() > 0 && crossprod[crossprod.size() - 1].content == ";")
				{
					crossprod.erase(crossprod.begin() + crossprod.size() - 1);
				}

				if (testingMain)
				{
					size_t loc = testStrings[iter].find("*");
					cout << "Cross Product: " << testStrings[iter].substr(loc,1)<< "store in: " << tablename << endl;
					for (int x = 0; x < crossprod.size(); x++)
					{
						cout << crossprod[x].content << " " << crossprod[x].type << endl << endl;
					}
				}
				cout << endl;
				Table addThisTable = atomicExpResolver(crossprod);
				addThisTable.setName(tablename);
				allTables.push_back(addThisTable);
				cout << "Cross product call produced the following table: " << endl;
				allTables[allTables.size() - 1].printTable(allTables[allTables.size() - 1]);
			}
			else
			{
				//inputLine must be a tablename

				vector <Token> tokenList = tokenizer(functionType + 3, testStrings[iter]);

				if (tokenList.size() > 0 && tokenList[tokenList.size() - 1].content == ";")
				{
					tokenList.erase(tokenList.begin() + tokenList.size() - 1);
				}

				Table result = atomicExpResolver(tokenList);

				//add the new view into all tables
				Table newView = result;
				newView.setName(viewName);
				allTables.push_back(newView);
			}
		}
		else
		{
			//else go thru commands (e.g. "CREATE TABLE", "SHOW", etc.)
			if (testStrings[iter].substr(0, 12) == "CREATE TABLE")
			{
				if (testingMain)
				{
					cout << testStrings[iter].substr(0, 12) << endl;
				}

				vector <Token> createTable = tokenizer(13, testStrings[iter]);

				if (testingMain)
				{
					for (int x = 0; x < createTable.size(); x++)
					{
						cout << createTable[x].content << " " << createTable[x].type << endl << endl;
					}
					cout << endl;
				}
				//after parser called the following is sotred in a vector "animals(nameVARCHAR(20),kindVARCHAR(8),yearsINTEGER)PRIMARYKEY(name,kind);"
				//call parserCreateTable("animals (name VARCHAR(10));") in parserFunctions namespace

				parser_CreateTable(createTable);
				addToCommandHistory(createTable[0].content, testStrings[iter]);

			}
			else if (testStrings[iter].substr(0, 4) == "SHOW")
			{
				vector <Token> showTable = tokenizer(5, testStrings[iter]);

				if (testingMain)
				{
					for (int x = 0; x < showTable.size(); x++)
					{
						cout << "| " << showTable[x].content << ", " << showTable[x].type << " |";
					}
					cout << endl;
				}

				if (showTable.size() > 2){

					vector<Token> newTokenList;

					for (int i = 0; i < showTable.size() - 1; i++)
					{
						newTokenList.push_back(showTable[i]);
					}

					Table result = atomicExpResolver(newTokenList);
					result.printTable(result);

				}
				else
				{
					for (int i = 0; i < allTables.size(); i++)
					{
						if (allTables[i].getTableName() == showTable[0].content)
						{
							allTables[i].printTable(allTables[i]);
						}
					}
				}
			}
			else if (testStrings[iter].substr(0, 11) == "INSERT INTO")
			{
				cout << testStrings[iter].substr(0, 11) << endl;

				vector <Token> insertINTO = tokenizer(12, testStrings[iter]);

				if (testingMain)
				{
					for (int x = 0; x < insertINTO.size(); x++)
					{
						cout << insertINTO[x].content << " " << insertINTO[x].type << endl << endl;
					}
					cout << endl;
				}

				parser_insertInto(insertINTO);
				addToCommandHistory(insertINTO[0].content, testStrings[iter]);
			}

			else if (testStrings[iter].substr(0, 11) == "DELETE FROM")
			{
				cout << testStrings[iter].substr(0, 11) << endl;

				vector <Token> DeleteThis = tokenizer(12, testStrings[iter]);

				if (testingMain)
				{
					for (int x = 0; x < DeleteThis.size(); x++)
					{
						cout << DeleteThis[x].content << " " << DeleteThis[x].type << endl << endl;
					}
					cout << endl;
				}

				parser_Delete(DeleteThis);
				addToCommandHistory(DeleteThis[0].content, testStrings[iter]);
			}
			else if (testStrings[iter].substr(0, 5) == "WRITE"){
				string table_name;
				int table1_index;
				vector <Token> WriteThis = tokenizer(6, testStrings[iter]);
				cout << "write this: " << WriteThis[0].content << endl;
				table_name = WriteThis[0].content;
				bool isTable = false;
				for (int i = 0; i < allTables.size(); i++)
				{
					if (allTables[i].getTableName() == table_name)
					{
						//token is a table
						table1_index = i;
						isTable = true;
						break;
					}
				}
				if (isTable == true){
					parser_Write(allTables[table1_index]);
				}

			}
			else if (testStrings[iter].substr(0, 5) == "CLOSE"){
				string table_name;
				int table1_index;
				vector <Token> CloseThis = tokenizer(6, testStrings[iter]);
				cout << "close this: " << CloseThis[0].content << endl;
				table_name = CloseThis[0].content;
				bool isTable = false;
				for (int i = 0; i < allTables.size(); i++)
				{
					if (allTables[i].getTableName() == table_name)
					{
						//token is a table
						table1_index = i;
						isTable = true;
						break;
					}
				}
				if (isTable == true){
					parser_Close(allTables[table1_index], table1_index);
				}

			}
			else if (testStrings[iter].substr(0, 4) == "OPEN"){
				string table_name;
				int table1_index;
				vector <Token> OpenThis = tokenizer(5, testStrings[iter]);
				cout << "open this: " << OpenThis[0].content << endl;
				table_name = OpenThis[0].content;
				parser_Open(table_name);
			}
			else if (testStrings[iter].substr(0, 4) == "EXIT"){
				//THis is a library of functions... Don't do anything
			}
			else if (testStrings[iter].substr(0, 6) == "UPDATE")
			{
				cout << testStrings[iter].substr(0, 6) << endl;
				vector <Token> updateThis = tokenizer(7, testStrings[iter]);
				if (testingMain)
				{
					for (int x = 0; x < updateThis.size(); x++)
					{
						cout << updateThis[x].content << " " << updateThis[x].type << endl << endl;
					}
					cout << endl;
				}
				paser_updateValue(updateThis);
			}
		}

	}
}

//Turns string input s into tokens starting from a specified position in the string (startLoc); Returns a vector of the tokens
vector<Token> tokenizer(int startLoc, string s)
{
	int p = startLoc;
	vector <Token> result;
	while (p < s.length())
	{
		if (isalpha(s.at(p)))
		{
			// read until non-alpha-numeric
			int q = p;
			// cout << s.at(21) <<endl;
			while (isalnum(s.at(q)) > 0)
			{
				// cout << s.at(q);
				q++;
			}
			Token t(s.substr(p, q - p), identifier);
			result.push_back(t);
			p = q;
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (isdigit(s[p]))
		{
			// read until non-numeric
			int q = p;
			while (isdigit(s[q]))
			{
				q++;

			}
			Token t(s.substr(p, q - p), number);
			result.push_back(t);
			p = q;		// p += num_length;
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == ' ')
		{
			int q = p;
			p++;
		}
		else if (s[p] == '(')
		{
			// create token('(')
			p++;
			Token t("(", punc);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == ')')
		{
			p++;
			Token t(")", punc);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == ',')
		{
			p++;
			Token t(",", punc);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == '>')
		{
			p++;
			Token t(">", op);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == '<')
		{
			p++;
			if (s[p + 1] != '-')
			{
				Token t("<", op);
				result.push_back(t);
			}
			else
			{
				Token t("<", punc);
				result.push_back(t);
			}
			if (testing)
			{
				cout << "Content: " << result[result.size()-1].content << " type: " << result[result.size()-1].type << endl;
			}
		}
		else if (s[p] == '!')
		{
			p++;
			Token t("!", op);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == '&')
		{
			p++;
			Token t("&", op);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == '|')
		{
			p++;
			Token t("|", op);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == '=')
		{
			p++;
			Token t("=", op);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == '*')
		{
			p++;
			Token t("*", op);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == '+')
		{
			p++;
			Token t("+", op);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == '"')
		{
			p++;
			Token t("\"", quotes);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
		}
		else if (s[p] == '-')
		{
			p++;

			if (s[p - 1] == '<')
			{
				Token t("-", punc);
				result.push_back(t);
			}
			else
			{
				Token t("-", op);
				result.push_back(t);
			}

			if (testing)
			{
				cout << "Content: " << result[result.size() - 1].content << " type: " << result[result.size() - 1].type << endl;
			}
		}
		else if (s[p] == ';')
		{
			Token t(";", punc);
			result.push_back(t);
			if (testing)
			{
				cout << "Content: " << t.content << " type: " << t.type << endl;
			}
			//exit
			return result;
		}

	}
}

void parser_CreateTable(vector<Token> cmd)
{
	string _name;							//stores name that will be passed to the dbms create table
	vector <Attribute> attrs;				//stores attributes that will be passed to dbms the create 
	int positionInInput;
	vector <Token> attributes;				//stores attributes (in test string input: (name VARCHAR(20), kind VARCHAR(8), years INTEGER))
	if (cmd[0].type == 1)
	{
		 _name = cmd[0].content;
	}
	else
	{
		cout << "ERROR: Create Table format incorrect. First word after Create Table should be name of table" << endl;
		return;
	}

	if (cmd[1].type == punc)
	{
		//parse thru for equal number of open and closed parenthesis. store content of middle as vector of tokens and start working with tht
		//for loop thru vector of tokens while storing in attribute list
		int openParenthesisCount = 0;
		int closedParenthesisCount = 0;
		if (cmd[1].content == "(")
		{
			Token t("(", punc);
			attributes.push_back(t);
			openParenthesisCount++;
			for (int x = 2; x < cmd.size(); x++)
			{
				if (cmd[x].content == ")")
					closedParenthesisCount++;
				else if (cmd[x].content == "(")
					openParenthesisCount++;
				
				attributes.push_back(cmd[x]);

				if (openParenthesisCount == closedParenthesisCount)
				{
					//update int position in input and use for next if/else clause with primary keys
					positionInInput = x+1;
					break;
				}
			}

			if (testingParserCreateTable)
			{
				cout << "Attributes vector <Tokens> = ";
				for (int x = 0; x < attributes.size(); x++)
				{
					cout << attributes[x].content;
				}
				cout << endl;
			}

			//if no formatting error, go through newly created vector <Token> that should now contain the attribute list. and store attributes in vector <Attribute> attrs
			if (openParenthesisCount != closedParenthesisCount)
			{
				cout << "ERROR: Create Table format incorrect. Check parenthesis" << endl;
				return;
			}
			else
			{
				for (int x = 1; x < attributes.size(); x++)
				{
					if (cmd[x].type == identifier)
					{
						if (cmd[x + 1].type == identifier)
						{
							if (cmd[x + 1].content == "VARCHAR")
							{
								Attribute a(cmd[x].content, "string");
								attrs.push_back(a);
								int y = x + 1;
								while (cmd[y].type != punc && cmd[y].content != ")")
								{
									y++;
								}
								x = y;
							}
							else if (cmd[x + 1].content == "INTEGER")
							{
								Attribute a(cmd[x].content, "int");
								attrs.push_back(a);
								x++;
							}
							else if (cmd[x + 1].content == "CHAR")
							{
								Attribute a(cmd[x].content, "char");
								attrs.push_back(a);
								x++;
							}
							else if (cmd[x + 1].content == "DOUBLE")
							{
								Attribute a(cmd[x].content, "double");
								attrs.push_back(a);
								x++;
							}
							else
							{
								cout << "ERROR: Create Table format incorrect. Expecting type name after identifier in attribute list." << endl;
								return;
							}
						}
						else
						{
							cout << "ERROR: Create Table format incorrect. Check attribute list entered." << endl;
							return;
						}
					}
					//dont do anything if they are commas
				}

				if (testingParserCreateTable)
				{
					for (int x = 0; x < attrs.size(); x++)
					{
						cout << attrs[x].getName() << "     " << endl;
					}
				}
			}
		}
		else
		{
			cout << "ERROR: Create Table format incorrect. Expecting open parenthesis after table name" << endl;
			return;
		}
	}
	else
	{
		cout << "ERROR: Create Table format incorrect. Missing open parenthesis. Attributes list." << endl;
		return;
	}

	//Go through last part of vector <Token> cmd passed in and identify primary keys. After identifying set primary keys in vector<Attribute> attr
	if (cmd[positionInInput].type == identifier && cmd[positionInInput].content == "PRIMARY" && cmd[positionInInput+1].content == "KEY")
	{
		positionInInput = positionInInput + 2;
		if (cmd[positionInInput].type != punc && cmd[positionInInput].content != "(")
		{
			cout << "ERROR: Create Table format incorrect. Expecting \"(\" after \"PRIMARY KEY\"" << endl;
			return;
		}
		else
		{
			positionInInput++;
			while (positionInInput < cmd.size())
			{
				if (cmd[positionInInput].type == identifier)
				{
					string pk = cmd[positionInInput].content;
					for (int x = 0; x < attrs.size(); x++)
					{
						if (attrs[x].getName() == cmd[positionInInput].content)
						{
							attrs[x].setPrimaryKey();
						}
					}
					positionInInput++;
				}
				else if (cmd[positionInInput].content == ";")
				{
					break;
				}
				else if (cmd[positionInInput].type == punc)
				{
					positionInInput++;
				}
				else
				{
					cout << "ERROR: Create Table format incorrect. Check Primary Key part of expression." << endl;
					return;
				}
			}
		}
	}
	else
	{
		cout << "ERROR: Create Table format incorrect. Primary Keys not identified. Even if table doesn't include Primary Keys put PRIMARY KEY() in input line";
		return;
	}
	createTable(allTables, attrs, _name);
	cout << "The following table has been created:" << endl;
	allTables[allTables.size() - 1].printTable(allTables[allTables.size() - 1]);
}

Table atomicExpResolver(vector<Token> tokenList)
{
	int table1_index, table2_index;

	Table* table1 = NULL;
	Table* table2 = NULL;

	Attribute dummy("dummy", "string");
	vector<Attribute> attributeList = { dummy };
	Table tableA(attributeList, "tableA");
	Table tableB(attributeList, "tableB");

	string opperator = "";

	for (int currentToken = 0; currentToken < tokenList.size(); currentToken++)
	{
		if (tokenList[0].type == identifier)
		{
			bool isTable = false;

			for (int i = 0; i < allTables.size(); i++)
			{
				if (allTables[i].getTableName() == tokenList[currentToken].content)
				{
					//token is a table
					table1_index = i;
					table1 = &allTables[i];
					isTable = true;
					break;
				}
			}

			if (isTable)
			{
				if (currentToken + 2 < tokenList.size())
				{
					if (tokenList[currentToken + 1].type == op)
					{
						bool isTable2 = false;

						for (int i = 0; i < allTables.size(); i++)
						{
							if (allTables[i].getTableName() == tokenList[currentToken + 2].content)
							{
								//token is a table
								table2_index = i;
								isTable2 = true;
								break;
							}
						}

						if (isTable2)
						{
							//compute opperation and return table
							if (tokenList[currentToken + 1].content == "+")
							{
								//compute union of currentToken and currentToken + 2
								Table result = setUnion(allTables[table1_index], allTables[table2_index]);
								return result;
							}
							else if (tokenList[currentToken + 1].content == "-")
							{
								//compute difference of currentToken and currentToken + 2
								Table result = setDifference(allTables[table1_index], allTables[table2_index]);
								return result;
							}
							else if (tokenList[currentToken + 1].content == "*")
							{
								//compute cross product of currentToken and currentToken + 2
								Table result = crossProduct(allTables[table1_index], allTables[table2_index]);
								return result;

							}
						}
						else if (tokenList[currentToken + 2].content == "(")
						{
							//find corresponding closed parenthesis
							int numOpenParens = 1;
							int numClosedParens = 0;
							int lastClosedParenIndex = 0;
							for (int k = currentToken + 1; k < tokenList.size(); k++)
							{
								if (tokenList[k].content == "(")
								{
									numOpenParens++;
								}
								else if (tokenList[k].content == ")")
								{
									numClosedParens++;
									if (numClosedParens == numOpenParens){
										lastClosedParenIndex = k;
										break;
									}
								}
							}
							if (numClosedParens != numOpenParens)
							{
								//ERROR not enough parenthesis
								cout << "ERROR mismatched parenthesis" << endl;
								Attribute ERROR("ERROR", "string");
								vector<Attribute> attributeList = { ERROR };
								Table result(attributeList, "ERROR");
								return result;
							}
							else
							{
								//create new vector of tokens with tokens enclosed by outermost parens
								vector<Token> newTokenList;
								for (int k = currentToken + 1; k < lastClosedParenIndex; k++)
								{
									newTokenList.push_back(tokenList[k]);
								}
								tableB = atomicExpResolver(newTokenList);
								table2 = &tableB;
								currentToken = lastClosedParenIndex;
								if (table1 != NULL && table2 != NULL)
								{
									//compute opperation and return table
									Table tableA = *table1;
									Table tableB = *table2;

									if (tokenList[currentToken + 1].content == "+")
									{
										//compute union of currentToken and currentToken + 2
										Table result = setUnion(tableA, tableB);
										return result;
									}
									else if (tokenList[currentToken + 1].content == "-")
									{
										//compute difference of currentToken and currentToken + 2
										Table result = setDifference(tableA, tableB);
										return result;
									}
									else if (tokenList[currentToken + 1].content == "*")
									{
										//compute cross product of currentToken and currentToken + 2
										Table result = crossProduct(tableA, tableB);
										return result;

									}
								}
							}
						}
						else if (tokenList[currentToken + 2].content == "select") //if first token is an identifier but not a table;
						{
							int startIndex = currentToken + 1;
							int endIndex = 0;
							//find corresponding closed parenthesis
							int numOpenParens = 0;
							int numClosedParens = 0;
							int lastClosedParenIndex = 0;
							for (int k = currentToken + 2; k < tokenList.size(); k++)
							{
								if (tokenList[k].content == "(")
								{
									numOpenParens++;
								}
								else if (tokenList[k].content == ")")
								{
									numClosedParens++;
									if (numClosedParens == numOpenParens){
										lastClosedParenIndex = k;
										break;
									}
								}
							}
							if (numClosedParens != numOpenParens)
							{
								//ERROR not enough parenthesis
								cout << "ERROR mismatched parenthesis" << endl;
								Attribute ERROR("ERROR", "string");
								vector<Attribute> attributeList = { ERROR };
								Table result(attributeList, "ERROR");
								return result;
							}
							currentToken = lastClosedParenIndex + 1;

							if (tokenList[currentToken + 2].type == identifier)
							{
								//token is a table name
								endIndex = currentToken;
							}
							else
							{
								if (tokenList[currentToken + 2].content == "(")
								{
									//find corresponding closed parenthesis
									int numOpenParens = 0;
									int numClosedParens = 0;
									int lastClosedParenIndex = 0;
									for (int k = currentToken + 2; k < tokenList.size(); k++)
									{
										if (tokenList[k].content == "(")
										{
											numOpenParens++;
										}
										else if (tokenList[k].content == ")")
										{
											numClosedParens++;
											if (numClosedParens == numOpenParens){
												lastClosedParenIndex = k;
												break;
											}
										}
									}
									if (numClosedParens != numOpenParens)
									{
										//ERROR not enough parenthesis
										cout << "ERROR mismatched parenthesis" << endl;
										Attribute ERROR("ERROR", "string");
										vector<Attribute> attributeList = { ERROR };
										Table result(attributeList, "ERROR");
										return result;
									}
									currentToken = lastClosedParenIndex;
									endIndex = lastClosedParenIndex;
								}
								else
								{
									//ERROR incorrect syntax
									cout << "ERROR incorrect syntax" << endl;
									Attribute ERROR("ERROR", "string");
									vector<Attribute> attributeList = { ERROR };
									Table result(attributeList, "ERROR");
									return result;
								}
							}
							vector<Token> newTokenList;
							for (int k = startIndex; k <= endIndex && k < tokenList.size(); k++)
							{
								newTokenList.push_back(tokenList[k]);
							}
							tableB = parser_select(newTokenList);
							table2 = &tableB;

						}
						else if (tokenList[currentToken + 2].content == "project")
						{
							vector<Token> newTokenList;
							for (int k = 1; k < tokenList.size(); k++)
							{
								newTokenList.push_back(tokenList[k]);
							}
							table2 = &parser_project(newTokenList);
							//return result;

						}
						else if (tokenList[currentToken + 2].content == "rename")
						{
							vector<Token> newTokenList;
							for (int k = 1; k < tokenList.size(); k++)
							{
								newTokenList.push_back(tokenList[k]);
							}
							table2 = &parser_rename(newTokenList);
							//return result;
						}
						else {
							//ERROR incorrect syntax
							cout << "ERROR incorrect syntax" << endl;
							Attribute ERROR("ERROR", "string");
							vector<Attribute> attributeList = { ERROR };
							Table result(attributeList, "ERROR");
							return result;
						}
						if (table1 != NULL && table2 != NULL)
						{
							//compute opperation and return table
							Table tableA = *table1;
							Table tableB = *table2;

							if (tokenList[currentToken + 1].content == "+")
							{
								//compute union of currentToken and currentToken + 2
								Table result = setUnion(tableA, tableB);
								return result;
							}
							else if (tokenList[currentToken + 1].content == "-")
							{
								//compute difference of currentToken and currentToken + 2
								Table result = setDifference(tableA, tableB);
								return result;
							}
							else if (tokenList[currentToken + 1].content == "*")
							{
								//compute cross product of currentToken and currentToken + 2
								Table result = crossProduct(tableA, tableB);
								return result;

							}
						}
					}
					else if (tokenList[currentToken + 1].content == "JOIN") //is the next token "JOIN"?
					{
						bool isTable = false;

						for (int i = 0; i < allTables.size(); i++)
						{
							if (allTables[i].getTableName() == tokenList[currentToken + 2].content)
							{
								//token is a table
								table2_index = i;
								table2 = &allTables[i];
								isTable = true;
								break;
							}
						}

						if (isTable)
						{
							//compute natural join
							if (table1 != NULL && table2 != NULL)
							{
								Table tableA = *table1;
								Table tableB = *table2;

								string name = "";
								string type = "";
								string keyType = "";

								for (int x = 0; x < tableA.getAttributes().size(); x++)
								{
									for (int y = 0; y < tableB.getAttributes().size(); y++)
									{
										if (tableA.getAttributes()[x].getName() == tableB.getAttributes()[y].getName() && tableA.getAttributes()[x].getType() == tableB.getAttributes()[y].getType())
										{
											name = tableA.getAttributes()[x].getName();
											type = tableA.getAttributes()[x].getType();
											keyType = tableA.getAttributes()[x].getKeyType();
											x = tableA.getAttributes().size();
											y = tableB.getAttributes().size();
										}
									}
								}
								if (name != "" && type != "")
								{
									Attribute attribute(name, type, keyType);
									Table result = naturalJoin(tableA, tableB, attribute);
									return result;
								}
							}
						}
						else if (tokenList[currentToken + 2].content == "(")
						{
							//find corresponding closed parenthesis
							int numOpenParens = 1;
							int numClosedParens = 0;
							int lastClosedParenIndex = 0;
							for (int k = currentToken + 1; k < tokenList.size(); k++)
							{
								if (tokenList[k].content == "(")
								{
									numOpenParens++;
								}
								else if (tokenList[k].content == ")")
								{
									numClosedParens++;
									if (numClosedParens == numOpenParens){
										lastClosedParenIndex = k;
										break;
									}
								}
							}
							if (numClosedParens != numOpenParens)
							{
								//ERROR not enough parenthesis
								cout << "ERROR mismatched parenthesis" << endl;
								Attribute ERROR("ERROR", "string");
								vector<Attribute> attributeList = { ERROR };
								Table result(attributeList, "ERROR");
								return result;
							}
							else
							{
								//create new vector of tokens with tokens enclosed by outermost parens
								vector<Token> newTokenList;
								for (int k = currentToken + 1; k < lastClosedParenIndex; k++)
								{
									newTokenList.push_back(tokenList[k]);
								}
								tableB = atomicExpResolver(newTokenList);
								table2 = &tableB;
								currentToken = lastClosedParenIndex;

								if (table1 != NULL && table2 != NULL)
								{
									//compute natural join and return table
									Table tableA = *table1;
									Table tableB = *table2;

									string name = "";
									string type = "";
									string keyType = "";

									for (int x = 0; x < tableA.getAttributes().size(); x++)
									{
										for (int y = 0; y < tableB.getAttributes().size(); y++)
										{
											if (tableA.getAttributes()[x].getName() == tableB.getAttributes()[y].getName() && tableA.getAttributes()[x].getType() == tableB.getAttributes()[y].getType())
											{
												name = tableA.getAttributes()[x].getName();
												type = tableA.getAttributes()[x].getType();
												keyType = tableA.getAttributes()[x].getKeyType();
												x = tableA.getAttributes().size();
												y = tableB.getAttributes().size();
											}
										}
									}
									if (name != "" && type != "")
									{
										Attribute attribute(name, type, keyType);
										Table result = naturalJoin(tableA, tableB, attribute);
										return result;
									}
								}
							}
						}
					}
				}
				else
				{
					//this was the only token provided to the function
					Table result = allTables[table1_index];
					return result;
				}
			}
			else if (tokenList[currentToken].content == "select") //if first token is an identifier but not a table;
			{
				int startIndex = currentToken + 1;
				int endIndex = 0;
				//find corresponding closed parenthesis
				int numOpenParens = 0;
				int numClosedParens = 0;
				int lastClosedParenIndex = 0;
				for (int k = currentToken + 1; k < tokenList.size(); k++)
				{
					if (tokenList[k].content == "(")
					{
						numOpenParens++;
					}
					else if (tokenList[k].content == ")")
					{
						numClosedParens++;
						if (numClosedParens == numOpenParens){
							lastClosedParenIndex = k;
							break;
						}
					}
				}
				if (numClosedParens != numOpenParens)
				{
					//ERROR not enough parenthesis
					cout << "ERROR mismatched parenthesis" << endl;
					Attribute ERROR("ERROR", "string");
					vector<Attribute> attributeList = { ERROR };
					Table result(attributeList, "ERROR");
					return result;
				}
				currentToken = lastClosedParenIndex + 1;

				if (tokenList[currentToken].type == identifier)
				{
					//token is a table name
					endIndex = currentToken;
				}
				else
				{
					if (tokenList[currentToken].content == "(")
					{
						//find corresponding closed parenthesis
						int numOpenParens = 0;
						int numClosedParens = 0;
						int lastClosedParenIndex = 0;
						for (int k = currentToken + 1; k < tokenList.size(); k++)
						{
							if (tokenList[k].content == "(")
							{
								numOpenParens++;
							}
							else if (tokenList[k].content == ")")
							{
								numClosedParens++;
								if (numClosedParens == numOpenParens){
									lastClosedParenIndex = k;
									break;
								}
							}
						}
						if (numClosedParens != numOpenParens)
						{
							//ERROR not enough parenthesis
							cout << "ERROR mismatched parenthesis" << endl;
							Attribute ERROR("ERROR", "string");
							vector<Attribute> attributeList = { ERROR };
							Table result(attributeList, "ERROR");
							return result;
						}
						currentToken = lastClosedParenIndex;
						endIndex = lastClosedParenIndex;
					}
					else
					{
						//ERROR incorrect syntax
						cout << "ERROR incorrect syntax" << endl;
						Attribute ERROR("ERROR", "string");
						vector<Attribute> attributeList = { ERROR };
						Table result(attributeList, "ERROR");
						return result;
					}
				}
				vector<Token> newTokenList;
				for (int k = startIndex; k <= endIndex && k < tokenList.size(); k++)
				{
					newTokenList.push_back(tokenList[k]);
				}
				tableA = parser_select(newTokenList);
				table1 = &tableA;

			}
			else if (tokenList[currentToken].content == "project")
			{
				vector<Token> newTokenList;
				for (int k = 1; k < tokenList.size(); k++)
				{
					newTokenList.push_back(tokenList[k]);
				}
				tableA = parser_project(newTokenList);
				table1 = &tableA;

			}
			else if (tokenList[currentToken].content == "rename")
			{
				vector<Token> newTokenList;
				for (int k = 1; k < tokenList.size(); k++)
				{
					newTokenList.push_back(tokenList[k]);
				}
				tableA = parser_rename(newTokenList);
				table1 = &tableA;
			}
			else {
				//ERROR incorrect syntax
				cout << "ERROR incorrect syntax" << endl;
				Attribute ERROR("ERROR", "string");
				vector<Attribute> attributeList = { ERROR };
				Table result(attributeList, "ERROR");
				return result;
			}
			if (currentToken + 2 < tokenList.size())
			{
				if (tokenList[currentToken + 1].type == op)
				{
					bool isTable = false;

					for (int i = 0; i < allTables.size(); i++)
					{
						if (allTables[i].getTableName() == tokenList[currentToken + 2].content)
						{
							//token is a table
							table2_index = i;
							tableB = allTables[i];
							isTable = true;
							break;
						}
					}

					if (isTable)
					{
						//compute opperation and return table
						if (tokenList[currentToken + 1].content == "+")
						{
							//compute union of currentToken and currentToken + 2
							Table result = setUnion(tableA, tableB);
							return result;
						}
						else if (tokenList[currentToken + 1].content == "-")
						{
							//compute difference of currentToken and currentToken + 2
							Table result = setDifference(tableA, tableB);
							return result;
						}
						else if (tokenList[currentToken + 1].content == "*")
						{
							//compute cross product of currentToken and currentToken + 2
							Table result = crossProduct(tableA, tableB);
							return result;

						}
					}
					else if (tokenList[currentToken + 2].content == "(")
					{
						//find corresponding closed parenthesis
						int numOpenParens = 1;
						int numClosedParens = 0;
						int lastClosedParenIndex = 0;
						for (int k = currentToken + 1; k < tokenList.size(); k++)
						{
							if (tokenList[k].content == "(")
							{
								numOpenParens++;
							}
							else if (tokenList[k].content == ")")
							{
								numClosedParens++;
								if (numClosedParens == numOpenParens){
									lastClosedParenIndex = k;
									break;
								}
							}
						}
						if (numClosedParens != numOpenParens)
						{
							//ERROR not enough parenthesis
							cout << "ERROR mismatched parenthesis" << endl;
							Attribute ERROR("ERROR", "string");
							vector<Attribute> attributeList = { ERROR };
							Table result(attributeList, "ERROR");
							return result;
						}
						else
						{
							//create new vector of tokens with tokens enclosed by outermost parens
							vector<Token> newTokenList;
							for (int k = currentToken + 1; k < lastClosedParenIndex; k++)
							{
								newTokenList.push_back(tokenList[k]);
							}
							tableB = atomicExpResolver(newTokenList);
							table2 = &tableB;
							currentToken = lastClosedParenIndex;
							if (table1 != NULL && table2 != NULL)
							{
								//compute opperation and return table
								Table tableA = *table1;
								Table tableB = *table2;

								if (tokenList[currentToken + 1].content == "+")
								{
									//compute union of currentToken and currentToken + 2
									Table result = setUnion(tableA, tableB);
									return result;
								}
								else if (tokenList[currentToken + 1].content == "-")
								{
									//compute difference of currentToken and currentToken + 2
									Table result = setDifference(tableA, tableB);
									return result;
								}
								else if (tokenList[currentToken + 1].content == "*")
								{
									//compute cross product of currentToken and currentToken + 2
									Table result = crossProduct(tableA, tableB);
									return result;

								}
							}
						}
					}
					else if (tokenList[currentToken + 2].content == "select") //if first token is an identifier but not a table;
					{
						int startIndex = currentToken + 1;
						int endIndex = 0;
						//find corresponding closed parenthesis
						int numOpenParens = 0;
						int numClosedParens = 0;
						int lastClosedParenIndex = 0;
						for (int k = currentToken + 2; k < tokenList.size(); k++)
						{
							if (tokenList[k].content == "(")
							{
								numOpenParens++;
							}
							else if (tokenList[k].content == ")")
							{
								numClosedParens++;
								if (numClosedParens == numOpenParens){
									lastClosedParenIndex = k;
									break;
								}
							}
						}
						if (numClosedParens != numOpenParens)
						{
							//ERROR not enough parenthesis
							cout << "ERROR mismatched parenthesis" << endl;
							Attribute ERROR("ERROR", "string");
							vector<Attribute> attributeList = { ERROR };
							Table result(attributeList, "ERROR");
							return result;
						}
						currentToken = lastClosedParenIndex + 1;

						if (tokenList[currentToken + 2].type == identifier)
						{
							//token is a table name
							endIndex = currentToken;
						}
						else
						{
							if (tokenList[currentToken + 2].content == "(")
							{
								//find corresponding closed parenthesis
								int numOpenParens = 0;
								int numClosedParens = 0;
								int lastClosedParenIndex = 0;
								for (int k = currentToken + 2; k < tokenList.size(); k++)
								{
									if (tokenList[k].content == "(")
									{
										numOpenParens++;
									}
									else if (tokenList[k].content == ")")
									{
										numClosedParens++;
										if (numClosedParens == numOpenParens){
											lastClosedParenIndex = k;
											break;
										}
									}
								}
								if (numClosedParens != numOpenParens)
								{
									//ERROR not enough parenthesis
									cout << "ERROR mismatched parenthesis" << endl;
									Attribute ERROR("ERROR", "string");
									vector<Attribute> attributeList = { ERROR };
									Table result(attributeList, "ERROR");
									return result;
								}
								currentToken = lastClosedParenIndex;
								endIndex = lastClosedParenIndex;
							}
							else
							{
								//ERROR incorrect syntax
								cout << "ERROR incorrect syntax" << endl;
								Attribute ERROR("ERROR", "string");
								vector<Attribute> attributeList = { ERROR };
								Table result(attributeList, "ERROR");
								return result;
							}
						}
						vector<Token> newTokenList;
						for (int k = startIndex; k <= endIndex && k < tokenList.size(); k++)
						{
							newTokenList.push_back(tokenList[k]);
						}
						tableB = parser_select(newTokenList);
						table2 = &tableB;

					}
					else if (tokenList[0].content == "project")
					{
						vector<Token> newTokenList;
						for (int k = 1; k < tokenList.size(); k++)
						{
							newTokenList.push_back(tokenList[k]);
						}
						table2 = &parser_project(newTokenList);
						//return result;

					}
					else if (tokenList[0].content == "rename")
					{
						vector<Token> newTokenList;
						for (int k = 1; k < tokenList.size(); k++)
						{
							newTokenList.push_back(tokenList[k]);
						}
						table2 = &parser_rename(newTokenList);
						//return result;
					}
					else {
						//ERROR incorrect syntax
						cout << "ERROR incorrect syntax" << endl;
						Attribute ERROR("ERROR", "string");
						vector<Attribute> attributeList = { ERROR };
						Table result(attributeList, "ERROR");
						return result;
					}
					if (table1 != NULL && table2 != NULL)
					{
						//compute opperation and return table
						Table tableA = *table1;
						Table tableB = *table2;

						if (tokenList[currentToken + 1].content == "+")
						{
							//compute union of currentToken and currentToken + 2
							Table result = setUnion(tableA, tableB);
							return result;
						}
						else if (tokenList[currentToken + 1].content == "-")
						{
							//compute difference of currentToken and currentToken + 2
							Table result = setDifference(tableA, tableB);
							return result;
						}
						else if (tokenList[currentToken + 1].content == "*")
						{
							//compute cross product of currentToken and currentToken + 2
							Table result = crossProduct(tableA, tableB);
							return result;

						}
					}
				}
				else if (tokenList[currentToken + 1].content == "JOIN") //is the next token "JOIN"?
				{
					bool isTable = false;

					for (int i = 0; i < allTables.size(); i++)
					{
						if (allTables[i].getTableName() == tokenList[currentToken + 2].content)
						{
							//token is a table
							table2_index = i;
							table2 = &allTables[i];
							isTable = true;
							break;
						}
					}

					if (isTable)
					{
						//compute natural join
						if (table1 != NULL && table2 != NULL)
						{
							Table tableA = *table1;
							Table tableB = *table2;

							string name = "";
							string type = "";
							string keyType = "";

							for (int x = 0; x < tableA.getAttributes().size(); x++)
							{
								for (int y = 0; y < tableB.getAttributes().size(); y++)
								{
									if (tableA.getAttributes()[x].getName() == tableB.getAttributes()[y].getName() && tableA.getAttributes()[x].getType() == tableB.getAttributes()[y].getType())
									{
										name = tableA.getAttributes()[x].getName();
										type = tableA.getAttributes()[x].getType();
										keyType = tableA.getAttributes()[x].getKeyType();
										x = tableA.getAttributes().size();
										y = tableB.getAttributes().size();
									}
								}
							}
							if (name != "" && type != "")
							{
								Attribute attribute(name, type, keyType);
								Table result = naturalJoin(tableA, tableB, attribute);
								return result;
							}
						}
					}
					else if (tokenList[currentToken + 2].content == "(")
					{
						//find corresponding closed parenthesis
						int numOpenParens = 1;
						int numClosedParens = 0;
						int lastClosedParenIndex = 0;
						for (int k = currentToken + 1; k < tokenList.size(); k++)
						{
							if (tokenList[k].content == "(")
							{
								numOpenParens++;
							}
							else if (tokenList[k].content == ")")
							{
								numClosedParens++;
								if (numClosedParens == numOpenParens){
									lastClosedParenIndex = k;
									break;
								}
							}
						}
						if (numClosedParens != numOpenParens)
						{
							//ERROR not enough parenthesis
							cout << "ERROR mismatched parenthesis" << endl;
							Attribute ERROR("ERROR", "string");
							vector<Attribute> attributeList = { ERROR };
							Table result(attributeList, "ERROR");
							return result;
						}
						else
						{
							//create new vector of tokens with tokens enclosed by outermost parens
							vector<Token> newTokenList;
							for (int k = currentToken + 1; k < lastClosedParenIndex; k++)
							{
								newTokenList.push_back(tokenList[k]);
							}
							tableB = atomicExpResolver(newTokenList);
							table2 = &tableB;
							currentToken = lastClosedParenIndex;

							if (table1 != NULL && table2 != NULL)
							{
								//compute natural join and return table
								Table tableA = *table1;
								Table tableB = *table2;

								string name = "";
								string type = "";
								string keyType = "";

								for (int x = 0; x < tableA.getAttributes().size(); x++)
								{
									for (int y = 0; y < tableB.getAttributes().size(); y++)
									{
										if (tableA.getAttributes()[x].getName() == tableB.getAttributes()[y].getName() && tableA.getAttributes()[x].getType() == tableB.getAttributes()[y].getType())
										{
											name = tableA.getAttributes()[x].getName();
											type = tableA.getAttributes()[x].getType();
											keyType = tableA.getAttributes()[x].getKeyType();
											x = tableA.getAttributes().size();
											y = tableB.getAttributes().size();
										}
									}
								}
								if (name != "" && type != "")
								{
									Attribute attribute(name, type, keyType);
									Table result = naturalJoin(tableA, tableB, attribute);
									return result;
								}
							}
						}
					}
				}
			}
			else
			{
				//this was the only token provided to the function
				return tableA;
			}
		}
		if (tokenList[currentToken].content == "(")
		{
			//find corresponding closed parenthesis
			int numOpenParens = 1;
			int numClosedParens = 0;
			int lastClosedParenIndex = 0;
			for (int k = currentToken + 1; k < tokenList.size(); k++)
			{
				if (tokenList[k].content == "(")
				{
					numOpenParens++;
				}
				else if (tokenList[k].content == ")")
				{
					numClosedParens++;
					if (numClosedParens == numOpenParens){
						lastClosedParenIndex = k;
						break;
					}
				}
			}
			if (numClosedParens != numOpenParens)
			{
				//ERROR not enough parenthesis
				cout << "ERROR mismatched parenthesis" << endl;
				Attribute ERROR("ERROR", "string");
				vector<Attribute> attributeList = { ERROR };
				Table result(attributeList, "ERROR");
				return result;
			}
			else
			{
				//create new vector of tokens with tokens enclosed by outermost parens
				vector<Token> newTokenList;
				for (int k = currentToken + 1; k < lastClosedParenIndex; k++)
				{
					newTokenList.push_back(tokenList[k]);
				}
				tableA = atomicExpResolver(newTokenList);
				table1 = &tableA;
				currentToken = lastClosedParenIndex;
			}
			if (table1 != NULL && currentToken + 2 < tokenList.size())
			{
				if (tokenList[currentToken + 1].type == op)
				{
					bool isTable = false;

					for (int i = 0; i < allTables.size(); i++)
					{
						if (allTables[i].getTableName() == tokenList[currentToken + 2].content)
						{
							//token is a table
							table2_index = i;
							table2 = &allTables[i];
							isTable = true;
							break;
						}
					}
					if (isTable)
					{
						if (table1 != NULL && table2 != NULL)
						{
							//compute opperation and return table
							Table tableA = *table1;
							Table tableB = *table2;

							if (tokenList[currentToken + 1].content == "+")
							{
								//compute union of currentToken and currentToken + 2
								Table result = setUnion(tableA, tableB);
								return result;
							}
							else if (tokenList[currentToken + 1].content == "-")
							{
								//compute difference of currentToken and currentToken + 2
								Table result = setDifference(tableA, tableB);
								return result;
							}
							else if (tokenList[currentToken + 1].content == "*")
							{
								//compute cross product of currentToken and currentToken + 2
								Table result = crossProduct(tableA, tableB);
								return result;

							}
						}
					}
					else if (tokenList[currentToken + 2].content == "(")
					{
						//find corresponding closed parenthesis
						int numOpenParens = 1;
						int numClosedParens = 0;
						int lastClosedParenIndex = 0;
						for (int k = currentToken + 1; k < tokenList.size(); k++)
						{
							if (tokenList[k].content == "(")
							{
								numOpenParens++;
							}
							else if (tokenList[k].content == ")")
							{
								numClosedParens++;
								if (numClosedParens == numOpenParens){
									lastClosedParenIndex = k;
									break;
								}
							}
						}
						if (numClosedParens != numOpenParens)
						{
							//ERROR not enough parenthesis
							cout << "ERROR mismatched parenthesis" << endl;
							Attribute ERROR("ERROR", "string");
							vector<Attribute> attributeList = { ERROR };
							Table result(attributeList, "ERROR");
							return result;
						}
						else
						{
							//create new vector of tokens with tokens enclosed by outermost parens
							vector<Token> newTokenList;
							for (int k = currentToken + 1; k < lastClosedParenIndex; k++)
							{
								newTokenList.push_back(tokenList[k]);
							}
							tableB = atomicExpResolver(newTokenList);
							table2 = &tableB;
							currentToken = lastClosedParenIndex;
							if (table1 != NULL && table2 != NULL)
							{
								//compute opperation and return table
								Table tableA = *table1;
								Table tableB = *table2;

								if (tokenList[currentToken + 1].content == "+")
								{
									//compute union of currentToken and currentToken + 2
									Table result = setUnion(tableA, tableB);
									return result;
								}
								else if (tokenList[currentToken + 1].content == "-")
								{
									//compute difference of currentToken and currentToken + 2
									Table result = setDifference(tableA, tableB);
									return result;
								}
								else if (tokenList[currentToken + 1].content == "*")
								{
									//compute cross product of currentToken and currentToken + 2
									Table result = crossProduct(tableA, tableB);
									return result;

								}
							}
						}
					}
					else
					{
						//ERROR incorrect syntax
						cout << "ERROR incorrect syntax" << endl;
						Attribute ERROR("ERROR", "string");
						vector<Attribute> attributeList = { ERROR };
						Table result(attributeList, "ERROR");
						return result;
					}
				}
				else if (tokenList[currentToken + 1].content == "JOIN") //is the next token "JOIN"?
				{
					bool isTable = false;

					for (int i = 0; i < allTables.size(); i++)
					{
						if (allTables[i].getTableName() == tokenList[currentToken + 2].content)
						{
							//token is a table
							table2_index = i;
							table2 = &allTables[i];
							isTable = true;
							break;
						}
					}

					if (isTable)
					{
						//compute natural join
						if (table1 != NULL && table2 != NULL)
						{
							Table tableA = *table1;
							Table tableB = *table2;

							string name = "";
							string type = "";
							string keyType = "";

							for (int x = 0; x < tableA.getAttributes().size(); x++)
							{
								for (int y = 0; y < tableB.getAttributes().size(); y++)
								{
									if (tableA.getAttributes()[x].getName() == tableB.getAttributes()[y].getName() && tableA.getAttributes()[x].getType() == tableB.getAttributes()[y].getType())
									{
										name = tableA.getAttributes()[x].getName();
										type = tableA.getAttributes()[x].getType();
										keyType = tableA.getAttributes()[x].getKeyType();
										x = tableA.getAttributes().size();
										y = tableB.getAttributes().size();
									}
								}
							}
							if (name != "" && type != "")
							{
								Attribute attribute(name, type, keyType);
								Table result = naturalJoin(tableA, tableB, attribute);
								return result;
							}
						}
					}
					else if (tokenList[currentToken + 2].content == "(")
					{
						//find corresponding closed parenthesis
						int numOpenParens = 1;
						int numClosedParens = 0;
						int lastClosedParenIndex = 0;
						for (int k = currentToken + 1; k < tokenList.size(); k++)
						{
							if (tokenList[k].content == "(")
							{
								numOpenParens++;
							}
							else if (tokenList[k].content == ")")
							{
								numClosedParens++;
								if (numClosedParens == numOpenParens){
									lastClosedParenIndex = k;
									break;
								}
							}
						}
						if (numClosedParens != numOpenParens)
						{
							//ERROR not enough parenthesis
							cout << "ERROR mismatched parenthesis" << endl;
							Attribute ERROR("ERROR", "string");
							vector<Attribute> attributeList = { ERROR };
							Table result(attributeList, "ERROR");
							return result;
						}
						else
						{
							//create new vector of tokens with tokens enclosed by outermost parens
							vector<Token> newTokenList;
							for (int k = currentToken + 1; k < lastClosedParenIndex; k++)
							{
								newTokenList.push_back(tokenList[k]);
							}
							tableB = atomicExpResolver(newTokenList);
							table2 = &tableB;
							currentToken = lastClosedParenIndex;

							if (table1 != NULL && table2 != NULL)
							{
								//compute natural join and return table
								Table tableA = *table1;
								Table tableB = *table2;

								string name = "";
								string type = "";
								string keyType = "";

								for (int x = 0; x < tableA.getAttributes().size(); x++)
								{
									for (int y = 0; y < tableB.getAttributes().size(); y++)
									{
										if (tableA.getAttributes()[x].getName() == tableB.getAttributes()[y].getName() && tableA.getAttributes()[x].getType() == tableB.getAttributes()[y].getType())
										{
											name = tableA.getAttributes()[x].getName();
											type = tableA.getAttributes()[x].getType();
											keyType = tableA.getAttributes()[x].getKeyType();
											x = tableA.getAttributes().size();
											y = tableB.getAttributes().size();
										}
									}
								}
								if (name != "" && type != "")
								{
									Attribute attribute(name, type, keyType);
									Table result = naturalJoin(tableA, tableB, attribute);
									return result;
								}
							}
						}
					}
				}
				else
				{
					//ERROR incorrect syntax
					cout << "ERROR incorrect syntax" << endl;
					Attribute ERROR("ERROR", "string");
					vector<Attribute> attributeList = { ERROR };
					Table result(attributeList, "ERROR");
					return result;
				}
			}
			else
			{
				//ERROR incorrect syntax
				cout << "ERROR incorrect syntax" << endl;
				Attribute ERROR("ERROR", "string");
				vector<Attribute> attributeList = { ERROR };
				Table result(attributeList, "ERROR");
				return result;
			}
		}
		else
		{
			//ERROR incorrect syntax
			cout << "ERROR incorrect syntax" << endl;
			Attribute ERROR("ERROR", "string");
			vector<Attribute> attributeList = { ERROR };
			Table result(attributeList, "ERROR");
			return result;
		}
	}
}

//parser functions
Table parser_select(vector <Token> cmd)
{
	int tokenIndex = 0;

	Table table;
	Table result;
	Table temp1;
	Table temp2;

	vector<Token> conditionTokenList;

	for (tokenIndex += 1; tokenIndex < cmd.size() && cmd[tokenIndex].content != ")"; tokenIndex++)
	{
		conditionTokenList.push_back(cmd[tokenIndex]);
	}

	if (tokenIndex + 1 < cmd.size() && cmd[tokenIndex + 1].content == "(")
	{
		//find corresponding closed parenthesis
		int numOpenParens = 1;
		int numClosedParens = 0;
		int lastClosedParenIndex = 0;
		for (int k = tokenIndex + 2; k < cmd.size(); k++)
		{
			if (cmd[k].content == "(" )
			{
				numOpenParens++;
			}
			else if (cmd[k].content == ")")
			{
				numClosedParens++;
				if (numClosedParens == numOpenParens){
					lastClosedParenIndex = k;
					break;
				}
			}
		}
		if (numClosedParens != numOpenParens)
		{
			//ERROR not enough parenthesis
			cout << "ERROR mismatched parenthesis" << endl;
			Attribute ERROR("ERROR", "string");
			vector<Attribute> attributeList = { ERROR };
			Table result(attributeList, "ERROR");
			return result;
		}
		else
		{
			vector<Token> newTokenList;
			for (int i = tokenIndex + 2; i < cmd.size() && i < lastClosedParenIndex; i++)
			{
				newTokenList.push_back(cmd[i]);
			}
			table = atomicExpResolver(newTokenList);
		}
	}
	else if (tokenIndex + 1 < cmd.size())
	{
		vector<Token> newTokenList = {cmd[tokenIndex + 1]};
		table = atomicExpResolver(newTokenList);
	}

	//iterate through conditionTokenList
	string boolOp = "";
	bool leftHasQuotes = false;
	for (int currentToken = 0; currentToken < conditionTokenList.size(); currentToken++)
	{
		if (conditionTokenList.size() > currentToken + 2 && conditionTokenList[currentToken].type == identifier &&
			conditionTokenList[currentToken + 1].type == op && conditionTokenList[currentToken + 2].type == op ||
			conditionTokenList[currentToken + 2].type == identifier || conditionTokenList[currentToken + 2].type == number ||
			conditionTokenList[currentToken + 2].type == quotes)
		{
			string leftOperand = conditionTokenList[currentToken].content;
			string operation = "";
			string rightOperand = "";

			if (conditionTokenList.size() > currentToken + 3 && conditionTokenList[currentToken + 3].type == quotes)
			{
				leftHasQuotes = true;
				if (conditionTokenList.size() > currentToken + 4)
				{
					rightOperand = conditionTokenList[currentToken + 4].content;
				}
			}
			if (conditionTokenList[currentToken + 1].content == "=")
			{
				operation = "==";
				if (rightOperand == "")
				{
					rightOperand = conditionTokenList[currentToken + 3].content;
				}
				currentToken = currentToken + 3;
			}
			else if (conditionTokenList[currentToken + 1].content == "!")
			{
				operation = "!=";
				if (rightOperand == "")
				{
					rightOperand = conditionTokenList[currentToken + 3].content;
				}
				currentToken = currentToken + 3;
			}
			else if (conditionTokenList[currentToken + 1].content == "<")
			{
				if (conditionTokenList[currentToken + 2].content == "=")
				{
					operation = "<=";
					rightOperand = conditionTokenList[currentToken + 3].content;
					currentToken = currentToken + 3;
				}
				else
				{
					operation = "<";
					rightOperand = conditionTokenList[currentToken + 2].content;
					currentToken = currentToken + 2;
				}
			}
			else if (conditionTokenList[currentToken + 1].content == ">")
			{
				if (conditionTokenList[currentToken + 2].content == "=")
				{
					operation = ">=";
					rightOperand = conditionTokenList[currentToken + 3].content;
					currentToken = currentToken + 3;
				}
				else
				{
					operation = ">";
					rightOperand = conditionTokenList[currentToken + 2].content;
					currentToken = currentToken + 2;
				}
			}
			if (boolOp == "")
			{
				result = select(table, leftOperand, operation, rightOperand);
			}
			else
			{
				temp1 = select(table, leftOperand, operation, rightOperand);

				if (boolOp == "&&")
				{
					result = intersection(result, temp1);
					boolOp = "";
				}
				else
				{
					result = setUnion(result, temp1);
					boolOp = "";
				}
			}
		}
		if (leftHasQuotes)
		{
			currentToken += 2;
			leftHasQuotes = false;
		}
		if (currentToken + 2 < conditionTokenList.size())
		{
			//boolean condition exists
			boolOp = conditionTokenList[currentToken + 1].content + conditionTokenList[currentToken + 2].content;
			currentToken += 2;
		}
	}

	return result;

}

Table parser_project(vector <Token> cmd)
{
	int tokenIndex = 0;

	Table table;
	Table result;
	Table temp1;
	Table temp2;
	vector<Attribute> attr;

	vector<Token> conditionTokenList;

	for (tokenIndex += 5; tokenIndex < cmd.size() && cmd[tokenIndex].content != ")"; tokenIndex++)
	{
		conditionTokenList.push_back(cmd[tokenIndex]);
	}

	if (tokenIndex + 1 < cmd.size() && cmd[tokenIndex + 1].content == "(")
	{
		//find corresponding closed parenthesis
		int numOpenParens = 1;
		int numClosedParens = 0;
		int lastClosedParenIndex = 0;
		for (int k = tokenIndex + 2; k < cmd.size(); k++)
		{
			if (cmd[k].content == "(")
			{
				numOpenParens++;
			}
			else if (cmd[k].content == ")")
			{
				numClosedParens++;
				if (numClosedParens == numOpenParens){
					lastClosedParenIndex = k;
					break;
				}
			}
		}
		if (numClosedParens != numOpenParens)
		{
			//ERROR not enough parenthesis
			cout << "ERROR mismatched parenthesis" << endl;
			Attribute ERROR("ERROR", "string");
			vector<Attribute> attributeList = { ERROR };
			Table result(attributeList, "ERROR");
			return result;
		}
		else
		{
			vector<Token> newTokenList;
			for (int i = tokenIndex + 2; i < cmd.size() && i < lastClosedParenIndex; i++)
			{
				newTokenList.push_back(cmd[i]);
			}
			table = atomicExpResolver(newTokenList);
		}
	}
	else if (tokenIndex + 1 < cmd.size())
	{
		vector<Token> newTokenList = { cmd[tokenIndex + 1] };
		table = atomicExpResolver(newTokenList);
	}
	for (int w = 0; w < conditionTokenList.size(); w++){
		if (conditionTokenList[w].content != ","){
			Attribute a(conditionTokenList[w].content, "string");
			attr.push_back(a);
		}

	}
	result = projection(table, attr);
	return result;
}

Table parser_union(vector <Token> cmd)
{
	//BEGIN DUMMY IMPLEMENTATION
	Attribute dummy("dummy", "string", "primary key");
	vector <Attribute> attributeList = { dummy };
	Table result(attributeList, "result");
	return result;

	//END DUMMY IMPLEMENTATION
}

Table parser_difference(vector <Token> cmd)
{
	//BEGIN DUMMY IMPLEMENTATION
	Attribute dummy("dummy", "string", "primary key");
	vector <Attribute> attributeList = { dummy };
	Table result(attributeList, "result");
	return result;

	//END DUMMY IMPLEMENTATION
}

//Takes in input command from vector. Inserts requests in command into table listed in command
void parser_insertInto(vector <Token> cmd)
{
	int positionInInput;
	int tableLoc;	//stores location of table in vector<Table> allTables
	vector <string> addThisRow; //contains row that will be inserted into table.

	//find table in vector <Token> cmd
	if (cmd[0].type == identifier)
	{
		bool foundTable = false;
		for (int x = 0; x < allTables.size(); x++)
		{
			if (cmd[0].content == allTables[x].getTableName())
			{
				tableLoc = x;
				foundTable = true;
				break;
			}
		}
		if (!foundTable)
		{
			cout << "ERROR: INSERT INTO table not found in vector of all tables" << endl;
			return;
		}
	}
	else
	{
		cout << "ERROR: INSERT INTO expected table name after INSERT INTO call" << endl;
		return;
	}

	//extract row from vector <Token> cmd and store into vector <string> addThisRow
	if (cmd[1].type == identifier && cmd[1].content == "VALUES" && cmd[2].type == identifier && cmd[2].content == "FROM")
	{
		if (cmd[3].type == punc && cmd[3].content == "(")
		{
			int openParenthesisCount = 0;
			int closedParenthesisCount = 0;
			positionInInput = 3;
			while (positionInInput < cmd.size())
			{
				if (cmd[positionInInput].content == ")")
				{
					closedParenthesisCount++;
				}
				else if (cmd[positionInInput].content == "(")
				{
					openParenthesisCount++;
				}
				positionInInput++;
			}

			if (openParenthesisCount == closedParenthesisCount)
			{
				if (cmd[4].content == "\"")
				{
					positionInInput = 5;
					while (positionInInput < cmd.size())
					{
						if (cmd[positionInInput].type == identifier)
						{
							addThisRow.push_back(cmd[positionInInput].content);
							positionInInput++;
						}
						else if (cmd[positionInInput].type == number)
						{
							addThisRow.push_back(cmd[positionInInput].content);
							positionInInput++;
						}
						else
						{
							positionInInput++;
						}
					}
					if (addThisRow.size() != allTables[tableLoc].getNumAttrs())
					{
						cout << "ERROR: Insert Into attributes in row does not match num attributes required to be in row" << endl;
						return;
					}
					
				}
			}
			else
			{
				cout << "ERROR: INSERT INTO open and closed parenthesis counts don't match up" << endl;
				return;
			}

		}
		else
		{
			cout << "ERROR INSERT INTO: expected \"(\" after VALUES FROM" << endl;
			return;
		}
	}
	else
	{
		cout << "ERROR: INSERT INTO expected \"VALUES FROM\" after table name" << endl;
		return;
	}
	//call void insertRow(Table &_table, vector <string> addRow)
	if (testingParserInsertInto)
	{
		cout << "ADD THIS ROW = ";
		for (int x = 0; x < addThisRow.size(); x++)
		{
			cout << addThisRow[x] << " ";
		}
		cout << endl;
	}
	insertRow(allTables[tableLoc], addThisRow);
	cout << "Row has been inserted. The table now looks like this: " << endl;
	allTables[tableLoc].printTable(allTables[tableLoc]);
}

void parser_Delete(vector <Token> cmd)
{
	int positionInInput;
	int tableLoc;	//stores location of table in vector<Table> allTables
	int attrLoc;
	vector <string> deleteRows; //rows marked for deletion will be stored here

	bool andcase = false; //condition involves &&
	bool orcase = false; //condition involves ||

	//find table in vector <Token> cmd
	if (cmd[0].type == identifier)
	{
		bool foundTable = false;
		for (int x = 0; x < allTables.size(); x++)
		{
			if (cmd[0].content == allTables[x].getTableName())
			{
				tableLoc = x;
				foundTable = true;
				break;
			}
		}
		if (!foundTable)
		{
			cout << "ERROR: DELETE table not found in vector of all tables" << endl;
			return;
		}
	}
	else
	{
		cout << "ERROR: DELETE expected table name after DELETE FROM call" << endl;
		return;
	}

	if (cmd[1].content == "WHERE")
	{
		if (cmd[2].content == "(")
		{
			int openparenthesiscount = 0;
			int closedparenthesiscount = 0;
			positionInInput = 2;
			while (positionInInput < cmd.size())
			{
				if (cmd[positionInInput].content == "(")
				{	
					openparenthesiscount++;
					positionInInput++;
				}
				else if (cmd[positionInInput].content == ")")
				{
					closedparenthesiscount++;
					positionInInput++;
				}
				else
					positionInInput++;
			}
			if (openparenthesiscount == closedparenthesiscount)
			{
				openparenthesiscount = 0;
				closedparenthesiscount = 0;
				bool firsttime = true;
				positionInInput = 2; 
				Token compareAgainst;				//value to compare attribute agains
				while (openparenthesiscount != closedparenthesiscount || firsttime)
				{
					firsttime = false;
					if (cmd[positionInInput].content == "(")
					{
						openparenthesiscount++;
						positionInInput++;
					}
					else if (cmd[positionInInput].content == ")")
					{
						closedparenthesiscount++;
						positionInInput++;
					}
					else if (cmd[positionInInput].type == identifier)
					{
						bool found = false;
						//look up identifier in list of attributes to see if it exists and store location of identifier
						for (int x = 0; x < allTables[tableLoc].getNumAttrs(); x++)
						{
							if (testingParserDelete)
							{
								cout << allTables[tableLoc].getNumAttrs() << endl;
							}
							if (cmd[positionInInput].content == allTables[tableLoc].attrNameAt(x))
							{
								if (testingParserDelete)
								{
									cout << "ATTR FOUND" <<endl;
								}
								found = true;
								attrLoc = x;
								break;
							}
						}
						if (!found)
						{
							cout << "ERROR: DELETE attribute not found" << endl;
							return;
						}
						positionInInput++;
					}
					else if (cmd[positionInInput].content == "<")
					{
						if (cmd[positionInInput + 1].content == "=")
						{
							positionInInput = positionInInput + 2;
							if (cmd[positionInInput].type == number)
							{
								compareAgainst = cmd[positionInInput];
								//store what is to be deleted
								for (int x = 0; x < allTables[tableLoc].getNumRows(); x++)
								{
									if (atoi(allTables[tableLoc].getRowAttr(x, attrLoc).c_str()) <= atoi(compareAgainst.content.c_str()))
									{
											cout << "PRIMARY KEY TO BE DELETED: " << allTables[tableLoc].getPrimaryKey(x);
											deleteRows.push_back(allTables[tableLoc].getPrimaryKey(x));
									}
								}
							}
							else
							{
								cout << "ERROR: DELETE condition. Cannot use \"<\" or \">\" with strings/varchars" << endl;
							}
							positionInInput++;
						}
						else
						{
							positionInInput++;
							if (cmd[positionInInput].type == number)
							{
								compareAgainst = cmd[positionInInput];
								//store what is to be deleted
								for (int x = 0; x < allTables[tableLoc].getNumRows(); x++)
								{
									if (atoi(allTables[tableLoc].getRowAttr(x, attrLoc).c_str()) < atoi(compareAgainst.content.c_str()))
									{
											deleteRows.push_back(allTables[tableLoc].getPrimaryKey(x));
									}
								}
							}
							else
							{
								cout << "ERROR: DELETE condition. Cannot use \"<\" or \">\" with strings/varchars" << endl;
							}
							positionInInput++;
						}
					}
					else if (cmd[positionInInput].content == ">")
					{
						if (cmd[positionInInput + 1].content == "=")
						{
							positionInInput = positionInInput + 2;
							if (cmd[positionInInput].type == number)
							{
								compareAgainst = cmd[positionInInput];
								//store what is to be deleted
								for (int x = 0; x < allTables[tableLoc].getNumRows(); x++)
								{
									if (atoi(allTables[tableLoc].getRowAttr(x, attrLoc).c_str()) >= atoi(compareAgainst.content.c_str()))
									{
											deleteRows.push_back(allTables[tableLoc].getPrimaryKey(x));
									}
								}
							}
							else
							{
								cout << "ERROR: DELETE condition. Cannot use \"<\" or \">\" with strings/varchars" << endl;
							}
							positionInInput++;
						}
						else
						{
							positionInInput++;
							if (cmd[positionInInput].type == number)
							{
								compareAgainst = cmd[positionInInput];
								//store what is to be deleted
								for (int x = 0; x < allTables[tableLoc].getNumRows(); x++)
								{
									if (atoi(allTables[tableLoc].getRowAttr(x, attrLoc).c_str()) > atoi(compareAgainst.content.c_str()))
									{
											deleteRows.push_back(allTables[tableLoc].getPrimaryKey(x));
									}
								}
							}
							else
							{
								cout << "ERROR: DELETE condition. Cannot use \"<\" or \">\" with strings/varchars" << endl;
							}
							positionInInput++;
						}
					}
					else if (cmd[positionInInput].content == "=")
					{
						if (cmd[positionInInput + 1].content == "=")
						{
							positionInInput = positionInInput + 2;
							if (cmd[positionInInput].type == number)
							{
								compareAgainst = cmd[positionInInput];
								positionInInput++;
							}
							else if (cmd[positionInInput].content == "\"")
							{
								compareAgainst = cmd[positionInInput + 1];
								if (cmd[positionInInput + 2].content == "\"")
									positionInInput = positionInInput + 3;
								else
								{
									cout << "ERROR: DELETE expected closing \"" << endl;
									return;
								}
							}

							//store rows to be deleted
							if (compareAgainst.type == identifier)
							{
								for (int x = 0; x < allTables[tableLoc].getNumRows(); x++)
								{
									if (allTables[tableLoc].getRowAttr(x, attrLoc) == compareAgainst.content)
									{
										deleteRows.push_back(allTables[tableLoc].getPrimaryKey(x));
									}
								}
							}
							else
							{
								for (int x = 0; x < allTables[tableLoc].getNumRows(); x++)
								{
									if (atoi(allTables[tableLoc].getRowAttr(x, attrLoc).c_str()) == atoi(compareAgainst.content.c_str()))
									{
										deleteRows.push_back(allTables[tableLoc].getPrimaryKey(x));
									}
								}
							}
						}
						else
						{
							cout << "ERROR: DELETE expected \"=\" after \"=\"";
							return;
						}

					}
					else if (cmd[positionInInput].content == "!")
					{
						if (cmd[positionInInput + 1].content == "=")
						{
							positionInInput = positionInInput + 2;
							if (cmd[positionInInput].type == number)
							{
								compareAgainst = cmd[positionInInput];
								positionInInput++;
							}
							else if (cmd[positionInInput].content == "\"")
							{
								compareAgainst = cmd[positionInInput + 1];
								if (cmd[positionInInput + 2].content == "\"")
									positionInInput = positionInInput + 3;
								else
								{
									cout << "ERROR: DELETE expected closing \"" << endl;
									return;
								}
							}

							//store rows to be deleted
							if (compareAgainst.type == identifier)
							{
								for (int x = 0; x < allTables[tableLoc].getNumRows(); x++)
								{
									if (allTables[tableLoc].getRowAttr(x, attrLoc) != compareAgainst.content)
									{
										deleteRows.push_back(allTables[tableLoc].getPrimaryKey(x));
									}
								}
							}
							else
							{
								for (int x = 0; x < allTables[tableLoc].getNumRows(); x++)
								{
									if (atoi(allTables[tableLoc].getRowAttr(x, attrLoc).c_str()) != atoi(compareAgainst.content.c_str()))
									{
										deleteRows.push_back(allTables[tableLoc].getPrimaryKey(x));
									}
								}
							}
						}
						else
						{
							cout << "ERROR: DELETE expected \"=\" after \"!\"" << endl;
							return;
						}
					}
					else if (cmd[positionInInput].content == "|")
					{
						if (cmd[positionInInput + 1].content == "|")
						{
							positionInInput = positionInInput + 2;
							orcase = true;
						}
						else
						{
							cout << "ERROR: DELETE expected \"|\" after \"|\"" << endl;
							return;
						}
					}
					else if (cmd[positionInInput].content == "&")
					{
						if (cmd[positionInInput + 1].content == "&")
						{
							positionInInput = positionInInput + 2;
							andcase = true;
						}
						else
						{
							cout << "ERROR: DELETE expected \"&\" after \"&\"" << endl;
							return;
						}
					}
					else
						positionInInput++;
				}
			}
			else
			{
				cout << "ERROR: DELETE open parenthesis count and closed parenthesis count dont match" << endl;
				return;
			}
		}
		else
		{
			cout << "ERROR: DELETE expected \"(\" after WHERE" << endl;
			return;
		}
	}
	else
	{
		cout << "ERROR: DELETE expected WHERE after table name" << endl;
		return;
	}

	//for && case remove the ones from the deleteRows vector that dont show up multiple times and only keep one copy of the ones that do show up multiple times
	if (andcase)
	{
		vector <string> deleteTheseRows;
		for (int x = 0; x < deleteRows.size(); x++)
		{
			bool repeat = false;
			for (int y = 0; y < deleteRows.size(); y++)
			{
				if (y>=deleteRows.size() || x>=deleteRows.size())
				{
					break;
				}
				if (y != x && deleteRows[y] == deleteRows[x])
				{
					repeat = true;
					deleteRows.erase(deleteRows.begin() + y);
				}
			}
			if (repeat == true)
			{
				if (x >= deleteRows.size())
					break;
				cout << "PRIMARY KEY TO BE DELETED: " << deleteRows[x] << endl;
				deleteTheseRows.push_back(deleteRows[x]);
			}
		}

		for (int x = 0; x < deleteTheseRows.size(); x++)
		{
			deleteRow(allTables[tableLoc], deleteTheseRows[x]);
		}
	}
	else if (orcase)
	{
		vector <string> deleteTheseRows;
		for (int x = 0; x < deleteRows.size(); x++)
		{
			if (x == 0)
			{
				deleteTheseRows.push_back(deleteRows[x]);
			}
			else
			{
				bool repeat = false;
				for (int y = 0; y < deleteTheseRows.size(); y++)
				{
					if (deleteTheseRows[y] == deleteRows[x])
					{
						repeat = true;
					}
				}
				if (!repeat)
				{
					deleteTheseRows.push_back(deleteRows[x]);
				}
			}
		}
		for (int x = 0; x < deleteTheseRows.size(); x++)
		{
			deleteRow(allTables[tableLoc], deleteTheseRows[x]);
		}
	}
	else
	{
		for (int x = 0; x < deleteRows.size(); x++)
		{
			deleteRow(allTables[tableLoc], deleteRows[x]);
		}
	}
	cout << "Row that met the conditions have been deleted. The table now looks like this: " << endl;
	allTables[tableLoc].printTable(allTables[tableLoc]);
}

void paser_updateValue(vector<Token> cmd)
{
	if (cmd[0].type == identifier)
	{
		bool found = false;
		int tableloc;			//send to allTables[tableloc] to updateValue dbms function
		int attrloc;			//for attribute call to updateValue dbms function. allTables[tableloc].attrNameAt(x)
		string newvalue;		//new value used in updatevalue dbms function call
		for (int x = 0; x < allTables.size(); x++)
		{
			if (allTables[x].getTableName() == cmd[0].content)
			{
				found = true;
				tableloc = x;
				break;
			}
		}
		if (!found)
		{
			cout << "ERROR: Update value parser. Table not found" << endl;
			return;
		}
		if (cmd[1].content == "SET")
		{
			if (cmd[2].type == identifier)
			{
				//check if identifier is valid attribute in table
				bool attrExists = false;
				for (int x = 0; x < allTables[tableloc].getNumAttrs(); x++)
				{
					if (cmd[2].content == allTables[tableloc].attrNameAt(x))
					{
						attrExists = true;
						attrloc = x;
						break;
					}
				}
				if (!attrExists)
				{
					cout << "ERROR: Update value parser: Attribute listed does not exist" << endl;
					return;
				}

				if (cmd[3].content == "=")
				{
					newvalue = cmd[4].content;
				}
				else
				{
					cout << "ERROR: Update value parser: assignment operator expected after change identifier" << endl;
					return;
				}

				if (cmd[5].content == "WHERE")
				{
					if (cmd[6].type == identifier)
					{
						int conditionLoc;
						bool conditionAttrfound = false;
						for (int x = 0; x < allTables[tableloc].getNumAttrs(); x++)
						{
							if (cmd[6].content == allTables[tableloc].attrNameAt(x))
							{
								conditionAttrfound = true;
								conditionLoc = x;
								break;
							}
						}
						if (!conditionAttrfound)
						{
							cout << "ERROR: update value parser: condition attribute not found" << endl;
							return;
						}
						if (cmd[7].content == "=")
						{
							if (cmd[8].content == "=")
							{
								if (cmd[10].type == identifier)
								{
									for (int x = 0; x < allTables[tableloc].getNumRows(); x++)
									{
										if (cmd[10].content == allTables[tableloc].getRowAttr(x, attrloc))
										{
											Attribute a(allTables[tableloc].attrNameAt(attrloc), allTables[tableloc].attrTypeAt(attrloc), allTables[tableloc].attrKeyAt(attrloc));
											updateValue(allTables[tableloc], allTables[tableloc].getPrimaryKey(x), a, newvalue);
										}
									}
									if (testingUpdateValue)
									{
										cout << "Table has been updated as requested. The table now looks like: " << endl;
										allTables[tableloc].printTable(allTables[tableloc]);
										return;
									}
									return;
								}
								else
								{
									for (int x = 0; x < allTables[tableloc].getNumRows(); x++)
									{
										if (atoi(cmd[9].content.c_str()) == atoi(allTables[tableloc].getRowAttr(x, attrloc).c_str()))
										{
											Attribute a(allTables[tableloc].attrNameAt(attrloc), allTables[tableloc].attrTypeAt(attrloc), allTables[tableloc].attrKeyAt(attrloc));
											updateValue(allTables[tableloc], allTables[tableloc].getPrimaryKey(x), a, newvalue);
										}
									}
									if (testingUpdateValue)
									{
										cout << "Table has been updated as requested. The table now looks like: " << endl;
										allTables[tableloc].printTable(allTables[tableloc]);
										return;
									}
									return;
								}
							}
							else
							{
								cout << "ERROR: UpdateValue parser: \"=\" expected after \"=\"" << endl;
								return;
							}
						}
						else if (cmd[7].content == ">")
						{
							if (cmd[8].content == "=")
							{
								if (cmd[9].type == number)
								{
									for (int x = 0; x < allTables[tableloc].getNumRows(); x++)
									{
										if (atoi(cmd[9].content.c_str()) >= atoi(allTables[tableloc].getRowAttr(x, attrloc).c_str()))
										{
											Attribute a(allTables[tableloc].attrNameAt(attrloc), allTables[tableloc].attrTypeAt(attrloc), allTables[tableloc].attrKeyAt(attrloc));
											updateValue(allTables[tableloc], allTables[tableloc].getPrimaryKey(x), a, newvalue);
										}
									}
									if (testingUpdateValue)
									{
										cout << "Table has been updated as requested. The table now looks like: " << endl;
										allTables[tableloc].printTable(allTables[tableloc]);
										return;
									}
									return;
								}
								else
								{
									cout << "ERROR: Update Value parser: Cannot do greater than or less than with strings" << endl;
									return;
								}
							}
							else
							{
								if (cmd[9].type == number)
								{
									for (int x = 0; x < allTables[tableloc].getNumRows(); x++)
									{
										if (atoi(cmd[9].content.c_str()) > atoi(allTables[tableloc].getRowAttr(x, attrloc).c_str()))
										{
											Attribute a(allTables[tableloc].attrNameAt(attrloc), allTables[tableloc].attrTypeAt(attrloc), allTables[tableloc].attrKeyAt(attrloc));
											updateValue(allTables[tableloc], allTables[tableloc].getPrimaryKey(x), a, newvalue);
										}
									}
									if (testingUpdateValue)
									{
										cout << "Table has been updated as requested. The table now looks like: " << endl;
										allTables[tableloc].printTable(allTables[tableloc]);
										return;
									}
									return;
								}
								else
								{
									cout << "ERROR: Update Value parser: Cannot do greater than or less than with strings" << endl;
									return;
								}
							}
						}
						else if (cmd[7].content == "<")
						{
							if (cmd[8].content == "=")
							{
								if (cmd[9].type == number)
								{
									for (int x = 0; x < allTables[tableloc].getNumRows(); x++)
									{
										if (atoi(cmd[9].content.c_str()) <= atoi(allTables[tableloc].getRowAttr(x, attrloc).c_str()))
										{
											Attribute a(allTables[tableloc].attrNameAt(attrloc), allTables[tableloc].attrTypeAt(attrloc), allTables[tableloc].attrKeyAt(attrloc));
											updateValue(allTables[tableloc], allTables[tableloc].getPrimaryKey(x), a, newvalue);
										}
									}
									if (testingUpdateValue)
									{
										cout << "Table has been updated as requested. The table now looks like: " << endl;
										allTables[tableloc].printTable(allTables[tableloc]);
										return;
									}
									return;
								}
								else
								{
									cout << "ERROR: Update Value parser: Cannot do greater than or less than with strings" << endl;
									return;
								}
							}
							else
							{
								if (cmd[9].type == number)
								{
									for (int x = 0; x < allTables[tableloc].getNumRows(); x++)
									{
										if (atoi(cmd[9].content.c_str()) < atoi(allTables[tableloc].getRowAttr(x, attrloc).c_str()))
										{
											Attribute a(allTables[tableloc].attrNameAt(attrloc), allTables[tableloc].attrTypeAt(attrloc), allTables[tableloc].attrKeyAt(attrloc));
											updateValue(allTables[tableloc], allTables[tableloc].getPrimaryKey(x), a, newvalue);
										}
									}
									if (testingUpdateValue)
									{
										cout << "Table has been updated as requested. The table now looks like: " << endl;
										allTables[tableloc].printTable(allTables[tableloc]);
										return;
									}
									return;
								}
								else
								{
									cout << "ERROR: Update Value parser: Cannot do greater than or less than with strings" << endl;
									return;
								}
							}
						}
						else if (cmd[7].content == "!")
						{
							if (cmd[8].content == "=")
							{
								if (cmd[10].type == identifier)
								{
									for (int x = 0; x < allTables[tableloc].getNumRows(); x++)
									{
										if (cmd[10].content != allTables[tableloc].getRowAttr(x, attrloc))
										{
											Attribute a(allTables[tableloc].attrNameAt(attrloc), allTables[tableloc].attrTypeAt(attrloc), allTables[tableloc].attrKeyAt(attrloc));
											updateValue(allTables[tableloc], allTables[tableloc].getPrimaryKey(x), a, newvalue);
										}
									}
									if (testingUpdateValue)
									{
										cout << "Table has been updated as requested. The table now looks like: " << endl;
										allTables[tableloc].printTable(allTables[tableloc]);
										return;
									}
									return;
								}
								else
								{
									for (int x = 0; x < allTables[tableloc].getNumRows(); x++)
									{
										if (atoi(cmd[9].content.c_str()) == atoi(allTables[tableloc].getRowAttr(x, attrloc).c_str()))
										{
											Attribute a(allTables[tableloc].attrNameAt(attrloc), allTables[tableloc].attrTypeAt(attrloc), allTables[tableloc].attrKeyAt(attrloc));
											updateValue(allTables[tableloc], allTables[tableloc].getPrimaryKey(x), a, newvalue);
										}
									}
									if (testingUpdateValue)
									{
										cout << "Table has been updated as requested. The table now looks like: " << endl;
										allTables[tableloc].printTable(allTables[tableloc]);
										return;
									}
									return;
								}
							}
							else
							{
								cout << "ERROR: UpdateValue parser: expected \"=\" after \"!\"" << endl;
								return;
							}
						}
						else
						{
							cout << "ERROR: update value parser: conditional operands expected" << endl;
							return;
						}
					}
					else
					{
						cout << "ERROR: Update value parser: identifier expected after \"WHERE\" call" << endl;
						return;
					}
				}
				else
				{
					cout << "ERROR: Update  value parser: \"WHERE\" expected after assignment ";
					return;
				}
			}
			else
			{
				cout << "ERROR: Update value parser. Identifier expected after \"SET\"" << endl;
				return;
			}
		}
		else
		{
			cout << "ERROR: Update value parser. \"SET\" expected" << endl;
			return;
		}
	}
	else
	{
		cout << "ERROR: Update value identifier expected as first input." << endl;
		return;
	}
}

Table parser_product(vector <Token> cmd)
{
	vector<Token> atomichelp;
	for (int x = 0; x < cmd.size()-1; x++)
	{
		atomichelp.push_back(cmd[x]);
	}
	return atomicExpResolver(atomichelp);
}

Table parser_rename(vector <Token> cmd)
{
	if (cmd[0].content == "(")
	{
		int positionInInput = 1;
		int tableloc;
		vector <string> newnames;
		int openparenthesiscount = 1;
		int closedparenthesiscount = 0;

		//store all names that you want to change in vector <string> newnames. Each name changed must be in order it appears on the table. 
		//In other words, first name entered will change the name of the first attribute of the table; second name will change the second attribute in table, etc.
		while (openparenthesiscount != closedparenthesiscount)
		{
			if (cmd[positionInInput].content == ")")
			{
				closedparenthesiscount++;
				positionInInput++;
			}
			else if (cmd[positionInInput].content == "(")
			{
				openparenthesiscount++;
				positionInInput++;
			}
			else if (cmd[positionInInput].type == identifier)
			{
				newnames.push_back(cmd[positionInInput].content);
				positionInInput++;
			}
			else
			{
				positionInInput++;
			}
		}
		if (cmd[positionInInput].content == "(")
		{
			openparenthesiscount = 1;
			closedparenthesiscount = 0;
			positionInInput++;
			vector<Token> atomichelp;
			for (int x = positionInInput; x < cmd.size(); x++)
			{
				if (cmd[x].content == "(")
				{
					openparenthesiscount++;
				}
				else if (cmd[x].content == ")")
				{
					closedparenthesiscount++;
				}

				if (closedparenthesiscount == openparenthesiscount)
					break;

				atomichelp.push_back(cmd[x]);
			}
			Table tabel = atomicExpResolver(atomichelp);
					bool found = false;
					for (int x = 0; x < allTables.size(); x++)
					{
						if (allTables[x].getTableName() == tabel.getTableName())
						{
							found = true;
							tableloc = x;
							break;
						}
					}
					if (!found)
					{
						allTables.push_back(tabel);
						tableloc = allTables.size() - 1;
					}


					Attribute a(allTables[tableloc].attrNameAt(0), allTables[tableloc].attrTypeAt(0), allTables[tableloc].attrKeyAt(0));
					Table returnTable=renameAttr(allTables[tableloc], a, newnames[0]);
					for (int x = 1; x < newnames.size(); x++)
					{
						Attribute a(allTables[tableloc].attrNameAt(x), allTables[tableloc].attrTypeAt(x), allTables[tableloc].attrKeyAt(x));
						returnTable.setAttrNameAt(x, newnames[x]);
					}
					return returnTable;
		}
	}
	else
	{
		cout << "ERROR Rename parser first token expected \"(\". Returning dummy table" << endl;
		Attribute a("dummmy", "int");
		vector <Attribute> aaa;
		Table dummy(aaa, "dummy");
		return dummy;
	}
}

void parser_Write(Table _table){
	vector<string> commandList;
	string table_name = _table.name;
	commandList = _table.commandHistory;
	ofstream output_file;
	output_file.open(table_name + ".db");
	for (int x = 0; x < commandList.size(); x++){
		output_file << commandList[x] << endl;
	}
	output_file.close();
}
void parser_Close(Table _table, int index){
	parser_Write(_table);
	allTables.erase(allTables.begin() + index);
}
void parser_Open(string _name){
	string table_name = _name;
	vector<string> tab_in;
	string file_name = table_name + ".db";
	ifstream my_file;
	my_file.open(file_name);
	string line;
	int x = 0;
	bool done = false;
	if (my_file.is_open()){
		while ((getline(my_file, line) && (done == false))){
			if (x > 0 && line.find("CREATE TABLE") != string::npos){
				done = true;
			}
			if (done == false){
				tab_in.push_back(line);
			}
			x++;
		}
	}
	inputDecoder(tab_in);
	
}

void addToCommandHistory(string tableName, string cmd){
	bool isTable = false;
	int tableIndex = 0;
	for (int i = 0; i < allTables.size(); i++)
	{
		if (allTables[i].getTableName() == tableName)
		{
			//token is a table
			isTable = true;
			tableIndex = i;
			break;
		}
	}
	if (isTable == true)
	{
		allTables[tableIndex].pushBackCommand(cmd);
	}
	else
	{
		cout << "addToCommandHistory ERROR: no such table" << endl;
		return;
	}
}



