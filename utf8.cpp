#include "./public_include/uunicode.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __LITTLE_ENDIAN__
#else

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define __BIG_ENDIAN__
#endif

#endif

#if !defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#error "Endianess not supported"
#endif

ssize_t UCS32ToUTF4 (UCS32	ucs4, UTF8 dest[4]);

static UTF8 *libRealloc(UTF8 *ptr, ssize_t byteLength)
{
	return (UTF8 *) realloc(ptr, byteLength);
}

static UTF8 *libAlloc(ssize_t byteLength)
{
	return (UTF8 *) malloc(byteLength);
}

static void libFree(UTF8 *ptr)
{
	free(ptr);
}

static void libStrcpy(UTF8 *dst, UTF8 *src)
{
	while (*src != '\0')
	{
		(*dst++) = (*src++);
	}
}

static void libStrncpy(UTF8 *dst, UTF8 *src, ssize_t len)
{
	ssize_t i = 0;
	while (*src != '\0' && i < len)
	{
		(*dst++) = (*src++);
		i ++;
	}
}


static void libMemmove(UTF8 *dst, UTF8 *src, ssize_t len)
{
	memmove (dst, src, len);
}


static UTF8 *uuMemMem(UTF8 *haystack, UTF8 *needle)
{
	return (UTF8 *) strstr( (const char *) haystack, (const char *) needle);
}

static UTF8 *uuRMemMem(UTF8 *haystack, UTF8 *needle)
{
	return NULL;
}

static ssize_t libStrlen(UTF8 *str)
{
	return strlen( (const char *) str);
}

int uuCreateFromStack(UUStr *str, void *pstack, ssize_t byteLength)
{
	if (byteLength == -1)
	{
		return -1;
	}

	str->capacity = byteLength;
	str->byteLength = byteLength;
	str->flags = UU_HAS_BYTELENGTH;
	str->ptr = (UTF8 *) pstack;

	return 0;
}


int uuCreateFromCSTR(UUStr *str, const char *cstr, ssize_t byteLength)
{
	return uuCreateFromUTF8(str, (const UTF8 *) cstr, byteLength, -1);
}

int uuCreateFromUTF16(UUStr *str, const UTF16 *in, ssize_t charLength)
{
	ssize_t estByteLength;
	UTF8 *out;
	UTF8 *ptr;
	UCS32 ucs;
	int iSur = 0;
	UTF16 surrugate;

	if (charLength == -1)
	{
		UTF16 *lptr = (UTF16 *) in;
		charLength = 0;

		while (*lptr != '\0')
		{
			lptr ++;
			charLength ++;
		}
	}

	estByteLength = charLength * 4;

	str->flags = UU_HAS_BYTELENGTH | UU_MUST_FREE;
	out = libAlloc(estByteLength + 1);
	ptr = out;

	while (*in != '\0')
	{
		ucs = (*in++);

		if (iSur == 0)
		{
			if ((ucs & 0xfc00) == 0xd800)
			{
				surrugate = ucs;
				iSur ++;
				continue;
			}

			iSur = 0;
		}
		else
		{
			if ((ucs & 0xfc00) != 0xdc00)
			{
				libFree(out);
				return -1;
			}

			ucs = 0x10000 + (((ucs - 0xd800) << 10) | (surrugate - 0xdc00));
			iSur = 0;
		}

		ptr += UCS32ToUTF4(ucs, ptr);
	}

	(*ptr) = '\0';
	str->byteLength = (ssize_t) (ptr - out);
	str->capacity = estByteLength;
	str->ptr = out;

	return 0;
}


int uuCreateFromUCS32(UUStr *str, const UCS32 *in, ssize_t charLength)
{
	ssize_t estByteLength;
	UTF8 *out;
	UTF8 *ptr;
	UCS32 ucs;

	if (charLength == -1)
	{
		UTF16 *lptr = (UTF16 *) in;
		charLength = 0;

		while (*lptr != '\0')
		{
			lptr ++;
			charLength ++;
		}
	}

	estByteLength = charLength * 4;

	str->flags = UU_HAS_BYTELENGTH | UU_MUST_FREE;
	out = libAlloc(estByteLength + 1);
	ptr = out;

	while (*in != '\0')
	{
		ucs = (*in++);
		ptr += UCS32ToUTF4(ucs, ptr);
	}

	(*ptr) = '\0';
	str->byteLength = (ssize_t) (ptr - out);
	str->capacity = estByteLength;
	str->ptr = out;

	return 0;
}



int uuCreateFromUTF8(UUStr *str, const UTF8 *cstr, ssize_t byteLength, ssize_t charLength)
{
	const UTF8 *cstrorg = cstr;

	if (byteLength == -1)
	{
		byteLength = 0;
		while ((*cstr++) != '\0') byteLength ++;
	}

	str->byteLength = byteLength;
	str->flags = UU_HAS_BYTELENGTH | UU_MUST_FREE;

	str->ptr = libAlloc(byteLength + 1);
	memcpy (str->ptr, cstrorg, byteLength);
	*(str->ptr + byteLength) = '\0';
	str->capacity = byteLength;

	return 0;
}

int uuCreateEmpty(UUStr *str, ssize_t byteCapacity)
{
	str->byteLength = 0;
	str->capacity = byteCapacity;
	str->ptr = libAlloc(byteCapacity + 1);
	str->flags = UU_HAS_BYTELENGTH | UU_MUST_FREE;
	str->ptr[0] = '\0';

	return 0;
}


int uuConvertToUTF16(UUStr *str, const UTF16 *output, ssize_t byteLength, ssize_t *outCharLength)
{
	//FIXME: Check for overflow of output
	ssize_t bo = 0;
	UCS32 ucs;
	UTF16 *ptr = (UTF16 *) output;
	
	while ( (ucs = uuReadNextChar(str, &bo)))
	{
		if (ucs > 0x10000)
		{
			(*ptr++) = (ucs >> 10) + 0xd800;
			(*ptr++) = (ucs & 0x3ff) + 0xdc00;
		}
		else
		{
			(*ptr++) = ucs;
		}
	}

	(*ptr) = '\0';

	(*outCharLength) = (ssize_t) (ptr - output);
	return 0;
}

int uuConvertToUCS32(UUStr *str, const UCS32 *output, ssize_t byteLength, ssize_t *outCharLength)
{
	//FIXME: Check for overflow of output
	ssize_t bo = 0;
	UCS32 *ptr = (UCS32 *) output;
	
	while (*ptr = uuReadNextChar(str, &bo))
	{
		ptr ++;
	}

	(*outCharLength) = (ssize_t) (ptr - output);
	return 0;
}

int uuClone(UUStr *str, UUStr *source)
{
	if (str->capacity < source->capacity)
	{
		if (uuAdjustCapacity(str, source->capacity) == -1)
		{
			return -1;
		}
	}
	
	str->byteLength = source->byteLength;

	libStrcpy(str->ptr, source->ptr);
	str->ptr[str->byteLength] = '\0';
	return 0;
}

ssize_t uuFind(UUStr *str, UUStr *needle)
{
	UTF8 *ptr = uuMemMem (str->ptr, needle->ptr);

	if (ptr == NULL)
	{
		return -1;
	}

	return (ssize_t) (ptr - str->ptr);
}

ssize_t uuRFind(UUStr *str, UUStr *needle)
{
	UTF8 *ptr = uuRMemMem (str->ptr, needle->ptr);

	if (ptr == NULL)
	{
		return -1;
	}

	return (ssize_t) (ptr - str->ptr);
}

int uuCompare(UUStr *str, UUStr *str2)
{
	return strcmp ( (const char *) str->ptr, (const char *) str2->ptr);
}

int uuSubString(UUStr *str, UUStr *input, ssize_t byteOffset, ssize_t byteLength)
{
	ssize_t bo = byteOffset;
	ssize_t co = 0;
	UCS32 chr;

	while ((chr = uuReadNextChar(input, &bo)) && bo - byteOffset < byteLength)
	{
		co ++;
	}

	return uuCreateFromUTF8(str, input->ptr + byteOffset, bo - byteOffset, co);
}

ssize_t uuOffsetToIndex(UUStr *str, ssize_t offset)
{
	ssize_t byteOffset = 0;
	ssize_t charOffset = 0;
	UCS32 output;

	while ((output = uuReadNextChar(str, &byteOffset)) && byteOffset < offset)
	{
		charOffset ++;
	}

	return charOffset;
}

int uuAdjustCapacity(UUStr *str, ssize_t byteCapacity)
{
	if (str->capacity >= byteCapacity)
	{
		return 0;
	}

	if (!(str->flags & UU_MUST_FREE))
	{
		// We refuse to grow strings on the stack
		return -1;
	}

	str->ptr = libRealloc(str->ptr, byteCapacity + 1);
	str->capacity = byteCapacity;
	return 0;
}

int uuReplace(UUStr *str, UUStr *what, UUStr *with)
{
	ssize_t neededCapacity = str->byteLength - what->byteLength + with->byteLength;
	assert (neededCapacity >= 0);

	UTF8 *ptr = uuMemMem(str->ptr, what->ptr);
	UTF8 *basePtr = str->ptr;

	if (ptr == NULL)
	{
		return 0;
	}

	if (str->capacity < neededCapacity)
	{
		if (uuAdjustCapacity(str, neededCapacity) == -1)
		{
			return -1;
		}
		
		ptr = str->ptr + (ptr - basePtr);
	}

	ssize_t bytesToMove;

	bytesToMove = (ptr + what->byteLength) - str->ptr;
	bytesToMove = str->byteLength - bytesToMove;

	libMemmove (
		ptr + with->byteLength,
		ptr + what->byteLength, bytesToMove);

	libStrcpy (ptr, with->ptr);

	str->ptr[neededCapacity] = '\0';

	return 0;
}

int uuAppend(UUStr *str, UUStr *second)
{
	ssize_t neededCapacity = str->byteLength + second->byteLength;

	if (str->capacity < neededCapacity)
	{
		if (uuAdjustCapacity(str, neededCapacity) == -1)
		{
			return -1;
		}
	}

	libStrcpy (str->ptr + str->byteLength, second->ptr);

	str->byteLength = neededCapacity;
	str->ptr[str->byteLength] = '\0';

	return 0;
}

int uuAppend(UUStr *str, const char *second)
{
	UUStr temp;
	temp.byteLength = libStrlen( (UTF8 *) second);
	temp.ptr = (UTF8 *) second;
	
	return uuAppend(str, &temp);
}


UCS32 uuCharAt(UUStr *str, ssize_t byteOffset)
{
	UCS32 output;

	if ( (output = uuReadNextChar(str, &byteOffset)) == -1)
	{
		return -1;
	}

	return output;
}

static const UTF8 g_utf8LengthLookup[256] = 
{
/* 0x00 */ 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
/* 0x10 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 0x20 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
/* 0x30 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 0x40 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
/* 0x50 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 0x60 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
/* 0x70 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 0x80 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
/* 0x90 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 0xa0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
/* 0xb0 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
/* 0xc0 */ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
/* 0xd0 */ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
/* 0xe0 */ 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
/* 0xf0 */ 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 1
};

UCS32 uuReadNextChar(UUStr *str, ssize_t *byteOffset)
{
	UTF8 *ptr = (str->ptr + *byteOffset);
	UTF8 len = g_utf8LengthLookup[*ptr];
	UCS32 ucs;

	switch (len)
	{
	case 1:
		ucs = *ptr;
		(*byteOffset) ++;
		break;

	case 2:
	{
		UTF16 in;

		if (*byteOffset + 1 > str->byteLength)
		{
			return -1;
		}

		in = *((UTF16 *) ptr);

		#ifdef __LITTLE_ENDIAN__
		ucs = ((in & 0x1f) << 6) | ((in >> 8) & 0x3f);
		#else
		ucs = ((in & 0x1f00) >> 2) | (in & 0x3f);
		#endif

		if (ucs < 0x80)
		{
			//FIXME: Set error here!
			return -1;
		}

		*byteOffset += 2;
		break;
	}

	case 3:
	{
		UCS32 in;
		if (*byteOffset + 2 > str->byteLength)
		{
			return -1;
		}

		#ifdef __LITTLE_ENDIAN__
		in = *((UTF16 *) ptr);
		in |= *((UTF8 *) ptr + 2) << 16;
		ucs = ((in & 0x0f) << 12) | ((in & 0x3f00) >> 2) | ((in & 0x3f0000) >> 16);
		#else
		in = *((UTF16 *) ptr) << 8;
		in |= *((UTF8 *) ptr + 2);
		ucs = ((in & 0x0f0000) >> 4) | ((in & 0x3f00) >> 2) | (in & 0x3f);
		#endif

		if (ucs < 0x800)
		{
			return -1;
		}

		*byteOffset += 3;
		break;
	}

	case 4:
	{
		UCS32 in;

		if (*byteOffset + 3 > str->byteLength)
		{
			return -1;
		}

		#ifdef __LITTLE_ENDIAN__
		in = *((UCS32 *) ptr);
		ucs = ((in & 0x07) << 18) | ((in & 0x3f00) << 4) | ((in & 0x3f0000) >> 10) | ((in & 0x3f000000) >> 24);
		#else
		in = *((UCS32 *) ptr);
		ucs = ((in & 0x07000000) >> 6) | ((in & 0x3f0000) >> 4) | ((in & 0x3f00) >> 2) | (in & 0x3f);
		#endif
		if (ucs < 0x10000)
		{
			return -1;
		}

		*byteOffset += 4;
		break;
	}

	case 0:
		return 0;

	case 5:
	case 6:
	default:
		return -1;

	}

	return ucs;
}

ssize_t uuCharLength(UUStr *str)
{
	ssize_t bo = 0;
	ssize_t co = 0;
	UCS32 chr;

	while ( (chr = uuReadNextChar(str, &bo)))
	{
		co ++;
	}

	return co;
}

ssize_t uuByteLength(UUStr *str)
{
	assert (str->flags & UU_HAS_BYTELENGTH);
	return str->byteLength;
}

void uuFree(UUStr *str)
{
	if (str->flags & UU_MUST_FREE)
	{
		libFree(str->ptr);
	}
}

const char *uuToCSTR(UUStr *str)
{
	return (const char *) str->ptr;
}

int uuIsEmpty(UUStr *str)
{
	return (str->byteLength == 0) ? 1 : 0;
}

static ssize_t UCS32ToUTF4 (UCS32	ucs4, UTF8 dest[4])
{
	UTF8 *d = dest;

	if (ucs4 < 0x80) 
	{  
		*d++=  ucs4;
	}
	else 
	if (ucs4 < 0x800) 
	{ 
		*d++= ((ucs4 >> 6) & 0x1F) | 0xC0;
		*d++= ((ucs4 >> 0) & 0x3F) | 0x80;
	}
	else 
	if (ucs4 < 0x10000) 
	{  
		*d++= ((ucs4 >> 12) & 0x0F) | 0xE0;  
		*d++= ((ucs4 >> 6) & 0x3F) | 0x80;
		*d++= ((ucs4 >> 0) & 0x3F) | 0x80;

	}
	else 
	if (ucs4 < 0x200000) 
	{  
		*d++= ((ucs4 >> 18) & 0x07) | 0xF0; 
		*d++= ((ucs4 >> 12) & 0x3F) | 0x80;
		*d++= ((ucs4 >> 6) & 0x3F) | 0x80;
		*d++= ((ucs4 >> 0) & 0x3F) | 0x80;

	}
	else
	{
		return -1;
	}

	return d - dest;
}

ssize_t uuOffsetOf(UUStr *str, UCS32 chr)
{
	UTF8 utf[5];
	UTF8 *ptr;
	ssize_t utfLen;
	utfLen = UCS32ToUTF4(chr, utf);

	if (utfLen == -1)
	{
		return -1;
	}

	utf[utfLen] = '\0';

	ptr = uuMemMem(str->ptr, utf);

	if (ptr == NULL)
	{
		return -1;
	}

	return (ssize_t) (ptr - str->ptr);
}

ssize_t uuRIndexOf(UUStr *str, UCS32 chr)
{
	return -1;
}

int uuStartsWith(UUStr *str, UUStr *with)
{
	return -1;
}

void uuToLower(UUStr *str, UUStr *input)
{
}

void uuToUpper(UUStr *str, UUStr *input)
{
}

void uuClear(UUStr *str)
{
	str->byteLength = 0;
	str->ptr[0] = '\0';
}
