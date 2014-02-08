#ifndef attribute_H
#define attribute_H

#include <string>
#include <vector>

using namespace std;

class Attribute{
public:
	string name;
	string type;

	Attribute(string _name, string _type);
	void printAttribute(Attribute _att);

};

#endif

//hello