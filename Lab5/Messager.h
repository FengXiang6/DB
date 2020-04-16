#pragma once
#include <string>

#define MaxMsgQueueSize 10
#define MaxInfoStrLen 41
class Messager {
public:
	bool Append(const std::string& msg);
	void Clear();
	void Print(bool mtime = false, bool info = false, int nl = 0);
	void PrintDirect(const std::string& msg, 
		bool mtime = false, bool info = false, int nl = 0);
	void MakeTable(const std::string token, int maxlen, bool splitor = false);
	void EndTable();
private:
	std::string mMsgQueue[MaxMsgQueueSize];
	int mSp = 0;
	int mTsp = 0;
	int mAccu = 0;
	int mTableLen[10] = {0};
	void PrintInfo(const std::string* message, int num,
		bool mtime = false, bool info = false, int nl = 0);
};