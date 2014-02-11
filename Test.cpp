#include "stdafx.h"
#include "dbmsFunctions.h"

using namespace dbmsFunctions;

/*===========Declare Variables for Global Use==========*/
vector<Table> tableList; //The list which stores all tables currently in operation

//All Attributes related to user info
Attribute user_id("user_id", "int", "primary key");
Attribute user_name("user_name", "string");
Attribute privilege_level("privilege_level", "string");

//All Attributes related to orders
Attribute order_id("order_id", "int", "primary key");
Attribute order_paymentMethod("order_paymentMethod", "string");

//All Vectors of Attributes used in Table Creation
vector<Attribute> userAtt = { user_id, user_name, privilege_level };
vector<Attribute> orderAtt = { order_id, order_paymentMethod };
vector<Attribute> joinAtt = { user_id, order_id };

//All Rows related to user info
vector<string> row1 = { "0", "Bobby", "admin" };
vector<string> row2 = { "1", "Will", "guest" };
vector<string> row3 = { "2", "John", "guest" };

//All Rows related to orders
vector<string> row4 = { "0", "credit" };
vector<string> row5 = { "1", "counterfit?" };
vector<string> row5b = { "2", "Yen?" };

//All Rows Related to Join
vector<string> row6 = { "0", "12" };
vector<string> row7 = { "1", "9" };

//Tables for use in Relations
Table user_table(userAtt, "User Table");
Table order_table(orderAtt, "Order Table");
Table join_table(joinAtt, "Join Table");

int main(int argc, _TCHAR* argv[]){


	//testing create table
	cout << "Testing Create Table" << endl;
	tableList = createTable(tableList, userAtt, "user_table");
	tableList = createTable(tableList, orderAtt, "order_table");
	cout << "number of Tables created: " << tableList.size() << endl << endl;

	//pushing Rows into Tables
	user_table.pushBackRow(row1);
	user_table.pushBackRow(row2);
	user_table.pushBackRow(row3);
	order_table.pushBackRow(row4);
	order_table.pushBackRow(row5);
	join_table.pushBackRow(row6);
	join_table.pushBackRow(row7);

	//Printing Tables
	cout << "Printing Tables" << endl;
	cout << user_table.name << endl;
	user_table.printTable(user_table);
	cout << order_table.name << endl;
	order_table.printTable(order_table);
	cout << endl;

	//testing cross product relation
	cout << "Testing Cross Product: " << endl << endl;
	crossProduct(order_table, user_table);
	cout << endl;

	//testing projection relation
	cout << "Testing Projection: " << endl;
	projection(user_table, "user_id");
	cout << endl;
	projection(user_table, "user_name");

	//testing setDifference Relation
	cout << endl << "Testing Set Difference: " << endl;
	setDifference(user_table, order_table);
	setDifference(order_table, user_table);
	cout << endl;
	
	//testing setUnion Relation
	cout << "Testing Set Union: " << endl;
	Table Union1 = setUnion(user_table, user_table);
	Union1.printTable(Union1);
	cout << endl;

	//testing natural join
	cout << "testing Natural Join: " << endl;
	Table NatJon = naturalJoin(user_table, join_table, user_id);
	NatJon.printTable(NatJon);
	cout << endl;

	//testing rename Attribute
	cout << "testing rename attribute" << endl;
	renameAttr(user_table, user_id, "user_number");
	cout << endl;

	//testing drop table
	cout << "testing drop table" << endl;
	cout << "current number of tables: " << tableList.size() << endl;
	tableList = dropTable(tableList, "User Table");
	cout << endl << endl;

	//select test
	cout << "testing select function" << endl;
	cout << "selecting all users from User Table with ID less than 2" << endl;
	Table result = select(user_table, user_id, "2", '<');
	result.printTable(result);
	cout << endl;

	//update value
	cout << "testing update table" << endl;
	cout << "changing username from will to Yin" << endl;
	updateValue(user_table, "1", user_name, "Yin");
	user_table.printTable(user_table);
	cout << endl;

	//insert test
	cout << "testing insert function" << endl;
	cout << "inserting row into Order Table" << endl;
	insertRow(order_table, row5b);
	order_table.printTable(order_table);
	cout << endl;


	//delete test
	cout << "testing delete function" << endl;
	cout << "deleting previously inserted row from Order Table" << endl;
	deleteRow(order_table, "2");
	order_table.printTable(order_table);
	cout << endl;

}