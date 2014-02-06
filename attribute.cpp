#include "stdafx.h"
#include "attribute.h"
#include <iostream>


Attribute::Attribute(string _name, string _type){
	
	name = _name;
	type = _type;

}

void Attribute::printAttribute(Attribute _att){
	cout << _att.name;

}
