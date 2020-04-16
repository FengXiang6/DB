#include <Windows.h>
#include <sql.h>
#include <sqltypes.h>
#include <odbcinst.h>
#include <odbcss.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sqlext.h>
using namespace std;
//#define MaxConfigStrLen 15
//#define Len(x) sizeof(x)/sizeof((x)[0])

int main() {
	SQLRETURN mRet;
	SQLHENV mEnv;
	SQLHDBC mConn;
	SQLCHAR mServerName[] = "lab5lms";
	SQLCHAR mUserName[] = "root";
	SQLCHAR mPassword[] = "fxfx123123";
	SQLHSTMT st;
	SQLAllocEnv(&mEnv);
	SQLAllocConnect(mEnv, &mConn);
	
	mRet = SQLConnect(mConn,
		mServerName, SQL_NTS,
		mUserName, SQL_NTS,
		mPassword, SQL_NTS);
	SQLAllocStmt(mConn, &st);
	if (mRet==SQL_SUCCESS||mRet==SQL_SUCCESS_WITH_INFO) {
		mRet = SQLExecDirect(st, (SQLCHAR*)"use lab5_lms;", SQL_NTS);
		mRet = SQLFreeHandle(SQL_HANDLE_STMT, st);

		SQLAllocStmt(mConn, &st);
		SQLINTEGER tgt = 1;
		SQLINTEGER P;
		SQLCHAR sql[] = "select fortest3(?);";
		mRet = SQLPrepare(st, sql, SQL_NTS);
		mRet = SQLBindParameter(st, 1, 
			SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 
			0, 0, &tgt, 0, NULL);
		mRet = SQLExecute(st);
		if (SQL_SUCCEEDED(mRet)) {
			mRet = mRet;
		}
	}
	else {
		SQLFreeConnect(mConn);
		SQLFreeEnv(mEnv);
	}
	return 0;
}