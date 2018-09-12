#include "md5Handler.h"
const char HEX[16] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'a', 'b',
	'c', 'd', 'e', 'f'
};

/* Default construct. */
MD5Handler::MD5Handler(){
	m_Res.reserve(1024);
}

/* Updating the context with a string. */
void MD5Handler::update(const string &str) {
	MD5_CTX ctx;
	MD5_Init(&ctx);
	MD5_Update(&ctx,str.c_str(), str.size());
	MD5_Final((unsigned char*)m_Res.data(), &ctx);
}

/* Convert byte array to hex string. */
string MD5Handler::bytesToHexString(const byte *input, size_t length) {
	string str;
	str.reserve(length << 1);
	for(size_t i = 0; i < length; i++) {
		int t = input[i];
		int a = t / 16;
		int b = t % 16;
		str.append(1, HEX[a]);
		str.append(1, HEX[b]);
	}
	return str;
}

/* Convert digest to string value */
string MD5Handler::toString() {
	return bytesToHexString((const byte*)m_Res.c_str(), 16);
}