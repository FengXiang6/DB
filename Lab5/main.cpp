#include "RawLinker.h"
#include "Messager.h"
#include <iostream>
#include <fstream>
#include <ctime>
using namespace std;

#define readString(x, y) {\
	char c_msg[200];\
	sprintf_s(c_msg, 200,\
	"For %s : Please input a string not longer than %s characters! Single @ for unknow or not changed.",\
		#y, #x);\
	mMessager.PrintDirect(c_msg); \
	ReadString(x, y);\
}

#define readIntegerX(x) {\
	char c_msg[200];\
	sprintf_s(c_msg, 200,\
	"For %s : Please input a integer bigger than zero! Single @ for unknow or not changed.", #x);\
	mMessager.PrintDirect(c_msg); \
	ReadInteger(x);\
}

#define readIntegerXYZ(x, y, z) {\
	char c_msg[200];\
	sprintf_s(c_msg, 200,\
	"For %s : Please input a integer bigger than zero! Single @ for unknow or not changed.", #x);\
	mMessager.PrintDirect(c_msg); \
	ReadInteger(x, y, z);\
}

#define readFloatX(x) {\
	char c_msg[200];\
	sprintf_s(c_msg, 200,\
	"For %s : Please input a float bigger than zero! Single @ for unknow or not changed.", #x);\
	mMessager.PrintDirect(c_msg); \
	ReadFloat(x);\
}

#define readFloatXYZ(x, y, z) {\
	char c_msg[200];\
	sprintf_s(c_msg, 200,\
	"For %s : Please input a float bigger than zero! Single @ for unknow or not changed.", #x);\
	mMessager.PrintDirect(c_msg); \
	ReadFloat(x, y, z);\
}

#define subCondition(sql, value, comp) (\
	"(" + string(#sql) + comp + "'" + string((char*)value) +\
	"' or " + ((value[0] == '@') ? "true" : "false") + ")")
#define subCondiWithNumber(sql, value, comp) (\
	"(" + string(#sql) + comp + \
	((value[0]=='@')?"0":string((char*)value)) +\
	" or " + ((value[0] == '@') ? "true" : "false") + ")")

#define notAtToken(x) (!(x[0]=='@'&&x[1]==0))

class LibraryManageSystem {
public:
	int Run() 
	{
		try { Initialize(); }
		catch (...) {
			mMessager.PrintDirect("Linking to database failed!", true, true);
			return 0;
		}
		while (mLinker.IsLinking())
		{
			Welcome();
			Function();
			mMessager.PrintDirect("Press any key to continue.");
		}

	}
	~LibraryManageSystem() 
	{
		if (mLinker.IsLinking())
			mLinker.Release();
	}
private:
	RawLinker mLinker;
	Messager mMessager;
	unsigned int mOpcode;
	SQLCHAR BookID[10 + 1];
	SQLCHAR BookName[30 + 1];
	SQLCHAR BookClass[20 + 1];
	SQLCHAR Press[20 + 1];
	SQLCHAR Author[20 + 1];
	SQLCHAR CardID[10 + 1];
	SQLCHAR OwnerName[20 + 1];
	SQLCHAR Department[20 + 1];
	SQLCHAR CardType[1 + 1];
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
	string Query;

private:
	void Welcome() 
	{
		mMessager.Append("Welcome to Library Manage System!");
		mMessager.Append("(1)Search         (2)Borrow      (3)Return");
		mMessager.Append("(4)Update by file (5)Card Manage (6)Update manually");
		mMessager.Append("(7)Print borrow record           (8)Show list");
		mMessager.Append("(0)Quit");
		mMessager.Append("Please input your operation code:");
		mMessager.Print(true, true);
	}
	void Initialize() {
		mLinker.Start();
		if (!mLinker.IsLinking()) {
			throw exception();
		}
	}
	void List() {
		Query = "select * from book";
		mLinker.Execute(Query);
		if (mLinker.GetLastError()) {
			mMessager.PrintDirect("An error occured:", false, true);
			mLinker.ReportError();
			throw std::exception();
		}
		mMessager.PrintDirect("All of the corresponding books in the library are listed in following table:",
			true, true, 1);
		FullFetch();
		mMessager.PrintDirect("\n\n");
	}
	void FullSearch() {
		Query = "select * from book where " +
			subCondition(BookID, BookID, "=") + " and " +
			subCondition(BookName, BookName, "=") + " and " +
			subCondition(BookClass, BookClass, "=") + " and " +
			subCondition(Press, Press, "=") + " and " +
			subCondition(Author, Author, "=") + " and " +
			subCondiWithNumber(Years, strMinYear, ">=") + " and " +
			subCondiWithNumber(Years, strMaxYear, "<=") + " and " +
			subCondiWithNumber(Price, strMinPrice, ">=") + " and " +
			subCondiWithNumber(Price, strMaxPrice, "<=")  + ";";
		//mLinker.Prepare((SQLCHAR*)Query.c_str(), SQL_NTS);
		mLinker.CloseStatement();
		mLinker.Execute(Query);
		if (mLinker.GetLastError()) {
			mMessager.PrintDirect("An error occured:", false, true);
			mLinker.ReportError();
			throw std::exception();
		}
	}
	void FullFetch() {
		mLinker.GetVarchar(1, BookID, 10 + 1);
		mLinker.GetVarchar(2, BookName, 30 + 1);
		mLinker.GetVarchar(3, BookClass, 20 + 1);
		mLinker.GetVarchar(4, Press, 20 + 1);
		mLinker.GetInteger(5, Years);
		mLinker.GetVarchar(6, Author, 20 + 1);
		mLinker.GetFloat(7, Price);
		mLinker.GetInteger(8, TotalCollect);
		mLinker.GetInteger(9, Stock);

		mMessager.MakeTable("BookID", 10 + 5, 1);
		mMessager.MakeTable("BookName", 30, 1);
		mMessager.MakeTable("BookClass", 20, 1);
		mMessager.MakeTable("Press", 20, 1);
		mMessager.MakeTable("Author", 20, 1);
		mMessager.MakeTable("Years", 8, 1);
		mMessager.MakeTable("Price", 8, 1);
		mMessager.MakeTable("Total", 6, 1);
		mMessager.MakeTable("Stock", 6, 1);
		mMessager.EndTable();

		while (mLinker.Fetch()) {
			mMessager.MakeTable((char*)BookID, 10 + 5, 1);
			mMessager.MakeTable((char*)BookName, 30, 1);
			mMessager.MakeTable((char*)BookClass, 20, 1);
			mMessager.MakeTable((char*)Press, 20, 1);
			mMessager.MakeTable((char*)Author, 20, 1);
			char tmp[30];
			sprintf_s(tmp, 30, "%-8d", Years); 
			mMessager.MakeTable(tmp, 8, 1);
			sprintf_s(tmp, 30, "%-8.2f", Price); 
			mMessager.MakeTable(tmp, 8, 1);
			sprintf_s(tmp, 30, "%-6d", TotalCollect); 
			mMessager.MakeTable(tmp, 6, 1);
 			//????????????????????????????? wtf
			sprintf_s(tmp, 30, "%-6d", Stock); 
			mMessager.MakeTable(tmp, 6, 1);
			mMessager.EndTable();
		}
	}
	int FetchBorrowRecord() {
		mLinker.CloseStatement();
		Query.clear();
		Query.append("select * from borrowing_view where ");
		Query.append(subCondition(CardID, CardID, "="));
		mLinker.Execute(Query);

		mLinker.GetVarchar(1, CardID, 10 + 1);
		mLinker.GetVarchar(2, BookID, 10 + 1);
		mLinker.GetVarchar(3, BookName, 30 + 1);
		mLinker.GetVarchar(4, Author, 20 + 1);
		mLinker.GetVarchar(5, BorrowDate, 25 + 1);

		mMessager.MakeTable("CardID", 10 + 3, 1);
		mMessager.MakeTable("BookID", 10 + 3, 1);
		mMessager.MakeTable("BookName", 30, 1);
		mMessager.MakeTable("Author", 20 + 1, 1);
		mMessager.MakeTable("BorrowDate", 25 + 1, 1);
		mMessager.EndTable();
		int cnt = 0;
		while (mLinker.Fetch()) {
			cnt++;
			mMessager.MakeTable((char*)CardID, 10 + 3, 1);
			mMessager.MakeTable((char*)BookID, 10 + 3, 1);
			mMessager.MakeTable((char*)BookName, 30, 1);
			mMessager.MakeTable((char*)Author, 20 + 1, 1);
			mMessager.MakeTable((char*)BorrowDate, 25 + 1, 1);
			mMessager.EndTable();
		}
		mMessager.PrintDirect("", false, false, 1);
		return cnt;
	}
	void Borrow() {
		bool success = true;
		do {
			mMessager.PrintDirect(
				"Now you are borrowing books...please input corresponding information.");
			try {
				readString(10, CardID);
				if (!notAtToken(CardID)) {
					mMessager.PrintDirect("Not accept @ now for specifically searching!", false, true);
					throw exception();
				}
			}
			catch (exception &e)
			{
				return;
			}

			unsigned int cnt = 0;
			Query = "select count(*) from card where" + subCondition(CardID, CardID, "=") + ";";
			mLinker.CloseStatement();
			mLinker.Execute(Query);
			if (mLinker.GetLastError()) {
				mMessager.PrintDirect("An error occured:", false, true);
				mLinker.ReportError();
				return;
			}
			mLinker.GetInteger(1, cnt);
			mLinker.Fetch();
			if (!cnt) {
				mMessager.PrintDirect("Not found this card!", true, true, 2);
				return;
			}

			mMessager.PrintDirect("All of the borrowing records of this card are listed in the following table:", 
				true, true, 1);
			FetchBorrowRecord();

			mMessager.PrintDirect("Now you need to input the id of book that you want to borrow.");
			try {
				readString(10, BookID);
				if (!notAtToken(BookID)) {
					mMessager.PrintDirect("Not accept @ now for specifically searching!", false, true);
					throw exception();
				}
			}
			catch (exception &e)
			{
				return;
			}

			//borrow book
			time_t now_time = time(NULL);
			tm* t_tm = localtime(&now_time);
			sprintf_s((char*)BorrowDate, 25, 
				"%04d-%02d-%02d %02d:%02d:%02d",
				t_tm->tm_year+1900, t_tm->tm_mon+1, t_tm->tm_mday,
				t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);

			Query.clear();
			Query.append("select borrow_book(?,?,?);");
			mLinker.CloseStatement();
			mLinker.Prepare((SQLCHAR*)Query.c_str(), SQL_NTS);
			mLinker.InVarchar(1, CardID, 10);
			mLinker.InVarchar(2, BookID, 10);
			mLinker.InVarchar(3, BorrowDate, 25);

			mLinker.Execute();
			if (mLinker.GetLastError()) {
				mMessager.PrintDirect("An error occured:", false, true);
				mLinker.ReportError();
				throw exception();
			}

			unsigned int state;
			mLinker.GetInteger(1, state);
			if (state == 1) {
				mMessager.PrintDirect("Failed to borrow the book!", true, true, 2);
				return;
			}
			else {
				mMessager.PrintDirect("Borrow the book successfully!", true, true, 2);
				return;
			}
			success = true;
			//check if borrow successfully
			//if not -> fetch data
			//if yes -> hint borrow successfully

		} while (!success);
	}
	void SearchBook() {
		bool success = true;
		do {
			mMessager.PrintDirect(
				"Now you are searching books...please input corresponding information.");
			try {
				readString(10, BookID);
				readString(30, BookName);
				readString(20, BookClass);
				readString(20, Press);
				readString(20, Author);
				readIntegerXYZ(minYear, 10, strMinYear);
				readIntegerXYZ(maxYear, 10, strMaxYear);
				readFloatXYZ(minPrice, 10, strMinPrice);
				readFloatXYZ(maxPrice, 10, strMaxPrice);
			}
			catch (exception &e)
			{
				success = false;
			}

		} while (!success);
		
		try {
			FullSearch();
		}
		catch (exception& e) {
			throw e;
		}
		mMessager.PrintDirect("All of the corresponding books in the library are listed in following table:",
			true, true, 1);
		FullFetch();
		mMessager.PrintDirect("\n\n");
	}
	void ReturnBook() {
		bool success = true;
		do {
			mMessager.PrintDirect(
				"Now you are returning the book...please input corresponding information.");
			try {
				readString(10, CardID);
				if (!notAtToken(CardID)) {
					mMessager.PrintDirect("Not accept @ now for specifically searching!", false, true);
					throw exception();
				}
			}
			catch (exception &e)
			{
				return;
			}

			unsigned int cnt = 0;
			Query = "select count(*) from card where" + subCondition(CardID, CardID, "=") + ";";
			mLinker.CloseStatement();
			mLinker.Execute(Query);
			if (mLinker.GetLastError()) {
				mMessager.PrintDirect("An error occured:", false, true, 2);
				mLinker.ReportError();
				return;
			}
			mLinker.GetInteger(1, cnt);
			mLinker.Fetch();
			if (!cnt) {
				mMessager.PrintDirect("Not found this card!", true, true, 2);
				return;
			}
 
			mMessager.PrintDirect("All of the borrowing records of this card are listed in the following table:",
				true, true, 1);
			FetchBorrowRecord();

			mMessager.PrintDirect("Now you need to input the BookID of the book that you are going to return.");
			try {
				readString(10, BookID);
			}
			catch (exception& e) {
				return;
			}

			//return books
			time_t now_time = time(NULL);
			tm* t_tm = localtime(&now_time);
			sprintf_s((char*)ReturnDate, 25,
				"%04d-%02d-%02d %02d:%02d:%02d",
				t_tm->tm_year + 1900, t_tm->tm_mon + 1, t_tm->tm_mday,
				t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);

			Query.clear();
			Query.append("select return_book(?,?,?);");
			mLinker.CloseStatement();
			mLinker.Prepare((SQLCHAR*)Query.c_str(), SQL_NTS);
			mLinker.InVarchar(1, CardID, 10);
			mLinker.InVarchar(2, BookID, 10);
			mLinker.InVarchar(3, ReturnDate, 25);

			mLinker.Execute();
			if (mLinker.GetLastError()) {
				mMessager.PrintDirect("An error occured:", false, true);
				mLinker.ReportError();
				throw exception();
			}

			unsigned int state;
			mLinker.GetInteger(1, state);
			if (state == 1) {
				mMessager.PrintDirect("Failed to return the book!", true, true, 2);
				return;
			}
			else {
				mMessager.PrintDirect("Return the book successfully!", true, true, 2);
				return;
			}
			success = true;
		} while (!success);
	}
	void UpdateMan() {
		bool success = false;
		SQLCHAR strT[10], strS[10];
		do {
			mMessager.PrintDirect(
				"Now you are updating book in the library...please input corresponding information.");
			try {
				readString(10, BookID);
				readString(30, BookName);
				readString(20, BookClass);
				readString(20, Press);
				readString(20, Author);
				readIntegerXYZ(Years, 10, strMinYear);
				readFloatXYZ(Price, 10, strMinPrice);
				readIntegerXYZ(TotalCollect, 10, strT);
				readIntegerXYZ(Stock, 10, strS);
			}
			catch (exception &e)
			{
				success = false;
				return;
			}
			if (notAtToken(BookID) && notAtToken(BookName) && notAtToken(BookClass) &&
				notAtToken(Press) && notAtToken(Author) && notAtToken(strMinYear) && 
				notAtToken(strMinPrice) && notAtToken(strT) && notAtToken(strS)) {
				
				mLinker.CloseStatement();
				mLinker.Execute("call new_execute_queue_prepare();");
				if (mLinker.GetLastError()) {
					mLinker.ReportError();
				}

				Query.clear();
				Query.append(
					"select update_library_book(?,?,?,?,?,?,?,?,?);");

				mLinker.CloseStatement();
				mLinker.Prepare((SQLCHAR*)Query.c_str(), SQL_NTS);

				int lY = Years; 
				int lT = TotalCollect; 
				int lS = Stock;

				mLinker.InVarchar(1, BookID, 10);
				mLinker.InVarchar(2, BookName, 30);
				mLinker.InVarchar(3, BookClass, 20);
				mLinker.InVarchar(4, Press, 20);
				mLinker.InInteger(5, lY);
				mLinker.InVarchar(6, Author, 20);
				mLinker.InFloat(7, Price);
				mLinker.InInteger(8, lT);
				mLinker.InInteger(9, lS);

				mLinker.Execute(); 
				if (mLinker.GetLastError()) {
					mMessager.PrintDirect("An error occured!", true, true, 1);
					mLinker.ReportError();
					mMessager.PrintDirect("\n\n");
					return;
				}

				unsigned int state;
				mLinker.GetInteger(1, state);
				mLinker.Fetch();
				if (state == 1) {
					mMessager.PrintDirect("Failed to update the book", true, true, 2);
				}
				else {
					mMessager.PrintDirect("This book has been modified", true, true, 1);
					mLinker.CloseStatement();
					mLinker.Execute("select * from update_mid_table;");

					mLinker.GetVarchar(1, BookID, 10 + 1);
					mLinker.GetVarchar(2, BookName, 30 + 1);

					mMessager.MakeTable("BookID", 10 + 5, 1);
					mMessager.MakeTable("BookName", 30, 1);
					mMessager.EndTable();

					while (mLinker.Fetch()) {
						mMessager.MakeTable((char*)BookID, 10 + 5, 1);
						mMessager.MakeTable((char*)BookName, 30, 1);
						mMessager.EndTable();
					}
					mMessager.PrintDirect("\n", false, false, 1);
				}

				success = true;
			}
			else {
				mMessager.PrintDirect("Do not input '@'! Specify the information!");
				continue;
			}
		} while (!success);
	}
	//bool FileReadString(FILE* fp, SQLCHAR* s) {
	//	int sp = 0;
	//	char c;
	//	while ((c = fgetchar()) == ' ');
	//	if (c == '\n')return false;
	//	do {
	//		if (c == ',' || c == '\n')break;
	//		s[sp++] = c;
	//	} while (c = fgetchar());
	//	s[sp] = 0;
	//	return true;
	//}
	void UpdateFile() {
		bool success = false;
		do {
			mMessager.PrintDirect("Please input the data file location.");
			try {
				readString(40, DataFilePath);
			}
			catch (exception& e) {
				mMessager.PrintDirect("Falied to input file location", false, false, 2);
				continue;
			}

			if (!notAtToken(DataFilePath)) {
				mMessager.PrintDirect("Please specify the file location!");
				continue;
			}

			
			FILE* fp = fopen((char*)DataFilePath, "r");
			if (fp == NULL) {
				mMessager.PrintDirect("Can not open the file!",false, true, 2);
				return;
			}

			char buffer[200];
			//read title
			fscanf(fp, "%[^\n]", buffer);
			int linecnt = 1;

			mLinker.CloseStatement();
			mLinker.Execute("call new_execute_queue_prepare();");
			if (mLinker.GetLastError()) {
				mLinker.ReportError();
			}

			Query.clear();
			Query.append(
				"select update_library_book(?,?,?,?,?,?,?,?,?);");

			mLinker.CloseStatement();
			mLinker.Prepare((SQLCHAR*)Query.c_str(), SQL_NTS);

			int lY; int lT; int lS;
			mLinker.InVarchar(1, BookID, 10);
			mLinker.InVarchar(2, BookName, 30);
			mLinker.InVarchar(3, BookClass, 20);
			mLinker.InVarchar(4, Press, 20);
			mLinker.InInteger(5, lY);
			mLinker.InVarchar(6, Author, 20);
			mLinker.InFloat(7, Price);
			mLinker.InInteger(8, lT);
			mLinker.InInteger(9, lS);

			int new_tuple = 0; 
			int insert = 0;
			int update = 0;
			unsigned int state;

			do {
				char c = fgetc(fp);
				int ret = fscanf(fp, 
					"%[^,], %[^,], %[^,], %[^,], %d, %[^,], %f, %d, %d",
					BookID, BookName, BookClass,
					Press, &lY, Author, 
					&Price, &lT, &lS);
				if (ret == EOF)break;

				linecnt++;

				mLinker.Execute();

				if (mLinker.GetLastError()) {
					mLinker.ReportError();
					char sline[25];
					sprintf(sline, "error at line %d", linecnt);
					mMessager.PrintDirect(sline);
				}
				else {
					new_tuple++;
					mLinker.GetInteger(1, state);
					mLinker.Fetch();
					if (state == 1) {
						insert++;
					}
					else {
						update++;
					}
				}
				mLinker.CloseStatement();
				
			} while (1);
			char Message[200];
			sprintf(Message, "%d rows in total: (%d) tuples have been inserted | (%d) tuples have been updated\n(%d) tuples has been operated.", linecnt - 1, insert, update, new_tuple);
			mMessager.Append(Message);
			mMessager.Append("These books has been operated:");
			mMessager.Print(true, true, 1);

			mLinker.CloseStatement();
			mLinker.Execute("select * from update_mid_table;");
			
			mLinker.GetVarchar(1, BookID, 10 + 1);
			mLinker.GetVarchar(2, BookName, 30 + 1);

			mMessager.MakeTable("BookID", 10 + 5, 1);
			mMessager.MakeTable("BookName", 30, 1);
			mMessager.EndTable();

			while (mLinker.Fetch()) {
				mMessager.MakeTable((char*)BookID, 10 + 5, 1);
				mMessager.MakeTable((char*)BookName, 30, 1);
				mMessager.EndTable();
			}
			mMessager.PrintDirect("\n", false, false, 1);

			fclose(fp);
			success = true;
		} while (!success);
	}
	void ListBorrowRecord() {
		bool success = false;
		do {
			mMessager.PrintDirect(
				"Now you are checking all of your borrow records...please input corresponding information.");
			try {
				readString(10, CardID);
				if (!notAtToken(CardID)) {
					mMessager.PrintDirect("Not accept @ now for specifically searching!", false, true);
					throw exception();
				}
			}
			catch (exception &e)
			{
				return;
			}

			unsigned int cnt = 0;
			Query = "select count(*) from card where" + subCondition(CardID, CardID, "=") + ";";
			mLinker.CloseStatement();
			mLinker.Execute(Query);
			if (mLinker.GetLastError()) {
				mMessager.PrintDirect("An error occured:", false, true);
				mLinker.ReportError();
				return;
			}
			mLinker.GetInteger(1, cnt);
			mLinker.Fetch();
			if (!cnt) {
				mMessager.PrintDirect("Not found this card!", true, true, 2);
				return;
			}


			Query.clear();
			Query.append("select * from Record where CardID = ?;");
			mLinker.CloseStatement();
			mLinker.Prepare((SQLCHAR*)Query.c_str(), SQL_NTS);
			mLinker.InVarchar(1, CardID, 10);
			mLinker.Execute();

			if (mLinker.GetLastError()) {
				mMessager.PrintDirect("An error occured:", false, true);
				mLinker.ReportError();
				return;
			}

			mLinker.GetVarchar(1, CardID, 10 + 1);
			mLinker.GetVarchar(2, BookID, 10 + 1);
			mLinker.GetVarchar(3, BorrowDate, 25 + 1);
			mLinker.GetVarchar(4, ReturnDate, 25 + 1);

			mMessager.PrintDirect("All of the records of this card are listed in the following table:",
				true, true, 1);

			mMessager.MakeTable("CardID", 10 + 3, 1);
			mMessager.MakeTable("BookID", 10 + 3, 1);
			mMessager.MakeTable("BorrowDate", 25 + 1, 1);
			mMessager.MakeTable("ReturnDate", 25 + 1, 1);
			mMessager.EndTable();
			while (mLinker.Fetch()) {
				mMessager.MakeTable((char*)CardID, 10 + 3, 1);
				mMessager.MakeTable((char*)BookID, 10 + 3, 1);
				mMessager.MakeTable((char*)BorrowDate, 25 + 1, 1);
				mMessager.MakeTable((char*)ReturnDate, 25 + 1, 1);
				mMessager.EndTable();
			}
			success = true;
			mMessager.PrintDirect("", false, false, 2);
		} while (!success);
	}
	void Function() {
		mOpcode = -1;
		string args;
		getline(cin, args);
		if (args.length() != 1 || args[0] - '0' < 0 || args[0] - '8'>0) {
			mMessager.PrintDirect("Please input a operation code of integer in [0...5]!",
				false, true, 2);
			return;
		}


		mOpcode = args[0] - '0';
		//quit -- [done]
		if (mOpcode == 0) {
			mLinker.Release();
			return;
		}
		//search book -- [done]
		else if (mOpcode == 1) {
			SearchBook();
		}
		//borrow book -- [done]
		else if (mOpcode == 2) {
			Borrow();
		}
		//return book -- [done]
		else if (mOpcode == 3) {
			ReturnBook();
		}
		//update by file -- [done]
		else if (mOpcode == 4) {
			UpdateFile();
		}
		else if (mOpcode == 5) {
			//CardManage();
		}
		else if (mOpcode == 6) {
			UpdateMan();
		}
		//list record -- [done]
		else if (mOpcode == 7) {
			ListBorrowRecord();
		}
		//list -- [done]
		else if (mOpcode == 8) {
			List();
		}
	}
	void ReadString(int maxlen, SQLCHAR* tgt) {
		string tmp;
		getline(cin, tmp);
		if (tmp.length() > maxlen || tmp.length() < 1) {
			mMessager.PrintDirect("Too much or too less characters!", 
				false, true, 2);
			throw exception();
		}
		tmp.push_back(0);
		memcpy(tgt, tmp.c_str(), tmp.length());
	}
	void ReadInteger(unsigned int & tgt, int maxlen = -1, SQLCHAR* str = NULL) {
		string tmp;
		int mid = 0xffffffff;
		getline(cin, tmp);
		if (maxlen > 0 && tmp.length() > maxlen) {
			mMessager.PrintDirect("Invalid Input!",
				false, true, 2);
			throw exception();
		}
		tmp.push_back(0);
		if (tmp.length() == 2 && tmp[0] == '@') {
			if (maxlen > 0) {
				//memcpy_s(str, maxlen, tmp.c_str(), tmp.length());
				str[0] = '@'; str[1] = 0;
			}
			return;
		}
		sscanf_s(tmp.c_str(), "%d", &mid);
		if (mid == 0xffffffff || mid <= 0) {
			mMessager.PrintDirect("Invalid Input!", 
				false, true, 2);
			throw exception();
		}
		if (maxlen > 0) {
			memcpy_s(str, maxlen, tmp.c_str(), tmp.length());
		}
		tgt = mid;
	}
	void ReadFloat(float & tgt, int maxlen = -1, SQLCHAR* str = NULL) {
		string tmp;
		float mid = -1;
		getline(cin, tmp);
		if (maxlen > 0 && tmp.length() > maxlen) {
			mMessager.PrintDirect("Invalid Input!",
				false, true, 2);
			throw exception();
		}
		tmp.push_back(0);
		if (tmp.length() == 2 && tmp[0] == '@') {
			if (maxlen > 0) {
				//memcpy_s(str, maxlen, tmp.c_str(), tmp.length());
				str[0] = '@'; str[1] = 0;
			}
			return;
		}
		sscanf_s(tmp.c_str(), "%f", &mid);
		if (mid < 0) {
			mMessager.PrintDirect("Invalid Input!",
				false, true, 2);
			throw exception();
		}
		if (maxlen > 0) {
			memcpy_s(str, maxlen, tmp.c_str(), tmp.length());
		}
		tgt = mid;
	}
};



int main() {
	LibraryManageSystem lms;
	return lms.Run();
}
