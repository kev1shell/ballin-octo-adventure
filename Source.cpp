#include "stdafx.h"
#include "dbmsFunctions.h"


using namespace dbmsFunctions;

#include <stdio.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <iostream>


//notes for parser_CREATETABLE.
#define identifier (1)
#define number (2)
#define op (3)				// < (when not used as an arrow), >, &, |, +, - (when not used as an arrow), *, !, =
#define punc (4)			// < (when not used as an arrow), comma, (, ), - (when not used as an arrow), ;
#define quotes (5)

//testing booleans
#define testing (false)		//used for testing tokenizer function
#define testingMain (true)	//used for testing main function
#define testingParserCreateTable (false)

using namespace std;


//TABLES STORED IN VECTOR
vector <Table> allTables;

struct Token {
	string content;

	int type;   // type can be: identifier, number, op (!, <, >, etc.), punc, quotes

	Token(string _content, int _type)
	{
		content = _content;
		type = _type;
	}


};

//*********************************************************************TEST INPUTS********************************************************

//string testString = "dogs <- select (kind == \"dog\") animals;";


//*********************************************************************FUNCTION HEADERS***************************************************
vector<Token> tokenizer(int startLoc, string s);
void parser_CreateTable(vector <Token> cmd);

Table parser_select(vector <Token> cmd);
Table parser_project(vector <Token> cmd);
Table parser_rename(vector <Token> cmd);
Table parser_union(vector <Token> cmd);
Table parser_difference(vector <Token> cmd);
Table parser_cross_product(vector <Token> cmd);
Table parser_natural_join(vector <Token> cmd);

//atomic expression resolver
Table atomicExpResolver(vector<Token> tokenList);

int main()
{
	
	Attribute user_id("userID", "int");
	Attribute user_name("userName", "string");
	Attribute privilege_level("privilegeLevel", "string");

	vector<Attribute> attributeList = { user_id, user_name, privilege_level };

	Table tablea(attributeList, "tableA");
	Table tableb(attributeList, "tableB");

	vector<string> newRow = { "0", "Bobby", "admin" };
	vector<string> newer = { "1", "Will", "guest" };

	vector<string> newRow2 = { "3", "Bobby", "admin" };
	vector<string> newer2 = { "2", "Will", "guest" };

	tablea.pushBackRow(newRow);
	tablea.pushBackRow(newer);

	tableb.pushBackRow(newRow2);
	tableb.pushBackRow(newer2);

	Table intersect = intersection(tablea, tableb);

	allTables.push_back(tablea);
	allTables.push_back(tableb);
	
	string testString = "CREATE TABLE animals (name VARCHAR(20), kind VARCHAR(8), years INTEGER) PRIMARY KEY (name, kind);";
	string testString2 = "SHOW (animals JOIN animals) + animals;";
	string testString3 = "result <- select (userName != \"Bobby\" && userID > 0) (select (userID > 1) tableB + tableA);";

	vector <string> testStrings = {testString, testString3};

	for (int iter = 0; iter < testStrings.size(); iter++)
	{
		
		//if the input is a query (idk how to spell tht) it includes a "<-" in it.
		size_t functionType = testStrings[iter].find("<-");
		if (functionType != string::npos)
		{
			//if "<-" is found go thru query calls (e.g. select, project, etc.)
			if (testStrings[iter].substr(functionType + 3, 6) == "select")
			{
				vector <Token> select;
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
				cout << "test" << endl;
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

					for (int i = 0; i < showTable.size()-1; i++)
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
		}

	}
	
	return 0;
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
	vector<Attribute> attributeList = {dummy};
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
									Table tableA(table1->getAttributes(), table1->getTableName());
									Table tableB(table2->getAttributes(), table2->getTableName());
									
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

							if (tokenList[currentToken+2].type == identifier)
							{
								//token is a table name
								endIndex = currentToken;
							}
							else
							{
								if (tokenList[currentToken+2].content == "(")
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
							Table tableA(table1->getAttributes(), table1->getTableName());
							Table tableB(table2->getAttributes(), table2->getTableName());

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
								Table tableA(table1->getAttributes(), table1->getTableName());
								Table tableB(table2->getAttributes(), table2->getTableName());

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
									Attribute attribute(name,type,keyType);
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
									Table tableA(table1->getAttributes(), table1->getTableName());
									Table tableB(table2->getAttributes(), table2->getTableName());

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
				int startIndex = currentToken+1;
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
				currentToken = lastClosedParenIndex+1;

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
								Table tableA(table1->getAttributes(), table1->getTableName());
								Table tableB(table2->getAttributes(), table2->getTableName());

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
					else if (tokenList[currentToken+2].content == "select") //if first token is an identifier but not a table;
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
						Table tableA(table1->getAttributes(), table1->getTableName());
						Table tableB(table2->getAttributes(), table2->getTableName());

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
							Table tableA(table1->getAttributes(), table1->getTableName());
							Table tableB(table2->getAttributes(), table2->getTableName());

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
								Table tableA(table1->getAttributes(), table1->getTableName());
								Table tableB(table2->getAttributes(), table2->getTableName());

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
				Table tableA(table1->getAttributes(), table1->getTableName());
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
							Table tableA(table1->getAttributes(), table1->getTableName());
							Table tableB(table2->getAttributes(), table2->getTableName());

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
								Table tableA(table1->getAttributes(), table1->getTableName());
								Table tableB(table2->getAttributes(), table2->getTableName());

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
							Table tableA(table1->getAttributes(), table1->getTableName());
							Table tableB(table2->getAttributes(), table2->getTableName());

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
								Table tableA(table1->getAttributes(), table1->getTableName());
								Table tableB(table2->getAttributes(), table2->getTableName());

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
	//BEGIN DUMMY IMPLEMENTATION
	Attribute dummy("dummy", "string", "primary key");
	vector <Attribute> attributeList = { dummy };
	Table result(attributeList, "result");
	return result;

	//END DUMMY IMPLEMENTATION
}

Table parser_rename(vector <Token> cmd)
{
	//BEGIN DUMMY IMPLEMENTATION
	Attribute dummy("dummy", "string", "primary key");
	vector <Attribute> attributeList = { dummy };
	Table result(attributeList, "result");
	return result;

	//END DUMMY IMPLEMENTATION
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

Table parser_cross_product(vector <Token> cmd)
{
	//BEGIN DUMMY IMPLEMENTATION
	Attribute dummy("dummy", "string", "primary key");
	vector <Attribute> attributeList = { dummy };
	Table result(attributeList, "result");
	return result;

	//END DUMMY IMPLEMENTATION
}

Table parser_natural_join(vector <Token> cmd)
{
	//BEGIN DUMMY IMPLEMENTATION
	Attribute dummy("dummy", "string", "primary key");
	vector <Attribute> attributeList = { dummy };
	Table result(attributeList, "result");
	return result;

	//END DUMMY IMPLEMENTATION
}
