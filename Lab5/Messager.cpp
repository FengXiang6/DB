#include "Messager.h"
#include <iostream>
#include <ctime>
using namespace std;

bool Messager::Append(const std::string & msg)
{
	if (mSp == MaxMsgQueueSize) {
		return false;
	}
	else {
		mMsgQueue[mSp++] = msg;
		return true;
	}
}

void Messager::Clear()
{
	mSp = 0;
}

void Messager::Print(bool mtime, bool info, int nl)
{
	PrintInfo(mMsgQueue, mSp, mtime, info, nl);
	Clear();
}

void Messager::PrintDirect(const std::string & msg, bool mtime, bool info, int nl)
{
	Clear();
	Append(msg);
	Print(mtime, info, nl);
}

void Messager::PrintInfo(const std::string * message, int num, bool mtime, bool info, int nl)
{
	if (mtime) {
		time_t now_time = time(NULL);
		tm* t_tm = localtime(&now_time);
		char ch[30];
		for (int i = 0; i < MaxInfoStrLen; i++)std::cout << "=";
		std::cout << std::endl;
		for (int i = 0; i < (MaxInfoStrLen - 25) / 2; i++)std::cout << " ";
		asctime_s(ch, t_tm);
		ch[strlen(ch)] = 0;
		std::cout << ch;
	}
	if (info) {
		for (int i = 0; i < (MaxInfoStrLen - 6) / 2; i++) std::cout << "*";
		std::cout << "|INFO|";
		for (int i = 0; i < (MaxInfoStrLen - 6) / 2; i++) std::cout << "*";
		std::cout << std::endl;
	}
	for (int i = 0; i < num; i++) {
		std::cout << message[i] << std::endl;
	}
	for (int i = 0; i < nl; i++) {
		std::cout << std::endl;
	}
	return;
}
void Messager::MakeTable(const std::string token, int maxlen, bool splitor)
{
	if (token.length() > maxlen) {
		for (int i = 0; i < maxlen; i++)cout << token[i];
	}
	cout << token;
	for (int i = 0; i < maxlen - token.length(); i++) cout << " ";
	mAccu += maxlen + (splitor ? 1 : 0);
	if (splitor) {
		cout << "|";
		mTableLen[mTsp] = mAccu - mTsp - 1;
		mTsp++;
	}
}
void Messager::EndTable()
{
	cout << endl;
	for (int i = 0, j = 0; i < mTsp; i++) {
		for (; j < mTableLen[i]; j++)cout << "=";
		cout << "|";
	}
	cout << endl;
	mTsp = 0;
	mAccu = 0;
}