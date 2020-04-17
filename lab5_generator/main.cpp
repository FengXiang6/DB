#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <ctime>
using namespace std;
int main() {
	freopen("D:/Project/VisualStudio/DB/data.csv", "w", stdout);
	string ClassSet[] = {
		"biology","geography","chemistry","physics","math","computer science",
		"economic","literature","philosophy","logic"
	};
	string PressSet[] = {
		"ZJU Press","THU Press","PKU Press","FDU Press","SJTU Press",
		"China Press","Zhejiang Press","Perlin Press","LA Press","NewYork Press",
		"NKU Press","NJU Press"
	};
	string AuthorSet[] = {
		"Jane","Wu Zhaohui","Peter","Alice","Mike","Tom","Zheng Qiang","Gu Zhehan",
		"Wang Junjie","Wang Haiwei","Ren Lianhai","Shen Chenwei","Jiang Yanbo",
		"Zhang Shimin","Tan Jiajie","Liu Junliang","Guo Ziyang","Alex","Haha",
		"Jin Yilong", "Yang Yuke"
	};
	int classlen = sizeof(ClassSet) / sizeof(ClassSet[0]);
	int presslen = sizeof(PressSet) / sizeof(PressSet[0]);
	int authorlen = sizeof(AuthorSet) / sizeof(AuthorSet[0]);

	printf("BookID, BookName, BookClass, Press, Years, Author, Price, TotalCollect, Stock\n");

	int gn = 150;
	srand(unsigned(time(NULL)));
	for (int i = 0; i < gn; i++) {
		int bookid = rand() % 10000000000;
		printf("%0*d, ", 10, bookid);
		printf("bookname-%d, ", i);
		int classIndex = rand() % classlen;
		printf("%s, ", ClassSet[classIndex].c_str());
		int pressIndex = rand() % presslen;
		printf("%s, ", PressSet[pressIndex].c_str());
		printf("%d, ", rand() % 115 + 1900);
		int authorIndex = rand() % authorlen;
		printf("%s, ", AuthorSet[authorIndex].c_str());
		float price = float(rand() % 1000 + 100) / 100.0;
		printf("%.2f, ", price);
		int tot = rand() % 7 + 1;
		printf("%d\n", tot);
	}
}