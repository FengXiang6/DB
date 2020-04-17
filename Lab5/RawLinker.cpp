#include "RawLinker.h"
#include <iostream>
#include <odbcinst.h>
#include <odbcss.h>

using namespace std;

void RawLinker::Start() 
{
	mErrCode = DEFAULT;
	if (IsLinking()) {
		mErrCode = LINK_ALREADY;
		return;
	}
	SQLAllocEnv(&mEnv);
	SQLAllocConnect(mEnv, &mConn);
	mRet = SQLConnect(mConn,
		mServerName, SQL_NTS,
		mUserName, SQL_NTS,
		mPassword, SQL_NTS);
	if (SQL_SUCCEEDED(mRet)) {
		mLinking = true;
		SQLAllocStmt(mConn, &mStatement);
		mRet = SQLExecDirect(mStatement, (SQLCHAR*)"use lab5_lms;", SQL_NTS);
		if (!SQL_SUCCEEDED(mRet)) {
			ReportError();
			SQLFreeStmt(mStatement, SQL_CLOSE);
			Release();
			return;
		}
		SQLFreeStmt(mStatement, SQL_CLOSE);
		return;
	}
	else {
		SQLFreeConnect(mConn);
		SQLFreeEnv(mEnv);
		mErrCode = LINK_FAIL;
		return;
	}
}

void RawLinker::Release() 
{
	mErrCode = DEFAULT;
	if (IsLinking()) {
		mLinking = false;
		SQLFreeStmt(mStatement, SQL_DROP);
		SQLDisconnect(mConn);
		SQLFreeConnect(mConn);
		SQLFreeEnv(mEnv);
		return;
	}
	else {
		mErrCode = RELEASE_FAIL;
		return;
	}
}

void RawLinker::ReportError()
{
	SQLLEN numRecs = 0;
	SQLGetDiagField(SQL_HANDLE_STMT, mStatement, 0, SQL_DIAG_NUMBER, &numRecs, 0, 0);
	// Get the status records.
	int i = 1;
	SQLRETURN rc2;
	while (i <= numRecs &&
		(rc2 = SQLGetDiagRec(SQL_HANDLE_STMT, mStatement,
			i, mState, &mNativeError, mMsg,
			sizeof(mMsg), &mMsgLen)) != SQL_NO_DATA)
	{
		std::cout << mMsg << std::endl;
		i++;
	}
}

bool RawLinker::IsLinking()
{
	return mLinking;
}

LinkerErr RawLinker::GetLastError() 
{
	return mErrCode;
}

void RawLinker::Execute()
{
	mRet = SQLExecute(mStatement);
	if (SQL_SUCCEEDED(mRet))
	{
		mErrCode = DEFAULT;
	}
	else {
		mErrCode = EXECUTE_ERROR;
	}
}

void RawLinker::Execute(const std::string & sentence)
{
	mRet = SQLExecDirect(mStatement, (SQLCHAR*)sentence.c_str(), SQL_NTS);
	if (SQL_SUCCEEDED(mRet))
	{
		mErrCode = DEFAULT;
	}
	else {
		mErrCode = EXECUTE_ERROR;
	}
}
void RawLinker::Prepare(SQLCHAR* query, SQLSMALLINT len)
{
	mRet = SQLPrepare(mStatement, query, len);
	if (SQL_SUCCEEDED(mRet))
	{
		mErrCode = DEFAULT;
	}
	else {
		mErrCode = PREPARE_ERROR;
	}
}
int RawLinker::GetVarchar(int coln, SQLCHAR* tgt, int maxlen)
{
	SQLINTEGER len;
	SQLBindCol(mStatement, coln, SQL_C_CHAR, tgt, maxlen, &len);
	return len;
}
#define PrintIfFailed(x) {if (!SQL_SUCCEEDED(x)) cout<<#x<<endl;}
int RawLinker::GetFloat(int coln, float& tgt)
{
	SQLINTEGER len;
	SQLHDESC hdesc = NULL;
	SQLGetStmtAttr(mStatement, SQL_ATTR_APP_ROW_DESC, &hdesc, 0, NULL);
	SQLSetDescField(hdesc, coln, SQL_DESC_TYPE, (SQLPOINTER)SQL_C_NUMERIC, 0);
	SQLSetDescField(hdesc, coln, SQL_DESC_PRECISION, (SQLPOINTER)8, 0);
	SQLSetDescField(hdesc, coln, SQL_DESC_SCALE, (SQLPOINTER)2, 0);
	SQLSetDescField(hdesc, coln, SQL_DESC_DATA_PTR, (SQLPOINTER) &(tgt), sizeof(tgt));
	SQLBindCol(mStatement, coln, SQL_C_FLOAT, (SQLPOINTER)&tgt, sizeof(tgt), &len);
	return len;
}

int RawLinker::GetInteger(int coln, unsigned int& tgt)
{
	SQLINTEGER len;
	SQLBindCol(mStatement, coln, SQL_INTEGER, &tgt, 0, &len);
	return len;
}
bool RawLinker::Fetch() {
	return SQLFetch(mStatement) != SQL_NO_DATA;
}
SQLRETURN RawLinker::InVarchar(int coln, SQLCHAR* tgt, int maxlen) {
	SQLRETURN ret = SQLBindParameter(
		mStatement, coln, 
		SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR,
		maxlen, 0, tgt, maxlen, NULL);
	return ret;
}
SQLRETURN RawLinker::InFloat(int coln, float& tgt) {
	SQLHDESC hdesc = NULL;
	SQLGetStmtAttr(mStatement, SQL_ATTR_APP_PARAM_DESC, &hdesc, 0, NULL); 
	SQLSetDescField(hdesc, coln, SQL_DESC_TYPE, (SQLPOINTER)SQL_DECIMAL, 0);
	SQLSetDescField(hdesc, coln, SQL_DESC_PRECISION, (SQLPOINTER)8, 0);
	SQLSetDescField(hdesc, coln, SQL_DESC_SCALE, (SQLPOINTER)2, 0);
	SQLSetDescField(hdesc, coln, SQL_DESC_DATA_PTR, (SQLPOINTER) &(tgt), sizeof(tgt));
	SQLRETURN ret = SQLBindParameter(mStatement, coln, SQL_PARAM_INPUT, SQL_C_FLOAT, SQL_DECIMAL,
		8, 2, &tgt, sizeof(tgt), NULL);
	return ret;
}
SQLRETURN RawLinker::InInteger(int coln, int& tgt) {
	SQLRETURN ret = SQLBindParameter(mStatement, coln,
		SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER,
		0, 0, &tgt, 0, NULL);
	return ret;
}
void RawLinker::CloseStatement() {
	SQLFreeStmt(mStatement, SQL_CLOSE);
}
