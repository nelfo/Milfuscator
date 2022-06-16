#pragma once

/*! The TypeClass class */
enum TypeClass {
	CLASS_CODE = 0,
	CLASS_DATA
};

/*! The DataType class */
enum DataType {
	DT_FUNCTION,
	DT_DATA
};

/*! The Data class */
class Data {
public:
	unsigned long offset;
	unsigned long length;

	std::string name;
	std::string cname; // This is the demangled C/++ name, this is in development and may be empty
	std::string library;
	std::string object; // This is for imports, and some other MSVCRT-type objects

	DataType type;
};

/*! The Section class */
class Section : public Data {
public:
	TypeClass typeclass;
};

/*! The Relocation class */
class Relocation : public Data {
public:
	unsigned long reltype;
};
