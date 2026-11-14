#ifndef EQ3CONFIG_MODULES_MD5_MD5_H
#define EQ3CONFIG_MODULES_MD5_MD5_H

typedef unsigned int uint4;
typedef unsigned short int uint2;
typedef unsigned char uchar;

char* PrintMD5(uchar md5Digest[16]);
char* MD5String(char* szString);
char* MD5File(char* szFilename);
char* MD5File(char* szFilename, uchar* aKeyString, uint4 nKeyLen);

class md5
{
public:
	md5();
	~md5();

	void Init();
	void Update(uchar* chInput, uint4 nInputLen);
	void Finalize();
	uchar* Digest() { return m_Digest; }

private:
	void* m_Context;
	uchar m_Digest[16];
	bool m_Finalized;
};

#endif
