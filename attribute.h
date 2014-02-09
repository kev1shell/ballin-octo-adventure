#ifndef attribute_H
#define attribute_H

#include <string>
#include <vector>

using namespace std;

class Attribute{
private:
	string name;		//stores value
	string type;		//type = int, string, double, char...
	string keyType;		//keyType = primary key, foreign key, or attr

public:

	Attribute(string _name, string _type);
	Attribute(string _name, string _type, string _keyType);
	
	//Print
	void printAttribute(Attribute _att);

	//Modifiers
	void setName(string _name);
	void setPrimaryKey();
	void setForeignKey();

	//Accessors
	string getKeyType();
	string getName();
	string getType();
};

#endif

//hello