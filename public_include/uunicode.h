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

#define UUSTACK(_str, _capacity) \
	UTF8 _uuBuff##_str[_capacity + 1]; \
	UUStr _str = { _uuBuff##_str, 0, _capacity, UU_HAS_BYTELENGTH }; \

#define UUSTACKCSTR(_str, _cstr) \
	UTF8 _uuBuff##_str[] = _cstr; \
	UUStr _str = { _uuBuff##_str, sizeof(_cstr) - 1, sizeof(_cstr) - 1, UU_HAS_BYTELENGTH }; \

/*
Deprecated */
#define UUSTACKWSTR(_str, _wstr) \
	const wchar_t _uuwBuff##_str[] = _wstr; \
	UTF8 _uuBuff##_str[((sizeof(_wstr) / sizeof(wchar_t)) * 4) + 1]; \
	UUStr _str = { _uuBuff##_str, 0, ((sizeof(_wstr) / sizeof(wchar_t)) * 4) + 1, UU_HAS_BYTELENGTH }; \
	uuAppend(&_str, _wstr); \

#define UUHEAP(_str, _byteCapacity) \
	UUStr _str; \
	__uuInitHeap(&_str, _byteCapacity); \

#define UUMEMBER_SDECL(_str, _byteCapacity) \
	UUStr _str; \
	UTF8 _uuBuff##_str[_byteCapacity + 1]; \

#define UUMEMBER_SINIT(_str) \
	(this->_str).ptr = this->_uuBuff##_str; \
	(this->_str).flags = UU_HAS_BYTELENGTH; \
	(this->_str).byteLength = 0; \
	(this->_str).capacity = sizeof(this->_uuBuff##_str) - 1; \

#define UUMEMBER_DECL(_str) \
	UUStr _str; \

#define UUMEMBER_INIT(_str) \
	__uuInitHeap(&(this->_str), 256); \

#define UUMEMBER_FREE(_str) \
	__uuFree(&(this->_str)); \

#define UUFREE(_str) \
	__uuFree(&_str);

/*[X]*/ int __uuInitHeap(UUStr *str, ssize_t byteCapacity);

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
/*[ ]*/ int uuAppend(UUStr *str, const wchar_t *second);
/*[ ]*/ int uuAppend(UUStr *str, const UCS32 *second);
/*[ ]*/ int uuAppend(UUStr *str, long long value, int radix);


/*[ ]*/ int uuCopy(UUStr *str, UUStr *source);
/*[ ]*/ int uuCopy(UUStr *str, const char *source);
/*[ ]*/ int uuCopy(UUStr *str, const wchar_t *source);
/*[ ]*/ int uuCopy(UUStr *str, const UCS32 *source);

/*[X]*/ void uuClear(UUStr *str);
/*[X]*/ int uuAdjustCapacity(UUStr *str, ssize_t byteCapacity);
/*[X]*/ UCS32 uuCharAt(UUStr *str, ssize_t byteOffset);
/*[X]*/ UCS32 uuReadNextChar(UUStr *str, ssize_t *byteOffset);
/*[X]*/ ssize_t uuCharLength(UUStr *str);
/*[X]*/ void __uuFree(UUStr *str);
/*[X]*/ int uuIsEmpty(UUStr *str);
/*[X]*/ ssize_t uuLastOffsetOf(UUStr *str, UCS32 chr);
/*[X]*/ ssize_t uuFirstOffsetOf(UUStr *str, UCS32 chr);
/*[X]*/ int uuStartsWith(UUStr *str, UUStr *with);
/*[X]*/ int uuTransform (UUStr *str, PFN_UUCHARFUNC pfn);
/*[X]*/ #define uuCSTR(_str) ((const char *) (_str)->ptr)
/*[X]*/ #define uuByteLength(_str) ((ssize_t)((_str)->byteLength))


#endif