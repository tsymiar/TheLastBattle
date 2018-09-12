#pragma once

#include <string>
#include <fstream>
#include <openssl/md5.h>

using namespace std;
/* Type define */
typedef unsigned char byte;
typedef unsigned long ulong;

/* MD5 declaration. */
class MD5Handler {
public:
	MD5Handler();
	void update(const string &str);
	string toString();
private:
	string bytesToHexString(const byte *input, size_t length);
private:
	string m_Res;
};

