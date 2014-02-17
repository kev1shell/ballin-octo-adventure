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

int main()
{
	string testString = "CREATE TABLE animals (name VARCHAR(20), kind VARCHAR(8), years INTEGER) PRIMARY KEY (name, kind);";
	string testString2 = "SHOW animals;";

	vector <string> testStrings = {testString, testString2};

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
				select = tokenizer(0, testStrings[iter]);
				if (testingMain)
				{
					for (int x = 0; x < select.size(); x++)
					{
						cout << select[x].content;
					}
					cout << endl;
				}
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

				for (int i = 0; i < showTable.size(); i++)
				{
					if (showTable[i].content == ";")
					{
						break;
					}
					else if (showTable[i].type == identifier)
					{
						bool isTableName = false;
						for (int j = 0; j < allTables.size(); j++)
						{
							if (allTables[j].getTableName() == showTable[i].content)
							{
								//then identifer corresponds to a table in allTables. print this table.
								isTableName = true;
								allTables[j].printTable(allTables[j]);
								break;
							}
						}

						if (isTableName == false)
						{
							//check to see if identifer is a querry
							
							if (i + 1 < showTable.size())
							{
								vector <Token> tokensVect;

								for (int k = i + 1; k < showTable.size(); k++)
								{
									tokensVect.push_back(showTable[k]);
								}

								if (showTable[i].content == "select")
								{
									Table result = parser_select(tokensVect);
									result.printTable(result);
									break;
								}
								else if (showTable[i].content == "project")
								{
									Table result = parser_project(tokensVect);
									result.printTable(result);
									break;
								}
								else if (showTable[i].content == "rename")
								{
									Table result = parser_rename(tokensVect);
									result.printTable(result);
									break;
								}
								else if (showTable[i].content == "union")
								{
									Table result = parser_union(tokensVect);
									result.printTable(result);
									break;
								}
								else if (showTable[i].content == "difference")
								{
									Table result = parser_difference(tokensVect);
									result.printTable(result);
									break;
								}
								else if (showTable[i].content == "natural")
								{
									if (i + 2 < showTable.size())
									{
										if (showTable[i + 1].content == "join")
										{
											//remove first element from tokensVect
											tokensVect.erase(tokensVect.begin());
											Table result = parser_select(tokensVect);

											result.printTable(result);
											break;
										}
									}
								}

							}
						}

					}
					else if (showTable[i].type == number){}
					else if (showTable[i].type == op){}
					else if (showTable[i].type == punc){}
					else if (showTable[i].type == quotes){}
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


//parser functions
Table parser_select(vector <Token> cmd)
{
	//BEGIN DUMMY IMPLEMENTATION
	Attribute dummy("dummy", "string", "primary key");
	vector <Attribute> attributeList = {dummy};
	Table result(attributeList, "result");
	return result;

	//END DUMMY IMPLEMENTATION
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
