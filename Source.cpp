
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
void customerMenu(int customerID);
void manufacturerMenu();
void welcome();
void logIn();
void registerUser();
void browseProducts(int customerID);
void viewCart(int customerID);
void checkout(int customerID);
void logInCustomer();
void logInManufacturer();
void new_user(int choice);
void manuOptions(int _manuID);
void addProduct(int _manuID);
void removeProduct(int _manuID);
void browseManuProducts(int _manuID);
void editInventory(int _manuID);
void viewInventory(int _manuID);

int main()
{	
	vector<string> startCommands;
	startCommands.push_back("OPEN customer;");
	startCommands.push_back("OPEN manufacturer;");
	startCommands.push_back("OPEN product;");
	startCommands.push_back("OPEN order;");
	startCommands.push_back("OPEN orderDetail;");
	startCommands.push_back("OPEN inventory;");

	inputDecoder(startCommands);

	welcome();
	return 0;
}
//Menu Functions
void welcome(){
	
	system("cls");
	cout << "welcome to Bazaar 1.0" << endl;
	cout << "Press 1 to access our menus as a customer" << endl;
	cout << "press 2 to access our menus as a manufacturer" << endl;
	cout << "press 3 to quit Bazaar" << endl;
	cin >> userType;
	if (userType == 1){
		logInCustomer();
	}
	else if (userType == 2){
		logInManufacturer();
	}
	else if (userType == 3){
		vector<string> startCommands;
		startCommands.push_back("WRITE customer;");
		startCommands.push_back("WRITE manufacturer;");
		startCommands.push_back("WRITE product;");
		startCommands.push_back("WRITE order;");
		startCommands.push_back("WRITE orderDetail;");
		startCommands.push_back("WRITE inventory;");

		inputDecoder(startCommands);
		exit(EXIT_SUCCESS);
	}
}
void logIn(){
	cout << "log in" << endl;
}
void registerUser(){
	cout << "register user" << endl;
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
				addToCommandHistory("inventory", testStrings[iter]);
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
					if (cmd[6].content == allTables[tableloc].attrNameAt(x))
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
	//parser_Write(_table);
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

void customerMenu(int customerID)
{
	int option = 0;

	system("cls");
	cout << "Cutomer Menu" << endl;
	cout << endl;
	cout << "Enter the number of the option you wish to choose" << endl;
	cout << endl;
	cout << "1.  Browse Products" << endl;
	cout << "2.  logout" << endl;
	cout << endl;
	cout << ">";
	if (cin >> option)
	{
		switch (option)
		{
		case 1:
			{
				  browseProducts(customerID);
				  break;
			}
		case 2:
			{
				  welcome();
				  break;
			}
		}
	}
	else
	{
		//error case
		std::cin.clear();
		while (std::cin.get() != '\n')
		{
			continue;
		}
		cout << endl;
		cout << "Oops! your input was not recognised" << endl;
		cout << "To select an option, enter the number" << endl;
		cout << "of the option you wish to choose and " << endl;
		cout << "press enter." << endl;
		cout << endl;
		cout << "" << endl;
		system("PAUSE");
		customerMenu(customerID);
	}
}

void browseProducts(int customerID)
{
	
	int cartIndex = 0;
	for (int i = 0; i < allTables.size(); i++)
	{
		if (allTables[i].getTableName() == "cart")
		{
			cartIndex = i;
		}
	}
	
	int cartSize = 0;
	Table products = allTables[2];
	vector<string> LastProduct = products.getRows()[products.getRows().size()-1];

	int maxId = atoi(LastProduct[1].c_str());
	
	system("cls");
	cout << "Product Browser" << endl;
	cout << endl;
	int option = 0;
	vector<string> commands = { "SHOW product;" };
	inputDecoder(commands);

	cout << endl;
	
	if (cartIndex != 0)
	{
		vector<Token> tokenList = tokenizer(0, "cart;");
		tokenList.erase(tokenList.begin() + 1);
		cartSize = atomicExpResolver(tokenList).getNumRows();
	}

	cout << "Items in your cart: " << cartSize << endl;
	cout << endl;
	cout << "Enter an option number, or the ID of the product you wish to add to your cart" << endl;
	cout << endl;
	cout << (maxId + 1) << ".  View Cart" << endl;
	cout << (maxId + 2) << ".  Back to Customer Menu" << endl;
	cout << (maxId + 3) << ".  logout" << endl;
	cout << endl;
	cout << ">";
	if (cin >> option)
	{
		if (option == maxId + 1)
		{
			if (cartSize > 0)
			{
				viewCart(customerID);
			}
			else
			{
				cout << endl;
				cout << "Sorry your cart is empty, please add items to your cart before checkout" << endl;
				cout << endl;
				system("PAUSE");
				browseProducts(customerID);
			}
		}
		else if (option == maxId + 2)
		{
				  customerMenu(customerID);
		}
		else if (option == maxId + 3)
		{
				  welcome();
		}
		else
		{
				   string itemID = int2string(option);

				   if (cartIndex == 0)
				   {
					   string qry1 = "CREATE TABLE cart (productName VARCHAR(20), productID INTEGER, manufacturerID INTEGER, price INTEGER, quantity INTEGER) PRIMARY KEY (productID);";
					   inputDecoder(vector<string> {qry1});
				   }
				   
				   Table result = select(allTables[cartIndex], "productID", "==", itemID);
				   if (result.getRows().size() > 0)
				   {
					   cout << endl;
					   cout << "This item is already in your cart." << endl;
					   cout << endl;
					   system("PAUSE");
					   browseProducts(customerID);
						
				   }

				   Table itemTable = select(allTables[2], "productID", "==", itemID);
				   vector<string> item = itemTable.getRows()[0];
				   int quantity = 0;
				   
				   system("cls");
				   cout << "Enter quantity" << endl;
				   cout << endl;
				   cout << ">";
				   if (!(cin >> quantity))
				   {
					   //error case
					   std::cin.clear();
					   while (std::cin.get() != '\n')
					   {
						   continue;
					   }
					   cout << endl;
					   cout << "Oops! your input was not recognised" << endl;
					   cout << endl;
					   cout << "" << endl;
					   system("PAUSE");
					   customerMenu(customerID);
				   }

				   string qry2 = "INSERT INTO cart VALUES FROM(\""+item[0]+"\", "+item[1]+", "+item[2]+", "+item[3]+", " + int2string(quantity) + ")"+";";
				   inputDecoder(vector<string> {qry2});
				  
				   system("cls");
				   cout << "product \"" << itemTable.getRows()[0][0] << "\" has been added to your cart." << endl;
				   cout << "quantity: " << quantity << endl;
				   cout << endl;
				   system("PAUSE");

				   browseProducts(customerID);

		}
	}
	else
	{
		//error case
		std::cin.clear();
		while (std::cin.get() != '\n')
		{
			continue;
		}
		cout << endl;
		cout << "Oops! your input was not recognised" << endl;
		cout << "To select an option, enter the number" << endl;
		cout << "of the option you wish to choose and " << endl;
		cout << "press enter." << endl;
		cout << endl;
		cout << "" << endl;
		system("PAUSE");
		customerMenu(customerID);
	}
}

void viewCart(int customerID)
{
	int option = 0;

	int cartIndex = 0;
	for (int i = 0; i < allTables.size(); i++)
	{
		if (allTables[i].getTableName() == "cart")
		{
			cartIndex = i;
		}
	}

	system("cls");
	cout << "Items in Cart" << endl;
	cout << endl;
	string qry = "SHOW cart;";
	inputDecoder(vector<string> {qry});
	cout << endl;
	cout << "Enter the number of the option you wish to choose" << endl;
	cout << endl;
	cout << "1.  Continue to Checkout" << endl;
	cout << "2.  Clear Cart" << endl;
	cout << "3.  Back to Product Browser" << endl;
	cout << "4.  logout" << endl;
	cout << endl;
	cout << ">";
	if (cin >> option)
	{
		switch (option)
		{
		case 1:
		{
				  checkout(customerID);
				  break;
		}
		case 2:
		{
				  allTables.erase(allTables.begin() + cartIndex);
				  system("cls");
				  cout << "Cart cleared." << endl;
				  cout << endl;
				  system("PAUSE");
				  browseProducts(customerID);
				  break;
		}
		case 3:
		{
				  browseProducts(customerID);
				  break;
		}
		case 4:
		{
				  welcome();
				  break;
		}
		}
	}
	else
	{
		//error case
		std::cin.clear();
		while (std::cin.get() != '\n')
		{
			continue;
		}
		cout << endl;
		cout << "Oops! your input was not recognised" << endl;
		cout << "To select an option, enter the number" << endl;
		cout << "of the option you wish to choose and " << endl;
		cout << "press enter." << endl;
		cout << endl;
		cout << "" << endl;
		system("PAUSE");
		customerMenu(customerID);
	}
}

void checkout(int customerID)
{
	
	int option = 0;

	system("cls");
	cout << "Checkout Menu" << endl;
	cout << endl;
	cout << "Enter the number of the option you wish to choose" << endl;
	cout << endl;
	cout << "1.  Purchase Items" << endl;
	cout << "2.  Back to Cart" << endl;
	cout << "3.  logout" << endl;
	cout << endl;
	cout << ">";
	if (cin >> option)
	{
		switch (option)
		{
		
		case 1:
		{
				  
				int cartIndex = 0;
				for (int i = 0; i < allTables.size(); i++)
				{
					if (allTables[i].getTableName() == "cart")
					{
						cartIndex = i;
					}
				}

				int inventoryIndex = 0;
				for (int i = 0; i < allTables.size(); i++)
				{
					if (allTables[i].getTableName() == "inventory")
					{
						inventoryIndex = i;
					}
				}

				bool suffiecientInventory = true;
				vector<int> inventoryIndecies;
				vector<int> inventoryToRemove;
				for (int i = 0; i < allTables[cartIndex].rows.size(); i++)
				{
					for (int k = 0; k < allTables[inventoryIndex].rows.size(); k++)
					{
						if (allTables[cartIndex].rows[i][1] == allTables[inventoryIndex].rows[k][1])
						{
							if (atoi(allTables[cartIndex].rows[i][4].c_str()) > atoi(allTables[inventoryIndex].rows[k][3].c_str()))
							{
								suffiecientInventory = false;
							}
							inventoryIndecies.push_back(k);
							inventoryToRemove.push_back(atoi(allTables[cartIndex].rows[i][4].c_str()));
						}
					}
				}

				if (suffiecientInventory == false)
				{
					system("cls");
					cout << "Insufficient inventory" << endl;
					cout << endl;

					allTables.erase(allTables.begin() + cartIndex);
					system("PAUSE");
					browseProducts(customerID);
				}
				else
				{
					for (int i = 0; i < inventoryIndecies.size(); i++)
					{
						int oldInventory = atoi(allTables[inventoryIndex].rows[i][3].c_str());
						int newInventory = oldInventory - inventoryToRemove[i];
						string productID = allTables[inventoryIndex].rows[i][1];
						string qry = "UPDATE inventory SET quantity = " + int2string(newInventory) + " WHERE productID == " + productID + " ;";
						vector<string> cmd = { qry };
						inputDecoder(cmd);
						//updateValue(allTables[inventoryIndex], productID, allTables[5].attributes[3], int2string(newInventory));
					}
					
					


					allTables.erase(allTables.begin() + cartIndex);
					system("cls");
					cout << "Items purchased, thank you." << endl;
					system("PAUSE");
					browseProducts(customerID);
				}

		}
		case 2:
		{
				  viewCart(customerID);
				  break;
		}
		case 3:
		{
				  welcome();
				  break;
		}
		}
	}
	else
	{
		//error case
		std::cin.clear();
		while (std::cin.get() != '\n')
		{
			continue;
		}
		cout << endl;
		cout << "Oops! your input was not recognised" << endl;
		cout << "To select an option, enter the number" << endl;
		cout << "of the option you wish to choose and " << endl;
		cout << "press enter." << endl;
		cout << endl;
		cout << "" << endl;
		system("PAUSE");
		customerMenu(customerID);
	}
	
	
	Table customerTable = select(allTables[0], "customerId", "==", int2string(customerID));
	vector<string> customer = customerTable.getRows()[0];
	
	string orderedFrom = customer[2];
	string orderID = "0";
	if (allTables[3].getRows().size() > 0)
	{
		vector<vector<string>> rows = allTables[3].getRows();
		vector<string> lastRow = rows[rows.size() - 1];
		int maxID = atoi(lastRow[1].c_str()) + 1;
		orderID = int2string(maxID);
	}

	string qry = "INSERT INTO order VALUES FROM(\"" + orderedFrom + "\", " + orderID + ", "+ int2string(customerID) +");";
	inputDecoder(vector<string> {qry});

	Table cart = allTables[5];
	vector<vector<string>> rows = cart.getRows();
	for (int i = 0; i < rows.size(); i++)
	{
		string orderDetailID = "0";
		if (allTables[4].getRows().size() > 0)
		{
			vector<vector<string>> rowsy = allTables[4].getRows();
			vector<string> lastRowy = rowsy[rows.size() - 1];
			int maxIDy = atoi(lastRowy[1].c_str()) + 1;
			orderDetailID = int2string(maxIDy);
		}
		
		string qry = "INSERT INTO orderDetail VALUES FROM(\"" + customer[2] + "\", " + orderDetailID + ", " + orderID + ", " + rows[i][2] + ", " + rows[i][4] + ", " + int2string(customerID) + ");";
		inputDecoder(vector<string> {qry});
	}




}

void logInCustomer()
{
	int option = 0;

	system("cls");
	cout << "Cutomer Log in" << endl;
	cout << endl;
	cout << "Enter the number of the option you wish to choose" << endl;
	cout << endl;
	cout << "1.  Existing user" << endl;
	cout << "2.  New user" << endl;
	cout << "3.  Back to Welcome Menu" << endl;
	cout << endl;
	cout << ">";
	if (cin >> option)
	{
		switch (option)
		{
		case 1:
		{
				  string customerName = "";
				  string password = "";
				  system("cls");
				  cout << "Enter customerName:" << endl;
				  cout << ">";
				  if (cin >> customerName)
				  {
					  cout << endl;
					  cout << "Enter password" << endl;
					  cin >> password;
					  Table result = select(allTables[0], "customerName", "==", customerName);
					  if (result.rows.size() > 0)
					  {
						  if (password == result.getRows()[0][3])
						  {
							  int customerID = atoi(result.getRows()[0][1].c_str());
							  customerMenu(customerID);
						  }
						  else
						  {
							  cout << endl;
							  cout << "Customer name or password incorrect" << endl;
							  cout << endl;
							  system("PAUSE");
							  logInCustomer();
						  }
					  }
					  else
					  {
						  cout << endl;
						  cout << "Customer name or password incorrect" << endl;
						  cout << endl;
						  system("PAUSE");
						  logInCustomer();

					  }
				  }
				  else
				  {
					  //error case
					  std::cin.clear();
					  while (std::cin.get() != '\n')
					  {
						  continue;
					  }
					  cout << endl;
					  cout << "Oops! your input was not recognised" << endl;
					  cout << endl;
					  system("PAUSE");
					  logInCustomer();
				  }
				  break;
		}
		case 2:
		{
				  new_user(1);
				  break;
		}
		case 3:
		{
				  welcome();
				  break;
		}
		}
	}
	else
	{
		//error case
		std::cin.clear();
		while (std::cin.get() != '\n')
		{
			continue;
		}
		cout << endl;
		cout << "Oops! your input was not recognised" << endl;
		cout << "To select an option, enter the number" << endl;
		cout << "of the option you wish to choose and " << endl;
		cout << "press enter." << endl;
		cout << endl;
		cout << "" << endl;
		system("PAUSE");
		logInCustomer();
	}
}

void logInManufacturer()
{
	int option = 0;

	system("cls");
	cout << "Manufacturer Log in" << endl;
	cout << endl;
	cout << "Enter the number of the option you wish to choose" << endl;
	cout << endl;
	cout << "1.  Existing user" << endl;
	cout << "2.  New user" << endl;
	cout << "3.  Back to Welcome Menu" << endl;
	cout << endl;
	cout << ">";
	if (cin >> option)
	{
		switch (option)
		{
		case 1:
		{
				  string manufacturerName = "";
				  string password = "";
				  system("cls");
				  cout << "Enter manufacturerName:" << endl;
				  cout << ">";
				  if (cin >> manufacturerName)
				  {
					  cout << endl;
					  cout << "Enter password" << endl;
					  cin >> password;
					  Table result = select(allTables[1], "userName", "==", manufacturerName);
					  if (result.rows.size() > 0)
					  {
						  if (password == result.getRows()[0][2])
						  {
							  int manufacturerID = atoi(result.getRows()[0][1].c_str());
							  manuOptions(manufacturerID);
						  }
						  else
						  {
							  cout << endl;
							  cout << "Manufacturer name or password incorrect" << endl;
							  cout << endl;
							  system("PAUSE");
							  logInManufacturer();
						  }
					  }
					  else
					  {
						  cout << endl;
						  cout << "Manufacturer name or password incorrect" << endl;
						  cout << endl;
						  system("PAUSE");
						  logInManufacturer();

					  }
				  }
				  else
				  {
					  //error case
					  std::cin.clear();
					  while (std::cin.get() != '\n')
					  {
						  continue;
					  }
					  cout << endl;
					  cout << "Oops! your input was not recognised" << endl;
					  cout << endl;
					  system("PAUSE");
					  logInManufacturer();
				  }
				  break;
		}
		case 2:
		{
				  new_user(2);
				  break;
		}
		case 3:
		{
				  welcome();
				  break;
		}
		}
	}
	else
	{
		//error case
		std::cin.clear();
		while (std::cin.get() != '\n')
		{
			continue;
		}
		cout << endl;
		cout << "Oops! your input was not recognised" << endl;
		cout << "To select an option, enter the number" << endl;
		cout << "of the option you wish to choose and " << endl;
		cout << "press enter." << endl;
		cout << endl;
		cout << "" << endl;
		system("PAUSE");
		logInCustomer();
	}
}

void new_user(int choice)
{
	system("cls");

	if (choice == 1)
	{
		string user_name;
		string password;
		string state_live_in;
		cout << "What is the customers username:" << endl;
		cin >> user_name;
		cout << "What is the customers password:" << endl;
		cin >> password;
		cout << "What state does the customer live in:" << endl;
		cin >> state_live_in;
		cout << "The customers username is  " << user_name << " The customers password is  " << password << " The customers lives in  " << state_live_in << endl;
			
		string maxID = "0";
		maxID = allTables[0].getRows()[allTables[0].getRows().size() - 1][1];
		int customerID = atoi(maxID.c_str()) + 1;

		string qry2 = "INSERT INTO customer VALUES FROM(\"" + user_name + "\", " + int2string(customerID) + ", \"" + state_live_in + "\", \"" + password + "\")" + ";";
		inputDecoder(vector<string> {qry2});

		system("cls");
		cout << "Congrats! you are now registered!" << endl;
		cout << endl;
		system("PAUSE");
		customerMenu(customerID);

	}
	else if (choice == 2)
	{
		string user_name;
		string password;
		cout << "What is the manufacturer username:" << endl;
		cin >> user_name;
		cout << "What is the manufacturer password:" << endl;
		cin >> password;
		cout << "The manufacturer username is  " << user_name << " The manufacturer password is  " << password << endl;

		string maxID = "0";
		maxID = allTables[1].getRows()[allTables[1].getRows().size() - 1][1];
		int manufacturerID = atoi(maxID.c_str()) + 1;

		string qry2 = "INSERT INTO manufacturer VALUES FROM(\"" + user_name + "\", " + int2string(manufacturerID) + ", \"" + password + "\")" + ";";
		inputDecoder(vector<string> {qry2});

		system("cls");
		cout << "Congrats! you are now registered!" << endl;
		cout << endl;
		system("PAUSE");
		manuOptions(manufacturerID);

	}
	else if (choice != 1 && choice != 2)
	{
		choice = 0;
	}
}


void addProduct(int _manuID){
	system("cls");
	string prodName;
	string quant;

	string productID = "123";
	int maxID = 0;
	maxID = atoi(allTables[2].getRows()[allTables[2].getRows().size() - 1][1].c_str());
	string newID = int2string(maxID + 1);
	productID = newID;

	string price;
	string manu_id = int2string(_manuID);
	cout << "Please input the name of the product " << endl;
	cin >> prodName;
	cout << "Please input the price you wish to sell it at" << endl;
	cin >> price;
	cout << "How many do you have on hand" << endl;
	cin >> quant;



	string cmd = "INSERT INTO product VALUES FROM(\"" + prodName + "\", " + productID + ", " + manu_id + ", " + price + ");";
	string cmd1 = "INSERT INTO inventory VALUES FROM(\"" + prodName + "\"," + productID + ", " + manu_id + ", " + quant + ");";
	vector<string> thisString = { cmd, cmd1 };
	thisString.push_back("WRITE product;");
	thisString.push_back("WRITE inventory;");
	inputDecoder(thisString);
	system("cls");
	cout << "please select an option: " << endl;
	cout << "1.  add Another Product" << endl;
	cout << "2.  Browse Products" << endl;
	cout << "3.  log out" << endl;
	int choice = 0;
	if (cin >> choice){
		switch (choice){
		case 1:
		{
				  addProduct(_manuID);
		}
		case 2:
		{
				  browseManuProducts(_manuID);
		}
		case 3:
		{
				  welcome();
		}
		}


	}
	else{
		cout << "input not recognized" << endl;
		manuOptions(_manuID);
	}
}

void editInventory(int _manuID){
	int pid;
	int quant;
	system("cls");
	cout << "please input ID of the product you would like to update inventory quantity of" << endl;
	cin >> pid;
	cout << "please input the new quantity" << endl;
	cin >> quant;
	string manu_id = int2string(_manuID);
	string id = int2string(pid);
	string quants = int2string(quant);
	vector<string> cmds;
	//updateValue(allTables[5], id, allTables[5].attributes[3], quants);
	cmds.push_back("UPDATE inventory SET quantity = " + quants + " WHERE productID == " + id + ";");
	cmds.push_back("WRITE inventory;");
	inputDecoder(cmds);

	manuOptions(_manuID);

}

void browseManuProducts(int _manuID){
	system("cls");
	int choose;
	vector<string> startCommands;
	startCommands.push_back("WRITE customer;");
	startCommands.push_back("WRITE manufacturer;");
	startCommands.push_back("WRITE product;");
	startCommands.push_back("WRITE order;");
	startCommands.push_back("WRITE orderDetail;");
	startCommands.push_back("WRITE inventory;");

	startCommands.push_back("CLOSE customer;");
	startCommands.push_back("CLOSE manufacturer;");
	startCommands.push_back("CLOSE product;");
	startCommands.push_back("CLOSE order;");
	startCommands.push_back("CLOSE orderDetail;");
	startCommands.push_back("CLOSE inventory;");

	startCommands.push_back("OPEN customer;");
	startCommands.push_back("OPEN manufacturer;");
	startCommands.push_back("OPEN product;");
	startCommands.push_back("OPEN order;");
	startCommands.push_back("OPEN orderDetail;");
	startCommands.push_back("OPEN inventory;");

	inputDecoder(startCommands);
	system("cls");
	vector<string> decode;
	decode.push_back("SHOW product;");
	inputDecoder(decode);
	cout << "press 1 to return to manufacturer menu" << endl;
	cout << "press 2 to log out" << endl;
	cin >> choose;
	if (choose == 1){
		manuOptions(_manuID);
	}
	if (choose == 2){
		welcome();
	}
}
void removeProduct(int _manuID){
	system("cls");
	string prodName;
	string manu_id = int2string(_manuID);
	cout << "Please input the name of the product you wish to remove " << endl;
	cin >> prodName;

	string cmd = "DELETE FROM product WHERE(\"manufacturerID ==" + manu_id + " && productName ==\"" + prodName + "\");";
	vector<string> thisString = { cmd };
	thisString.push_back("WRITE product;");
	inputDecoder(thisString);
	system("cls");
	cout << "please select an option: " << endl;
	cout << "1.  add Another Product" << endl;
	cout << "2.  Browse Products" << endl;
	cout << "3.  log out" << endl;
	int choice = 0;
	if (cin >> choice){
		switch (choice){
		case 1:
		{
				  addProduct(_manuID);
		}
		case 2:
		{
				  browseManuProducts(_manuID);
		}
		case 3:
		{
				  welcome();
		}
		}


	}
	else{
		cout << "input not recognized" << endl;
		manuOptions(_manuID);
	}
}

void viewInventory(int _manuID){
	system("cls");
	string id = int2string(_manuID);
	int blah = 0;
	string c1 = "inven <- project(productName, quantity)(select(manufacturerID == " + id + ") inventory);";
	vector<string> cmd;
	vector<string> startCommands;
	startCommands.push_back("WRITE customer;");
	startCommands.push_back("WRITE manufacturer;");
	startCommands.push_back("WRITE product;");
	startCommands.push_back("WRITE order;");
	startCommands.push_back("WRITE orderDetail;");
	startCommands.push_back("WRITE inventory;");

	startCommands.push_back("CLOSE customer;");
	startCommands.push_back("CLOSE manufacturer;");
	startCommands.push_back("CLOSE product;");
	startCommands.push_back("CLOSE order;");
	startCommands.push_back("CLOSE orderDetail;");
	startCommands.push_back("CLOSE inventory;");

	startCommands.push_back("OPEN customer;");
	startCommands.push_back("OPEN manufacturer;");
	startCommands.push_back("OPEN product;");
	startCommands.push_back("OPEN order;");
	startCommands.push_back("OPEN orderDetail;");
	startCommands.push_back("OPEN inventory;");
	cmd.push_back(c1);
	//	cmd.push_back("SHOW inven;");
	inputDecoder(cmd);
	cout << "press 1 to return to manufacturer options" << endl;
	cout << "press 2 to log out" << endl;
	cin >> blah;
	cmd.clear();
	cmd.push_back("CLOSE inven;");
	inputDecoder(cmd);
	if (blah == 1) {
		manuOptions(_manuID);
	}
	else if (blah == 2){
		welcome();
	}
	else {
		cout << "error in input" << endl;

	}



}

void manuOptions(int _manuID){
	system("cls");
	int choice = 0;
	cout << "Manufacturer Menu" << endl << endl;
	cout << "please input the number of the option you wish to choose " << endl;
	cout << "1.  Add a product" << endl;
	cout << "2.  Remove a product" << endl;
	cout << "3.  Browse Products" << endl;
	cout << "4.  Edit Inventory" << endl;
	cout << "5.  View Inventory" << endl;
	cout << "6.  Log Out" << endl;
	if (cin >> choice)
	{
		switch (choice)
		{
		case 1:
		{
				  addProduct(_manuID);
				  break;
		}
		case 2:
		{
				  removeProduct(_manuID);
				  break;
		}
		case 3:
		{
				  browseManuProducts(_manuID);
				  break;
		}
		case 4:
		{
				  editInventory(_manuID);
				  break;
		}
		case 5:
		{
				  viewInventory(_manuID);
				  break;
		}
		case 6:
		{
				  welcome();
				  break;
		}
		}
	}
	else
	{
		//error case
		std::cin.clear();
		while (std::cin.get() != '\n')
		{
			continue;
		}
		cout << endl;
		cout << "Oops! your input was not recognised" << endl;
		cout << "To select an option, enter the number" << endl;
		cout << "of the option you wish to choose and " << endl;
		cout << "press enter." << endl;
		cout << endl;
		cout << "" << endl;
		system("PAUSE");
		manuOptions(_manuID);
	}


}