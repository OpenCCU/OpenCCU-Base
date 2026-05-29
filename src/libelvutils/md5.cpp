#include "md5.h"

#include <openssl/evp.h>

#include <cstdio>
#include <cstring>

namespace {

EVP_MD_CTX* toContext(void* context) {
	return static_cast<EVP_MD_CTX*>(context);
}

}  // namespace

md5::md5() : m_Context(nullptr), m_Digest{0}, m_Finalized(false) {
	Init();
}

md5::~md5() {
	if (m_Context) {
		EVP_MD_CTX_free(toContext(m_Context));
		m_Context = nullptr;
	}
}

void md5::Init() {
	if (m_Context) {
		EVP_MD_CTX_free(toContext(m_Context));
	}

	m_Context = EVP_MD_CTX_new();
	std::memset(m_Digest, 0, sizeof(m_Digest));
	m_Finalized = false;

	if (m_Context) {
		EVP_DigestInit_ex(toContext(m_Context), EVP_md5(), nullptr);
	}
}

void md5::Update(const uchar* chInput, uint4 nInputLen) {
	if (!m_Context || !chInput || nInputLen == 0 || m_Finalized) {
		return;
	}

	EVP_DigestUpdate(toContext(m_Context), chInput, nInputLen);
}

void md5::Finalize() {
	if (!m_Context || m_Finalized) {
		return;
	}

	unsigned int digestLength = 0;
	EVP_DigestFinal_ex(toContext(m_Context), m_Digest, &digestLength);
	if (digestLength < sizeof(m_Digest)) {
		std::memset(m_Digest + digestLength, 0, sizeof(m_Digest) - digestLength);
	}
	m_Finalized = true;
}

/*static*/ std::string md5::PrintMD5(const uchar md5Digest[16]) {
	char buffer[33];
	for (size_t i = 0; i < 16; ++i) {
		std::snprintf(buffer + (i * 2), 3, "%02X", md5Digest[i]);
	}
	buffer[32] = '\0';
	return std::string(buffer);
}

/*static*/ std::string md5::MD5String(const char* szString) {
	if (!szString) {
		return "";
	}

	md5 alg;
	alg.Update(reinterpret_cast<const uchar*>(szString), std::strlen(szString));
	alg.Finalize();
	return PrintMD5(alg.Digest());
}

/*static*/ std::string md5::MD5File(const char* szFilename) {
	if (!szFilename) {
		return "";
	}

	FILE* file = std::fopen(szFilename, "rb");
	if (!file) {
		return "";
	}

	md5 alg;
	uchar buffer[1024];
	size_t bytesRead = 0;
	while ((bytesRead = std::fread(buffer, 1, sizeof(buffer), file)) > 0) {
		alg.Update(buffer, static_cast<uint4>(bytesRead));
	}
	std::fclose(file);

	alg.Finalize();
	return PrintMD5(alg.Digest());
}
