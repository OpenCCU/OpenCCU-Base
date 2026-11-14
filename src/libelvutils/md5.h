#ifndef _MD5_H_
#define _MD5_H_

#include "dllexport.h"
#include <string>

typedef unsigned int uint4;
typedef unsigned short int uint2;
typedef unsigned char uchar;

class ELVUTILS_DLLEXPORT md5
{
public:
	md5();
	~md5();

	void Init();
	void Update(const uchar* chInput, uint4 nInputLen);
	void Finalize();
	const uchar* Digest() const { return m_Digest; }

	static std::string PrintMD5(const uchar md5Digest[16]);
	static std::string MD5String(const char* szString);
	static std::string MD5File(const char* szFilename);

private:
	void* m_Context;
	uchar m_Digest[16];
	bool m_Finalized;
};

#endif
