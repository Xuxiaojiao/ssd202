#include "md5.h"
#include <iostream>
 
using namespace std;
 
void PrintMD5(const string &str, MD5 &md5) {
	cout << "MD5(\"" << str << "\") = " << md5.toString() << endl;
}
 
string FileDigest(const string &file) {
 
	ifstream in(file.c_str(), ios::binary);
	if (!in)
		return "";
 
	MD5 md5;
	std::streamsize length;
	char buffer[1024];
	while (!in.eof()) {
		in.read(buffer, 1024);
		length = in.gcount();
		if (length > 0)
			md5.update(buffer, length);
	}
	in.close();
	return md5.toString();
}
 
int main() {
	cout << MD5("abc").toString() << endl;
 
	cout << FileDigest("D:\\staticDll.dll") << endl;
 
	return 0;
}
