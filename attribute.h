#ifndef attribute_H
#define attribute_H

#include <string>
#include <vector>

using namespace std;

class Attribute{
public:
	string name;		//stores value
	string type;		//type = int, string, double, char...
	string keyType;		//keyType = primary key, foreign key, or attribute
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