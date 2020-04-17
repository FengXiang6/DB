#include "LMS.h"
#include <ctime>

void LibraryManageSystem::Welcome()
{
	mMessager.Append("Welcome to Library Manage System!");
	mMessager.Append("(1)Search         (2)Borrow      (3)Return");
	mMessager.Append("(4)Update by file (5)Card Manage (6)Update manually");
	mMessager.Append("(7)Print borrow record           (8)Show list");
	mMessager.Append("(9)Create or Delete Card         (0)Quit");
	mMessager.Append("Please input your operation code:");
	mMessager.Print(true, true);
}

void LibraryManageSystem::Initialize() 
{
	mLinker.Start();
	if (!mLinker.IsLinking()) {
		throw exception();
	}
}

void LibraryManageSystem::List() 
{
	Query.clear();
	Query.append("select * from book");
	mLinker.CloseStatement();
	mLinker.Execute(Query);
	if (mLinker.GetLastError()) {
		mMessager.PrintDirect("An error occured:", false, true);
		mLinker.ReportError();
		return;
	}
	mMessager.PrintDirect("All of the corresponding books in the library are listed in following table:",
		true, true, 1);
	FullFetch();
	mMessager.PrintDirect("\n\n");
}

void LibraryManageSystem::FullMatch() 
{
	Query = "select * from book where " +
		subCondition(BookID, BookID, "=") + " and " +
		subCondition(BookName, BookName, "=") + " and " +
		subCondition(BookClass, BookClass, "=") + " and " +
		subCondition(Press, Press, "=") + " and " +
		subCondition(Author, Author, "=") + " and " +
		subCondiWithNumber(Years, strMinYear, ">=") + " and " +
		subCondiWithNumber(Years, strMaxYear, "<=") + " and " +
		subCondiWithNumber(Price, strMinPrice, ">=") + " and " +
		subCondiWithNumber(Price, strMaxPrice, "<=") + ";";
	mLinker.CloseStatement();
	mLinker.Execute(Query);
	if (mLinker.GetLastError()) {
		mMessager.PrintDirect("An error occured:", false, true);
		mLinker.ReportError();
		throw std::exception();
	}
}

void LibraryManageSystem::FullFetch() 
{
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
		sprintf_s(tmp, 30, "%-6d", Stock);
		mMessager.MakeTable(tmp, 6, 1);
		mMessager.EndTable();
	}
}

int LibraryManageSystem::FetchBorrowRecord() 
{
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

void LibraryManageSystem::Borrow() 
{
	do {
		mMessager.PrintDirect(
			"Now you are borrowing books...please input corresponding information.");
		try {
			readString(10, CardID);
			if (!notENTER(CardID)) {
				mMessager.PrintDirect("Not accept ENTER now for specifically searching!", false, true);
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
			if (!notENTER(BookID)) {
				mMessager.PrintDirect("Not accept ENTER now for specifically searching!", false, true);
				throw exception();
			}
		}
		catch (exception &e)
		{
			continue;
		}

		//borrow book
		time_t now_time = time(NULL);
		tm* t_tm = localtime(&now_time);
		sprintf_s((char*)BorrowDate, 25,
			"%04d-%02d-%02d %02d:%02d:%02d",
			t_tm->tm_year + 1900, t_tm->tm_mon + 1, t_tm->tm_mday,
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
			return;
		}

		unsigned int state;
		mLinker.GetInteger(1, state);
		mLinker.Fetch();
		if (state == 1) {
			mMessager.PrintDirect("Not found this book in the library!", true, true, 2);
		}
		else if (state == 3) {
			mMessager.PrintDirect("Already borrowed this book!", true, true, 2);
		}
		else if (state == 2) {
			mMessager.PrintDirect("Sorry...the book has no stock.", true, true, 1);
			mMessager.PrintDirect("These are borrowing records of this book:");
			Query.clear();
			Query.append("select CardID, BorrowDate from record where BookID = ? and ReturnDate is null;");
			mLinker.CloseStatement();
			mLinker.Prepare((SQLCHAR*)Query.c_str(), SQL_NTS);
			mLinker.InVarchar(1, BookID, 10);
			mLinker.Execute();
			mLinker.GetVarchar(1, CardID, 10);
			mLinker.GetVarchar(2, BorrowDate, 25);
			
			mMessager.MakeTable("CardID", 10 + 3, 1);
			mMessager.MakeTable("BorrowDate", 25, 1);
			mMessager.EndTable();
			while (mLinker.Fetch()) {
				mMessager.MakeTable((char*)CardID, 10 + 3, 1);
				mMessager.MakeTable((char*)BorrowDate, 25, 1);
				mMessager.EndTable();
			}
			mMessager.PrintDirect("\n\n");
		}
		else {
			mMessager.PrintDirect("Borrow the book successfully!", true, true, 2);
		}
		break;
	} while (1);
}

void LibraryManageSystem::SearchForBook() 
{
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
			continue;
		}
	} while (0);

	try {
		FullMatch();
	}
	catch (exception& e) {
		return;
	}
	mMessager.PrintDirect("All of the corresponding books in the library are listed in following table:",
		true, true, 1);
	FullFetch();
	mMessager.PrintDirect("\n\n");
}

void LibraryManageSystem::CardManage()
{
	SQLCHAR storeCID[10 + 1];
	try {
		readString(10, CardID);
	}
	catch (exception& e) {
		return;
	}
	if (!notENTER(CardID)) {
		mMessager.PrintDirect("Please specify the CardID", false, true, 2);
		return;
	}

	Query.clear();
	Query.append("select count(*) from card where ");
	Query.append(noEnterCondition(CardID, CardID, "="));
	mLinker.CloseStatement();
	mLinker.Execute(Query);
	unsigned int exist;
	mLinker.GetInteger(1, exist);
	mLinker.Fetch();
	if (!exist) {
		mMessager.PrintDirect("Not found this card!", false, true, 2);
		return;
	}

	Query.clear();
	Query.append("select * from card where");
	Query.append(noEnterCondition(CardID, CardID, "="));
	mLinker.CloseStatement();
	mLinker.Execute(Query);
	
	memcpy(storeCID, CardID, sizeof(CardID));

	mLinker.GetVarchar(1, CardID, 10);
	mLinker.GetVarchar(2, OwnerName, 20);
	mLinker.GetVarchar(3, Department, 20);
	mLinker.GetVarchar(4, CardType, 10);

	mMessager.PrintDirect("This is the information of this card:", false, true, 1);

	mMessager.MakeTable("CardID", 10 + 3, 1);
	mMessager.MakeTable("OwnerName", 20, 1);
	mMessager.MakeTable("Department", 20, 1);
	mMessager.MakeTable("CardType", 10, 1);
	mMessager.EndTable();

	while (mLinker.Fetch()) {
		mMessager.MakeTable((char*)CardID, 10 + 3, 1);
		mMessager.MakeTable((char*)OwnerName, 20, 1);
		mMessager.MakeTable((char*)Department, 20, 1);
		mMessager.MakeTable((char*)CardType, 10, 1);
		mMessager.EndTable();
	}

	mMessager.PrintDirect("\n");

	unsigned int choose;
	mMessager.PrintDirect("Edit information: 1 for OwnerName, 2 for Department, 3 for type, ENTER for quit.");
	try {
		readIntegerX(choose);
	}
	catch (...) {
		return;
	}
	if (choose==0xffffffff) {
		return;
	}
	else if (choose == 1) {
		do {
			try {
				readString(20, OwnerName);
			}
			catch (...) {
				continue;
			}
			if (!notENTER(OwnerName)) {
				mMessager.PrintDirect("No ENTER! Please specify the information you want to edit!");
				continue;
			}
			break;
		} while (1);
		Query.clear();
		Query.append("update card set OwnerName = '");
		Query.append(string((char*)OwnerName));
		Query.append("' where "); Query.append(noEnterCondition(CardID, storeCID, "="));
		Query.append(";");
		mLinker.CloseStatement();
		mLinker.Execute(Query);
		//cout << Query << endl << endl;
		if (mLinker.GetLastError()) {
			mMessager.PrintDirect("An error occured:", true, true);
			mLinker.ReportError();
			mMessager.PrintDirect("\n\n");
			return;
		}
		mMessager.PrintDirect("successfully update", true, true, 2);
	}
	else if (choose == 2) {
		do {
			try {
				readString(20, Department);
			}
			catch (...) {
				continue;
			}
			if (!notENTER(Department)) {
				mMessager.PrintDirect("No ENTER! Please specify the information you want to edit!");
				continue;
			}
			break;
		} while (1);
		Query.clear();
		Query.append("update card set Department = '");
		Query.append(string((char*)Department));
		Query.append("' where "); Query.append(noEnterCondition(CardID, storeCID, "="));
		Query.append(";");
		mLinker.CloseStatement();
		mLinker.Execute(Query);

		if (mLinker.GetLastError()) {
			mMessager.PrintDirect("An error occured:", true, true);
			mLinker.ReportError();
			mMessager.PrintDirect("\n\n");
			return;
		}
		mMessager.PrintDirect("successfully update", true, true, 2);
	}
	else if (choose == 3) {
		do {
			try {
				mMessager.PrintDirect("Input Teacher or Student.");
				readString(10, CardType);
			}
			catch (...) {
				continue;
			}
			if (!notENTER(CardType)) {
				mMessager.PrintDirect("No ENTER! Please specify the information you want to edit!");
				continue;
			}
			break;
		} while (1);
		Query.clear();
		Query.append("update card set CardType = '");
		Query.append(string((char*)CardType));
		Query.append("' where "); Query.append(noEnterCondition(CardID, storeCID, "="));
		Query.append(";");
		mLinker.CloseStatement();
		mLinker.Execute(Query);

		if (mLinker.GetLastError()) {
			mMessager.PrintDirect("An error occured:", true, true);
			mLinker.ReportError();
			mMessager.PrintDirect("\n\n");
			return;
		}
		mMessager.PrintDirect("successfully update", true, true, 2);

	}
}

void LibraryManageSystem::CreateCard()
{
	unsigned choose;
	mMessager.PrintDirect("Create or Delete Card -- 0 for creating, 1 for deleting, Enter for quit.");
	try {
		readIntegerX(choose);
	}
	catch (...) {
		return;
	}
	if (choose == 0xffffffff) return;
	else if (choose == 0) {
		try {
			readString(10, CardID);
			readString(20, OwnerName);
			readString(20, Department);
			mMessager.PrintDirect("please input teacher or student for type information.");
			readString(10, CardType);
		}
		catch (...) {
			return;
		}
		if (!(notENTER(CardID)&&notENTER(OwnerName)&&notENTER(Department)&&notENTER(CardType))) {
			mMessager.PrintDirect("Please specify the CardID!", false, true, 2);
			return;
		}
		Query.clear();
		Query.append("insert into card values(?,?,?,?);");
		mLinker.CloseStatement();
		mLinker.Prepare((SQLCHAR*)Query.c_str(), SQL_NTS);
		mLinker.InVarchar(1, CardID, 10);
		mLinker.InVarchar(2, OwnerName, 20);
		mLinker.InVarchar(3, Department, 20);
		mLinker.InVarchar(4, CardType, 10);
		mLinker.Execute();
		if (mLinker.GetLastError()) {
			mMessager.PrintDirect("An error occured:", true, true);
			mLinker.ReportError();
			mMessager.PrintDirect("\n\n");
			return;
		}
		else {
			mMessager.PrintDirect("Successfully create a new card", true, true, 2);
			return;
		}
	}
	else if (choose == 1) {
		try {
			readString(10, CardID);
		}
		catch (...) {
			return;
		}
		if (!notENTER(CardID)) {
			mMessager.PrintDirect("please specify the CardID you want to delete.", false, true, 2);
			return;
		}
		Query.clear();
		Query.append("select delete_card(?);");
		mLinker.CloseStatement();
		mLinker.Prepare((SQLCHAR*)Query.c_str(), SQL_NTS);
		mLinker.InVarchar(1, CardID, 10);
		mLinker.Execute();
		unsigned int state;
		mLinker.GetInteger(1, state);
		mLinker.Fetch();
		if (state == 1) {
			mMessager.PrintDirect("This card does not exist!", true, true, 2);
			return;
		}
		else if (state == 2) {
			mMessager.PrintDirect("Please return the borrowed books first!", true, true, 2);
			return;
		}
		else if (state == 3) {
			mMessager.PrintDirect("Delete the card successfully!", true, true, 2);
			return;
		}
	}
}

void LibraryManageSystem::ReturnBook() 
{
	do {
		mMessager.PrintDirect(
			"Now you are returning the book...please input corresponding information.");
		try {
			readString(10, CardID);
			if (!notENTER(CardID)) {
				mMessager.PrintDirect("Not accept ENTER now for specifically searching!", false, true, 2);
				throw exception();
			}

		}
		catch (exception &e)
		{
			continue;
		}
		break;
	} while (1);

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
	mLinker.Fetch();

	if (state == 2){
		mMessager.PrintDirect("Return the book successfully!", true, true, 2);
	}
	else {
		mMessager.PrintDirect("Failed to return the book!", true, true, 2);
	}
}

void LibraryManageSystem::UpdateMan() 
{
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
			//readIntegerXYZ(Stock, 10, strS);
		}
		catch (exception &e)
		{
			return;
		}
		if (!(notENTER(BookID) && notENTER(BookName) && notENTER(BookClass) &&
			notENTER(Press) && notENTER(Author) && notENTER(strMinYear) &&
			notENTER(strMinPrice) && notENTER(strT)))
		{
			mMessager.PrintDirect("Do not input ENTER! Specify the information!");
			continue;
		}
		else break;
	} while (1);

	mLinker.CloseStatement();
	mLinker.Execute("call new_execute_queue_prepare();");
	if (mLinker.GetLastError()) {
		mLinker.ReportError();
		return;
	}

	Query.clear();
	Query.append(
		"select update_library_book(?,?,?,?,?,?,?,?,?);");

	mLinker.CloseStatement();
	mLinker.Prepare((SQLCHAR*)Query.c_str(), SQL_NTS);

	int lY = Years;
	int lT = TotalCollect;
	int lS = Stock = lT;

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
	SQLCHAR State[10 + 1];
	mLinker.GetInteger(1, state);
	mLinker.Fetch();
	if (state == 1 || state == 2) {
		mMessager.PrintDirect("This book has been modified", true, true, 1);
		mLinker.CloseStatement();
		mLinker.Execute("select * from update_mid_table;");

		
		mLinker.GetVarchar(1, BookID, 10 + 1);
		mLinker.GetVarchar(2, BookName, 30 + 1);
		mLinker.GetVarchar(3, State, 10 + 1);

		mMessager.MakeTable("BookID", 10 + 5, 1);
		mMessager.MakeTable("BookName", 30, 1);
		mMessager.MakeTable("State", 10, 1);
		mMessager.EndTable();

		while (mLinker.Fetch()) {
			mMessager.MakeTable((char*)BookID, 10 + 5, 1);
			mMessager.MakeTable((char*)BookName, 30, 1);
			mMessager.MakeTable((char*)State, 10, 1);
			mMessager.EndTable();
		}
		mMessager.PrintDirect("\n", false, false, 1);
	}
}

void LibraryManageSystem::UpdateFile()
{
	do {
		mMessager.PrintDirect("Please input the data file location.");
		try {
			readString(40, DataFilePath);
		}
		catch (exception& e) {
			mMessager.PrintDirect("Falied to input file location", false, false, 2);
			continue;
		}

		if (!notENTER(DataFilePath)) {
			mMessager.PrintDirect("Please specify the file location!");
			continue;
		}

	} while (0);

	FILE* fp = fopen((char*)DataFilePath, "r");
	if (fp == NULL) {
		mMessager.PrintDirect("Can not open the file!", false, true, 2);
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
			"%[^,], %[^,], %[^,], %[^,], %d, %[^,], %f, %d",
			BookID, BookName, BookClass,
			Press, &lY, Author,
			&Price, &lT);
		if (ret == EOF)break;
		
		lS = lT;
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
	mLinker.GetVarchar(3, (SQLCHAR*)Message, 10 + 1);

	mMessager.MakeTable("BookID", 10 + 5, 1);
	mMessager.MakeTable("BookName", 30, 1);
	mMessager.MakeTable("State", 10, 1);
	mMessager.EndTable();

	while (mLinker.Fetch()) {
		mMessager.MakeTable((char*)BookID, 10 + 5, 1);
		mMessager.MakeTable((char*)BookName, 30, 1);
		mMessager.MakeTable(Message, 10, 1);
		mMessager.EndTable();
	}
	mMessager.PrintDirect("\n", false, false, 1);

	fclose(fp);
}

void LibraryManageSystem::ListBorrowRecord()
{
	bool success = false;
	do {
		mMessager.PrintDirect(
			"Now you are checking all of your borrow records...please input corresponding information.");
		try {
			readString(10, CardID);
			if (!notENTER(CardID)) {
				mMessager.PrintDirect("Not accept @ now for specifically searching!", false, true);
				throw exception();
			}
		}
		catch (exception &e)
		{
			continue;
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
			if (ReturnDate[0] != '2') {
				memcpy((char*)ReturnDate, "Unknow", 7);
			}
			mMessager.MakeTable((char*)CardID, 10 + 3, 1);
			mMessager.MakeTable((char*)BookID, 10 + 3, 1);
			mMessager.MakeTable((char*)BorrowDate, 25 + 1, 1);
			mMessager.MakeTable((char*)ReturnDate, 25 + 1, 1);
			mMessager.EndTable();
			memcpy((char*)ReturnDate, "Unknow", 7);
		}
		mMessager.PrintDirect("", false, false, 2);
		break;
	} while (1);
}

void LibraryManageSystem::Function() {
	mOpcode = -1;
	string args;
	getline(cin, args);
	if (args.length() != 1 || args[0] - '0' < 0 || args[0] - '9'>0 || !notENTER(args)) {
		mMessager.PrintDirect("Please input a operation code of integer in [0...9]!",
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
		SearchForBook();
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
		CardManage();
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
	else if (mOpcode == 9) {
		CreateCard();
	}
}

void LibraryManageSystem::ReadString(int maxlen, SQLCHAR* tgt) {
	string tmp;
	getline(cin, tmp);
	tmp.push_back(0);
	if (tmp.length() > maxlen + 1 || tmp.length() < 1) {
		mMessager.PrintDirect("Too much or too less characters!",
			false, true, 2);
		throw exception();
	}
	memcpy(tgt, tmp.c_str(), tmp.length());
}

void LibraryManageSystem::ReadInteger(unsigned int & tgt, int maxlen, SQLCHAR* str) 
{
	string tmp;
	int mid = 0xffffffff;
	getline(cin, tmp);
	tmp.push_back(0);
	if (maxlen > 0 && tmp.length() > maxlen + 1) {
		mMessager.PrintDirect("Invalid Input!",
			false, true, 2);
		throw exception();
	}

	if (!notENTER(tmp)) {
		if (maxlen > 0) {
			//memcpy_s(str, maxlen, tmp.c_str(), tmp.length());
			str[0] = 0;
		}
		return;
	}
	sscanf_s(tmp.c_str(), "%d", &mid);
	if (mid == 0xffffffff || mid < 0) {
		mMessager.PrintDirect("Invalid Input!",
			false, true, 2);
		throw exception();
	}
	if (maxlen > 0) {
		memcpy_s(str, maxlen, tmp.c_str(), tmp.length());
	}
	tgt = mid;
}

void LibraryManageSystem::ReadFloat(float & tgt, int maxlen, SQLCHAR* str) 
{
	string tmp;
	float mid = -1;
	getline(cin, tmp);
	tmp.push_back(0);
	if (maxlen > 0 && tmp.length() > maxlen + 1) {
		mMessager.PrintDirect("Invalid Input!",
			false, true, 2);
		throw exception();
	}
	if (!notENTER(tmp)) {
		if (maxlen > 0) {
			//memcpy_s(str, maxlen, tmp.c_str(), tmp.length());
			str[0] = 0;
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

int LibraryManageSystem::Run()
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
		//mMessager.PrintDirect("Press any key to continue.");
	}

}

LibraryManageSystem::~LibraryManageSystem()
{
	if (mLinker.IsLinking())
		mLinker.Release();
}