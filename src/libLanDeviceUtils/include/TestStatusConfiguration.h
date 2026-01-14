#ifndef _LIBLANDEVICEUTILS_TESTSTATUSCONFIGURATION_H_
#define _LIBLANDEVICEUTILS_TESTSTATUSCONFIGURATION_H_

#include <string>

namespace LDU {

#include <DLLImportExport.h>

class TestStatusConfiguration
{
public:
	LIBLANDEVICEUTILS_API TestStatusConfiguration(void);
	LIBLANDEVICEUTILS_API virtual ~TestStatusConfiguration(void);

//VARIABLES
private:
	unsigned char testStatus;

//METHODS

public:
	LIBLANDEVICEUTILS_API const unsigned char getTestStatus() const;
	LIBLANDEVICEUTILS_API void setTestStatus(const unsigned char testStatus);

	LIBLANDEVICEUTILS_API std::string toString() const;
public:
};
}
#endif
