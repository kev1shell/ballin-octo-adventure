
#include <stdio.h>
#include <tchar.h>
#include "attribute.h"
#include <iostream>


Attribute::Attribute(string _name, string _type){

	name = _name;
	type = _type;

	keyType = "attribute"; //keyType set to "attribute" if attribute isn't primary or foreign key
}

Attribute::Attribute(string _name, string _type, string _keyType)
{
	name = _name;
	type = _type;

	keyType = _keyType;
}

void Attribute::printAttribute(Attribute _att){
	cout << _att.name;

}

//************************************************ MODIFIERS **************************************************************
void Attribute::setName(string _name)
{
	name = _name;
}

void Attribute::setType(string _type)
{
	type = _type;
}

void Attribute::setPrimaryKey()
{
	keyType = "primary key";
}

void Attribute::setForeignKey()
{
	keyType = "foreign key";
}


//********************************************** ACCESSORS *****************************************************************
string Attribute::getKeyType()
{
	return keyType;
}

string Attribute::getName()
{
	return name;
}

string Attribute::getType()
{
	return type;
}