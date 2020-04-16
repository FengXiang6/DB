#pragma once
#include <string>
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#ifdef _DEBUG
#define private public
#endif

#define MaxConfigStrLen 15
typedef enum
{
	DEFAULT,
	LINK_INITIALIZE_FAIL,
	LINK_ALREADY,
	LINK_FAIL,
	RELEASE_FAIL,
	PREPARE_ERROR,
	EXECUTE_ERROR
} LinkerErr;
class RawLinker {
public:
	bool IsLinking();
	void Execute();
	void Execute(const std::string& sentence);
	void Start();
	void Release();
	void ReportError();
	void Prepare(SQLCHAR* query, SQLSMALLINT len);
	LinkerErr GetLastError();
	int GetVarchar(int coln, SQLCHAR* tgt, int maxlen);
	int GetFloat(int coln, float& tgt);
	int GetInteger(int coln, unsigned int& tgt);
	SQLRETURN InVarchar(int coln, SQLCHAR* tgt, int maxlen);
	SQLRETURN InFloat(int coln, float& tgt);
	SQLRETURN InInteger(int coln, int& tgt);
	bool Fetch();
	void CloseStatement();
private:
	LinkerErr	mErrCode = DEFAULT;
	SQLRETURN	mRet = SQL_SUCCESS;
	SQLHENV		mEnv = nullptr;
	SQLHDBC		mConn = nullptr;
	SQLCHAR		mServerName[MaxConfigStrLen] = "lab5lms";
	SQLCHAR		mUserName[MaxConfigStrLen] = "root";
	SQLCHAR		mPassword[MaxConfigStrLen] = "fxfx123123";
	SQLCHAR		mMsg[SQL_MAX_MESSAGE_LENGTH] = "";
	SQLCHAR		mState[6] = "";
	SQLINTEGER  mNativeError;
	SQLSMALLINT mMsgLen;
	bool		mLinking = false;
	SQLHSTMT	mStatement = nullptr;
};