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

typedef UCS32 (*PFN_UUCHARFUNC)(UCS32 chr);


typedef struct UUStr
{
	UTF8 *ptr;
	ssize_t byteLength;
	ssize_t capacity;
	char flags;
}UUStr;

#define UU_HAS_BYTELENGTH (1 << 0)
#define UU_MUST_FREE (1 << 1)

#define UUSTR_STACK(_str, _capacity) \
	UUStr _str; \
	UTF8 _uuBuff##_str[_capacity + 1]; \
	_str.ptr = _uuBuff##_str; \
	_str.flags = UU_HAS_BYTELENGTH; \
	_str.byteLength = 0; \
	_str.capacity = _capacity; \

#define UUSTR_STACK_CSTR(_str, _cstr) \
	UUStr _str; \
	UTF8 _uuBuff##_str[] = _cstr; \
	_str.ptr = _uuBuff##_str; \
	_str.flags = UU_HAS_BYTELENGTH; \
	_str.byteLength = sizeof(_cstr) - 1; \
	_str.capacity = sizeof(_cstr) - 1; \

#define UUSTR_HEAP(_str, _byteCapacity) \
	UUStr _str; \
	uuCreateEmpty(&_str, _byteCapacity); \

/*[X]*/ int uuCreateFromCSTR(UUStr *str, const char *cstr, ssize_t byteLength);
/*[X]*/ int uuCreateFromUTF8(UUStr *str, const UTF8 *cstr, ssize_t byteLength, ssize_t charLength);
/*[X]*/ int uuCreateFromUTF16(UUStr *str, const UTF16 *in, ssize_t charLength);
/*[X]*/ int uuCreateEmpty(UUStr *str, ssize_t byteCapacity);
/*[X]*/ int uuCreateByClone(UUStr *str, UUStr *input);


#if WCHAR_MAX == 0xffff
/*[X]*/ #define uuCreateFromWSTR(_str, _in, _charLength) uuCreateFromUTF16(_str, (const UTF16 *) _in, _charLength)
#else
/*[ ]*/ #define uuCreateFromWSTR(_str, _in, _charLength) uuCreateFromUCS32(_str, (const UCS32 *) _in, _charLength)
#endif

/*[ ]*/ int uuValidate(UUStr *str);
/*[X]*/ int uuCreateFromUCS32(UUStr *str, const UCS32 *in, ssize_t charLength, ssize_t byteLength);
/*[X]*/ int uuConvertToUTF16(UUStr *str, const UTF16 *output, ssize_t byteLength, ssize_t *outCharLength);
/*[X]*/ int uuConvertToUCS32(UUStr *str, const UCS32 *output, ssize_t byteLength, ssize_t *outCharLength);
/*[X]*/ int uuCompare(UUStr *str, UUStr *str2);
/*[X]*/ ssize_t uuFindFirstOf(UUStr *str, UUStr *needle);
/*[X]*/ ssize_t uuFindLastOf(UUStr *str, UUStr *needle);
/*[X]*/ int uuSubString(UUStr *str, UUStr *input, ssize_t byteOffset, ssize_t byteLength);
/*[X]*/ ssize_t uuOffsetOf(UUStr *str, UCS32 chr);
/*[X]*/ int uuReplace(UUStr *str, UUStr *what, UUStr *with);
/*[X]*/ int uuAppend(UUStr *str, UUStr *second);
/*[X]*/ int uuAppend(UUStr *str, const char *second);
/*[ ]*/ int uuAppend(UUStr *str, long long value, int radix);


/*[X]*/ void uuClear(UUStr *str);
/*[X]*/ int uuAdjustCapacity(UUStr *str, ssize_t byteCapacity);
/*[X]*/ UCS32 uuCharAt(UUStr *str, ssize_t byteOffset);
/*[X]*/ UCS32 uuReadNextChar(UUStr *str, ssize_t *byteOffset);
/*[X]*/ ssize_t uuCharLength(UUStr *str);
/*[X]*/ void uuFree(UUStr *str);
/*[X]*/ int uuIsEmpty(UUStr *str);
/*[X]*/ ssize_t uuLastOffsetOf(UUStr *str, UCS32 chr);
/*[X]*/ ssize_t uuFirstOffsetOf(UUStr *str, UCS32 chr);
/*[X]*/ int uuStartsWith(UUStr *str, UUStr *with);
/*[X]*/ int uuTransform (UUStr *str, PFN_UUCHARFUNC pfn);
/*[X]*/ #define uuCSTR(_str) ((const char *) (_str)->ptr)
/*[X]*/ #define uuByteLength(_str) ((ssize_t)((_str)->byteLength))


#endif