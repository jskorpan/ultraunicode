#include "./public_include/uunicode.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

#define UU_HAS_BYTELENGTH (1 << 0)
#define UU_MUST_FREE (1 << 1)
#define UU_HAS_CHARLENGTH (1 << 2)

static UTF8 *uuAlloc(ssize_t byteLength)
{
	return (UTF8 *) malloc(byteLength);
}

static void uuDelete(UTF8 *ptr)
{
	free(ptr);
}

static UTF8 *uuMemMem(UTF8 *haystack, UTF8 *needle)
{
	return (UTF8 *) strstr( (const char *) haystack, (const char *) needle);
}

static UTF8 *uuRMemMem(UTF8 *haystack, UTF8 *needle)
{
	return NULL;
}

int uuCreateFromCSTR(UUStr *str, const char *cstr, ssize_t byteLength)
{
	const char *cstrorg = cstr;

	if (byteLength == -1)
	{
		byteLength = 0;
		while ((*cstr++) != '\0') byteLength ++;
	}

	str->byteLength = byteLength;
	str->flags = UU_HAS_BYTELENGTH;
	str->ptr = (UTF8 *) cstrorg;

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

	if (charLength != -1)
	{
		str->charLength = charLength;
		str->flags |= UU_HAS_CHARLENGTH;
	}

	str->ptr = uuAlloc(byteLength + 1);
	memcpy (str->ptr, cstrorg, byteLength);
	*(str->ptr + byteLength) = '\0';
	
	return 0;
}

int uuCreateFromUTF16(UUStr *str, const UTF16 *in, ssize_t charLength, ssize_t byteLength)
{
	return -1;
}

int uuCreateFromUCS32(UUStr *str, const UCS32 *in, ssize_t charLength, ssize_t byteLength)
{
	return -1;
}

int uuConvertToUTF16(UUStr *str, const UTF16 *output, ssize_t byteLength, ssize_t *outCharLength)
{
	return -1;
}

int uuConvertToUCS32(UUStr *str, const UCS32 *output, ssize_t byteLength, ssize_t *outCharLength)
{
	return -1;
}

void uuClone(UUStr *str, UUStr *input)
{
	str->byteLength = input->byteLength;
	str->charLength = input->charLength;
	str->flags = str->flags;
	str->flags |= UU_MUST_FREE;

	str->ptr = uuAlloc(input->byteLength + 1);
	memcpy(str->ptr, input->ptr, input->byteLength + 1);
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

	while (uuReadNextChar(input, &bo, &chr) && bo - byteOffset < byteLength)
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

	while (uuReadNextChar(str, &byteOffset, &output) && byteOffset < offset)
	{
		charOffset ++;
	}

	return charOffset;
}

int uuReplace(UUStr *str, UUStr *input, UUStr *what, UUStr *with)
{
	UUStr left;
	UUStr right;
	UUStr part;

	ssize_t bo;
	bo = uuFind(input, what);

	if (bo == -1)
	{
		return -1;
	}

	uuSubString(&left, input, 0, bo);
	uuSubString(&right, input, bo + uuByteLength(what), uuByteLength(input) - uuByteLength(with));

	uuAppend(&part, &left, with);
	uuAppend(str, &part, &right);

	uuFree(&part);
	uuFree(&left);
	uuFree(&right);
	
	return 0;
}

void uuAppend(UUStr *str, UUStr *first, UUStr *second)
{
	str->byteLength = first->byteLength + second->byteLength;
	str->charLength = first->charLength + second->charLength;
	str->flags = UU_HAS_BYTELENGTH | UU_MUST_FREE;

	str->ptr = uuAlloc(str->byteLength + 1);

	memcpy (str->ptr, first->ptr, first->byteLength);
	memcpy (str->ptr + first->byteLength, second->ptr, second->byteLength + 1);
}

UCS32 uuCharAt(UUStr *str, ssize_t byteOffset)
{
	UCS32 output;

	if (uuReadNextChar(str, &byteOffset, &output) == -1)
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

int uuReadNextChar(UUStr *str, ssize_t *byteOffset, UCS32 *output)
{
	UTF8 *ptr = (str->ptr + *byteOffset);
	UTF8 len = g_utf8LengthLookup[*ptr];

	switch (len)
	{
	case 1:
		*output = *ptr;
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
		*output = ((in & 0x1f) << 6) | ((in >> 8) & 0x3f);
		#else
		*output = ((in & 0x1f00) >> 2) | (in & 0x3f);
		#endif

		if (*output < 0x80)
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
		*output = ((in & 0x0f) << 12) | ((in & 0x3f00) >> 2) | ((in & 0x3f0000) >> 16);
		#else
		in = *((UTF16 *) ptr) << 8;
		in |= *((UTF8 *) ptr + 2);
		*output = ((in & 0x0f0000) >> 4) | ((in & 0x3f00) >> 2) | (in & 0x3f);
		#endif

		if (*output < 0x800)
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
		*output = ((in & 0x07) << 18) | ((in & 0x3f00) << 4) | ((in & 0x3f0000) >> 10) | ((in & 0x3f000000) >> 24);
		#else
		in = *((UCS32 *) ptr);
		*output = ((in & 0x07000000) >> 6) | ((in & 0x3f0000) >> 4) | ((in & 0x3f00) >> 2) | (in & 0x3f);
		#endif
		if (*output < 0x10000)
		{
			return -1;
		}

		*byteOffset += 4;
		break;
	}

	case 5:
	case 6:
	case 0:
	default:
		return -1;


	}

	if (*byteOffset == str->byteLength)
	{
		return 0;
	}

	return 1;
}

ssize_t uuCharLength(UUStr *str)
{
	if (!(str->flags & UU_HAS_CHARLENGTH))
	{
		ssize_t bo = 0;
		ssize_t co = 0;
		UCS32 chr;

		while (uuReadNextChar(str, &bo, &chr))
		{
			co ++;
		}

		str->charLength = co;
		str->flags |= UU_HAS_CHARLENGTH;
	}

	return str->charLength;
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
		uuDelete(str->ptr);
	}
}

const char *uuToCSTR(UUStr *str)
{
	return NULL;
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
