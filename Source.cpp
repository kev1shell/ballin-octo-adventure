#include "stdafx.h"
#include "dbmsFunctions.h"
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
#define testingMain (true)	//used for testing main function
#define testingParserCreateTable (false)
#define testingParserInsertInto (false)
#define testingParserDelete (false)
#define testingUpdateValue (true)

using namespace std;


struct Token {
	string content;

	int type;   // type can be: identifier, number, op (!, <, >, etc.), punc, quotes

	Token()
	{

	};
	Token(string _content, int _type)
	{
		content = _content;
		type = _type;
	}

	/*Token& operator=(Token token)
	{
		content = token.content;
		type = token.type;
		return token;
	}*/
};



//*********************************************************************FUNCTION HEADERS***************************************************
vector<Token> tokenizer(int startLoc, string s);
//Commands
void parser_CreateTable(vector <Token> cmd);
void parser_insertInto(vector <Token> cmd);
void parser_Delete(vector <Token> cmd);
void paser_updateValue(vector<Token> cmd);
//Queries
void parser_select(vector <Token> cmd);
Table parser_product(vector <Token> cmd);
Table parser_rename(vector <Token> cmd);
//*******************************************************************Global Variables*****************************************************
vector <Table> allTables; //Tables stored in vector

//*********************************************************************MAIN****************************************************************
int main()
{
	//*********************************************************************TEST INPUTS********************************************************
	string testString = "CREATE TABLE animals (name VARCHAR(20), kind VARCHAR(8), years INTEGER) PRIMARY KEY (name, kind);";
	string testString2 = "dogs <- select (kind == \"dog\") animals;";
	string testString3 = "INSERT INTO animals VALUES FROM(\"Joe\", \"cat\", 4);";
	string testString4 = "INSERT INTO animals VALUES FROM(\"Spot\", \"dog\", 10);";
	string testString5 = "INSERT INTO animals VALUES FROM(\"Snoopy\", \"dog\", 3);";
	string testString6 = "INSERT INTO animals VALUES FROM(\"Tweety\", \"bird\", 1);";
	string testString7 = "INSERT INTO animals VALUES FROM(\"Joe\", \"bird\", 2);";
	string testString8 = "DELETE FROM animals WHERE (kind !=\"dog\" && name ==\"Joe\");";
	string testString9 = "a <- animals * animals;";
	string testString10 = "a <- rename (aname, akind) (animals);";
	string testString11 = "UPDATE animals SET name = change WHERE name == \"Spot\" ;";
	vector <string> testStrings = { testString, testString2, testString3, testString4, testString5, testString6, testString7, testString8, testString9, testString10, testString11 };
	string filename = "parsertest.txt";
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
				select = tokenizer(functionType+9, testStrings[iter]);
				if (testingMain)
				{
					cout << testStrings[iter].substr(functionType + 3, 6) << endl;
					for (int x = 0; x < select.size(); x++)
					{
						cout << select[x].content << " " << select[x].type << endl << endl;
					}
					cout << endl;
				}
				parser_select(select);
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
			else if (testStrings[iter].find("*") != string::npos)
			{
				string tablename = testStrings[iter].substr(0, functionType - 1);
				vector <Token> crossprod;
				crossprod = tokenizer(functionType + 3, testStrings[iter]);
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
				Table addThisTable = parser_product(crossprod);
				addThisTable.setName(tablename);
				allTables.push_back(addThisTable);
				cout << "Cross product call produced the following table: " << endl;
				allTables[allTables.size() - 1].printTable(allTables[allTables.size() - 1]);
			}
		}
		else
		{
			//else go thru commands (e.g. "CREATE TABLE", "SHOW", etc.)
			if (testStrings[iter].substr(0, 12) == "CREATE TABLE")
			{
				cout << testStrings[iter].substr(0, 12) << endl;

				vector <Token> createTable = tokenizer(13, testStrings[iter]);

				if (testingMain)
				{
					for (int x = 0; x < createTable.size(); x++)
					{
						cout << createTable[x].content << " " << createTable[x].type << endl << endl;
					}
					cout << endl;
				}

				parser_CreateTable(createTable);

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
	system("pause");
}
//**************************************************************************************************************************************************

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

//Takes in input command in vector form. Creates new table and stores it in all tables. 
void parser_CreateTable(vector<Token> cmd)
{
	string _name;							//stores name that will be passed to the dbms create table
	vector <Attribute> attrs;				//stores attributes that will be passed to dbms the create 
	int positionInInput;
	vector <Token> attributes;				//stores attributes (in test string input: (name VARCHAR(20), kind VARCHAR(8), years INTEGER))
	if (cmd[0].type == identifier)
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

//Takes in input command in vector form. Performs select operation and returns a table.
void parser_select(vector <Token> cmd)
{
	return;
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
						cout << "ERROR: Rename parser table not found. Dummy table returned" << endl;
						Attribute a("dummmy", "int");
						vector <Attribute> aaa;
						Table dummy(aaa, "dummy");
						return dummy;
					}


					Attribute a(allTables[tableloc].attrNameAt(0), allTables[tableloc].attrTypeAt(0), allTables[tableloc].attrKeyAt(0));
					Table returnTable = renameAttr(allTables[tableloc], a, newnames[0]);
					for (int x = 1; x < newnames.size(); x++)
					{
						Attribute a(allTables[tableloc].attrNameAt(0), allTables[tableloc].attrTypeAt(0), allTables[tableloc].attrKeyAt(0));
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


