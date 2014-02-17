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
#define testingParserProject (true)

using namespace std;


//TABLES STORED IN VECTOR
vector <Table> allTables;
Attribute name{ "name", "string" };
Attribute kind{ "kind", "string" };
vector<Attribute> animAtt = { name, kind };
Table animals(animAtt, "animals");
Table dogs(animAtt, "dogs");
Table cats(animAtt, "cats");

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
//string testString = "CREATE TABLE animals (name VARCHAR(20), kind VARCHAR(8), years INTEGER) PRIMARY KEY (name, kind);";
//string testString = "a <- project (name, kind) animals;";
string testString = "catsordogs <- dogs - cats;";


//*********************************************************************FUNCTION HEADERS***************************************************
vector<Token> tokenizer(int startLoc, string s);
void parser_CreateTable(vector <Token> cmd);
void parser_Project(vector <Token> cmd);
void parser_setDif(vector<Token> cmd);
vector<string> row1 = { "joe", "parrot" };
vector<string> row2 = { "alice", "carrot" };
vector<string> row3 = { "oliver", "chesire" };
vector<string> row4 = { "felicity", "tabby" };
vector<string> row5 = { "rick", "shelty" };
vector<string> row6 = { "carl", "shitzu" };

int main()
{
	animals.pushBackRow(row1);
	animals.pushBackRow(row2);
	dogs.pushBackRow(row5);
	dogs.pushBackRow(row6);
	cats.pushBackRow(row3);
	cats.pushBackRow(row4);
	allTables.push_back(dogs);
	allTables.push_back(cats);
	allTables.push_back(animals);
	dogs.printTable(dogs);
	cats.printTable(cats);
	//if the input is a query (idk how to spell tht) it includes a "<-" in it.
	size_t functionType = testString.find("<-");
	size_t funcSubType = testString.find(" - ");
	cout << "funcSub" << funcSubType << endl;
	if (functionType != string::npos)
	{
		//if "<-" is found go thru query calls (e.g. select, project, etc.)
		if (testString.substr(functionType + 3, 6) == "select")
		{
			vector <Token> select;
			select = tokenizer(0, testString);
			if (testingMain)
			{
				for (int x = 0; x < select.size(); x++)
				{
					cout << select[x].content;
				}
				cout << endl;
			}
		}
		else if (testString.substr(functionType + 3, 7) == "project")
		{
			vector <Token> project;
			project = tokenizer(0, testString);
			if (testingMain)
			{
				for (int x = 0; x < project.size(); x++)
				{
					cout << project[x].content;
				}
				cout << endl;
			}
			parser_Project(project);
		}
		else if (funcSubType != string::npos){
			cout << "started: ";
			vector<Token> setDif;
			setDif = tokenizer(0, testString);
			cout << "thats a set dif: " << endl;
			if (testingMain)
			{
				for (int x = 0; x < setDif.size(); x++)
				{
					cout << setDif[x].content;
				}
				cout << endl;
			}
			parser_setDif(setDif);
		}
	}

	else
	{
		//else go thru commands (e.g. "CREATE TABLE", "SHOW", etc.)
		if (testString.substr(0, 12) == "CREATE TABLE")
		{
			if (testingMain)
			{
				cout << testString.substr(0, 12) << endl;
			}

			vector <Token> createTable = tokenizer(13, testString);

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
		// else if()
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

			if (testingParserProject)
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

void parser_Project(vector<Token> cmd){
	string _name;							//stores name of the table that will be passed to the parser project 
	string _tableName;
	vector <Attribute> attrs;				//stores attributes that will be passed to parser project
	int positionInInput;
	vector <Token> attributes; //stores attributes
	int tableLoc;

	_tableName = cmd[0].content;
	_name = cmd[cmd.size() - 2].content;
	if (cmd[4].type == punc)
	{
		//parse thru for equal number of open and closed parenthesis. store content of middle as vector of tokens and start working with tht
		//for loop thru vector of tokens while storing in attribute list
		int openParenthesisCount = 0;
		int closedParenthesisCount = 0;
		if (cmd[4].content == "(")
		{
			Token t("(", punc);
			attributes.push_back(t);
			openParenthesisCount++;
			for (int x = 5; x < cmd.size(); x++)
			{
				if (cmd[x].content == ")"){
					closedParenthesisCount++;
				}
				else if (cmd[x].content == "("){
					openParenthesisCount++;
				}

				attributes.push_back(cmd[x]);
				if (openParenthesisCount == closedParenthesisCount)
				{
					//update int position in input and use for next if/else clause with primary keys
					positionInInput = x + 1;
					break;

				}
			}

			if (testingParserProject)
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
				int x = 0;
				for (int z = 0; z < attributes.size(); z++)
				{
					x = z + 4;
					if (cmd[x].type == identifier)
					{
						Attribute a(cmd[x].content, "string");
						attrs.push_back(a);
						/*if (cmd[x+1].type == identifier){

								int y = x + 1;
								while (cmd[y].type != punc && cmd[y].content != ")")
								{
								y++;
								}
								x = y;
								}
								else
								{

								cout << "ERROR: Create Table format incorrect. Expecting type name after identifier in attribute list." << endl;
								return;
								}
								}
								*/
					}
					else if (cmd[x].type == punc){
						//dont do anything if they are commas
					}
					else
					{
						cout << attrs.size() << "size" << endl;
						cout << "ERROR: Create Table format incorrect. Check attribute list entered." << endl;
						return;
					}
				}

			}

		//	if (testingParserProject)
			//{
			//	for (int x = 0; x < attrs.size(); x++)
			//	{
			//		cout << attrs[x].getName() << "     " << endl;
			//	}
			//}


		}
		else
		{
			cout << "ERROR: Create Table format incorrect. Expecting open parenthesis as first input of projection" << endl;
			return;
		}

		
		bool projectFoundTable = false;
		for (int x = 0; x < allTables.size(); x++)
			{
				if (cmd[cmd.size() - 2].content == allTables[x].getTableName())
				{
					tableLoc = x;
					projectFoundTable = true;
					break;
					cout << "table loc: " << x << endl;
				}
			}
			
		}
	else
	{
		cout << "ERROR: project format incorrect. Missing open parenthesis." << endl;
		cout << cmd[0].content << endl;
		cout << cmd[1].content << endl;
		cout << cmd[2].content << endl;
		cout << cmd[3].content << endl;
		cout << cmd[4].content << endl;
		cout << cmd[5].content << endl;
		cout << cmd[6].content << endl;
		cout << cmd[7].content << endl;
		return;
	}
	animals.printTable(animals);
	Table projPrac = projection(allTables[tableLoc], attrs);
	projPrac.changeName(_tableName);
//	cout << projPrac.getTableName() << endl;
	allTables.push_back(projPrac);
}
void parser_setDif(vector<Token> cmd){
	int useThis = 0;
	int useThisToo = 0;
	int tableLoc = 0;
	int tableLoc2 = 0;
	string _newTableName = cmd[0].content;
	for (int w = 0; w < cmd.size(); w++){
		if (cmd[w].content == "<"){
			if (cmd[w + 1].content == "-"){
				useThis = w + 2;
			}
		}
	}
	useThisToo = useThis + 2; 
	bool setDifFoundTable1 = false;
	for (int x = 0; x < allTables.size(); x++){
		if (cmd[useThis].content == allTables[x].getTableName())
		{
			tableLoc = x;
			setDifFoundTable1 = true;
			break;
			cout << "table loc: " << x << endl;
		}
	}
	bool setDifFoundTable2 = false;
	for (int y = 0; y < allTables.size(); y++){
		if (cmd[useThisToo].content == allTables[y].getTableName())
		{
			tableLoc = y;
			setDifFoundTable2 = true;
			break;
			cout << "table loc2: " << y << endl;
		}
	}

	Table setDif1 = setDifference(allTables[tableLoc], allTables[tableLoc2]);
	setDif1.printTable(setDif1);
	setDif1.changeName(_newTableName);
	cout << setDif1.getTableName();

}