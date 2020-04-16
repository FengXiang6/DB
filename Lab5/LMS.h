#pragma once
#include "RawLinker.h"
#include "Messager.h"
#include <cstring>
#include <cstdlib>
#include <cstring>
#include <iostream>
using namespace std;

#define readString(x, y) {\
	char c_msg[200];\
	sprintf_s(c_msg, 200,\
	"For %s : Please input a string not longer than %s characters! Single ENTER for unknow or not changed.",\
		#y, #x);\
	mMessager.PrintDirect(c_msg); \
	ReadString(x, y);\
}

#define readIntegerX(x) {\
	char c_msg[200];\
	sprintf_s(c_msg, 200,\
	"For %s : Please input a integer bigger than zero! Single ENTER for unknow or not changed.", #x);\
	mMessager.PrintDirect(c_msg); \
	ReadInteger(x);\
}

#define readIntegerXYZ(x, y, z) {\
	char c_msg[200];\
	sprintf_s(c_msg, 200,\
	"For %s : Please input a integer bigger than zero! Single ENTER for unknow or not changed.", #x);\
	mMessager.PrintDirect(c_msg); \
	ReadInteger(x, y, z);\
}

#define readFloatX(x) {\
	char c_msg[200];\
	sprintf_s(c_msg, 200,\
	"For %s : Please input a float bigger than zero! Single ENTER for unknow or not changed.", #x);\
	mMessager.PrintDirect(c_msg); \
	ReadFloat(x);\
}

#define readFloatXYZ(x, y, z) {\
	char c_msg[200];\
	sprintf_s(c_msg, 200,\
	"For %s : Please input a float bigger than zero! Single ENTER for unknow or not changed.", #x);\
	mMessager.PrintDirect(c_msg); \
	ReadFloat(x, y, z);\
}

#define subCondition(sql, value, comp) (\
	"(" + string(#sql) + comp + "'" + string((char*)value) +\
	"' or " + ((value[0] == 0) ? "true" : "false") + ")")
#define subCondiWithNumber(sql, value, comp) (\
	"(" + string(#sql) + comp + \
	((value[0] == 0) ? "0" : string((char*)value)) +\
	" or " + ((value[0] == 0) ? "true" : "false") + ")")
#define noEnterCondition(sql, value, comp) \
	"(" + string(#sql) + comp + "'" + string((char*)value) + "')"
#define notENTER(x) (!(x[0]==0))

class LibraryManageSystem {
public:
	int Run();
	~LibraryManageSystem();
public:
	void List();
	void Borrow();
	void ReturnBook();
	void UpdateMan();
	void UpdateFile();
	void SearchForBook();
	void CardManage();
private:
	void Initialize();
	void Function();
	void Welcome();
	void FullMatch();
	void FullFetch();
	int FetchBorrowRecord();
	void ListBorrowRecord();

	void ReadString(int maxlen, SQLCHAR* tgt);
	void ReadInteger(unsigned int & tgt, int maxlen = -1, SQLCHAR* str = NULL);
	void ReadFloat(float & tgt, int maxlen = -1, SQLCHAR* str = NULL);

private:
	RawLinker mLinker;
	Messager mMessager;
	unsigned int mOpcode;
	string Query;
private:
	SQLCHAR BookID[10 + 1];
	SQLCHAR BookName[30 + 1];
	SQLCHAR BookClass[20 + 1];
	SQLCHAR Press[20 + 1];
	SQLCHAR Author[20 + 1];
	SQLCHAR CardID[10 + 1];
	SQLCHAR OwnerName[20 + 1];
	SQLCHAR Department[20 + 1];
	SQLCHAR CardType[10 + 1];
	SQLCHAR DataFilePath[40 + 1];
	unsigned int Years, minYear, maxYear;
	float Price, minPrice, maxPrice;
	SQLCHAR strMinYear[10 + 1];
	SQLCHAR strMaxYear[10 + 1];
	SQLCHAR strMinPrice[10 + 1];
	SQLCHAR strMaxPrice[10 + 1];
	SQLCHAR BorrowDate[25 + 1];
	SQLCHAR ReturnDate[25 + 1];
	unsigned int TotalCollect;
	SQLCHAR strCollect[5 + 1];
	unsigned int Stock;
	SQLCHAR strStock[5 + 1];
};
