#ifndef __UUNICODE_H__
#define __UUNICODE_H__

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef UINT8 UTF8;
typedef UINT16 UTF16;
typedef UINT32 UCS32;

typedef SSIZE_T ssize_t;

#else
#include <sys/types.h>
typedef u_int8_t UTF8;
typedef u_int16_t UTF16;
typedef u_int32_t UCS32;
#endif

#include <wchar.h>

typedef struct UUStr
{
	UTF8 *ptr;
	ssize_t byteLength;
	ssize_t charLength;
	char flags;
}UUStr;

/*[X]*/ int uuCreateFromCSTR(UUStr *str, const char *cstr, ssize_t byteLength);
/*[X]*/ int uuCreateFromUTF8(UUStr *str, const UTF8 *cstr, ssize_t byteLength, ssize_t charLength);
/*[X]*/ int uuCreateFromUTF16(UUStr *str, const UTF16 *in, ssize_t charLength);

#if WCHAR_MAX == 0xffff
#define uuCreateFromWSTR(_str, _in, _charLength) uuCreateFromUTF16(_str, (const UTF16 *) _in, _charLength)
#else
#define uuCreateFromWSTR(_str, _in, _charLength) uuCreateFromUCS32(_str, (const UCS32 *) _in, _charLength)
#endif

/*[X]*/ int uuCreateFromUCS32(UUStr *str, const UCS32 *in, ssize_t charLength, ssize_t byteLength);
/*[X]*/ int uuConvertToUTF16(UUStr *str, const UTF16 *output, ssize_t byteLength, ssize_t *outCharLength);
/*[ ]*/ int uuConvertToUCS32(UUStr *str, const UCS32 *output, ssize_t byteLength, ssize_t *outCharLength);
/*[X]*/ int uuCompare(UUStr *str, UUStr *str2);
/*[X]*/ void uuClone(UUStr *str, UUStr *input);
/*[X]*/ ssize_t uuFind(UUStr *str, UUStr *needle);
/*[ ]*/ ssize_t uuRFind(UUStr *str, UUStr *needle);
/*[X]*/ int uuSubString(UUStr *str, UUStr *input, ssize_t byteOffset, ssize_t byteLength);
/*[X]*/ ssize_t uuOffsetOf(UUStr *str, UCS32 chr);
/*[X]*/ int uuReplace(UUStr *str, UUStr *input, UUStr *what, UUStr *with);
/*[X]*/ void uuAppend(UUStr *str, UUStr *first, UUStr *second);
/*[X]*/ UCS32 uuCharAt(UUStr *str, ssize_t byteOffset);
/*[X]*/ UCS32 uuReadNextChar(UUStr *str, ssize_t *byteOffset);
/*[X]*/ ssize_t uuCharLength(UUStr *str);
/*[X]*/ ssize_t uuByteLength(UUStr *str);
/*[X]*/ void uuFree(UUStr *str);
/*[ ]*/ const char *uuAsCSTR(UUStr *str);
/*[X]*/ int uuIsEmpty(UUStr *str);
/*[ ]*/ ssize_t uuRIndexOf(UUStr *str, UCS32 chr);
/*[ ]*/ int uuStartsWith(UUStr *str, UUStr *with);
/*[ ]*/ void uuToLower(UUStr *str, UUStr *input);
/*[ ]*/ void uuToUpper(UUStr *str, UUStr *input);

#endif