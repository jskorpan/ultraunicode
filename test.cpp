#include "./public_include/uunicode.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

void testAssert(const char *expr, int line, const char *file)
{
	fprintf (stderr, "%s:%d: %s\n", file, line, expr);
	getchar();
	abort();
}

#define ASSERT(_x) if (!(_x)) testAssert(#_x, __LINE__, __FILE__);


void test_CreateFromASCIIBytes()
{
	char TEST_STRING[] = "A nice test string";

	UUStr str1;
	UUStr str2;
	ASSERT (uuCreateFromCSTR(&str1, TEST_STRING, -1) == 0);
	ASSERT (uuCreateFromCSTR(&str2, TEST_STRING, 6) == 0);
	ASSERT (uuByteLength(&str1) == 18);
	ASSERT (uuByteLength(&str2) == 6);
	uuFree(&str1);
	uuFree(&str2);

	UTF8 utfStr[18 + 1];
	memcpy (utfStr, TEST_STRING, 19);

	ASSERT (uuCreateFromUTF8(&str1, utfStr, -1, -1) == 0);
	ASSERT (uuCreateFromUTF8(&str2, utfStr, 6, -1) == 0);
	ASSERT (uuByteLength(&str1) == 18);
	ASSERT (uuByteLength(&str2) == 6);
	uuFree(&str1);
	uuFree(&str2);
}

void test_CreateFromUTF8Bytes()
{
	UUStr str1;
	UUStr str2;

	ASSERT (uuCreateFromCSTR(&str1, "A nice test string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&str2, "A nice test string", 6) == 0);
	ASSERT (uuByteLength(&str1) == 18);
	ASSERT (uuByteLength(&str2) == 6);
	uuFree(&str1);
	uuFree(&str2);

	UTF8 utfStr[18 + 1];
	memcpy (utfStr, "A nice test string", 19);

	ASSERT (uuCreateFromUTF8(&str1, utfStr, -1, -1) == 0);
	ASSERT (uuCreateFromUTF8(&str2, utfStr, 6, -1) == 0);
	ASSERT (uuByteLength(&str1) == 18);
	ASSERT (uuByteLength(&str2) == 6);
	uuFree(&str1);
	uuFree(&str2);
}

void test_Compare()
{
	UUStr str1;
	UUStr str2;

	ASSERT (uuCreateFromCSTR(&str1, "A nice test string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&str2, "A nice test string", -1) == 0);
	ASSERT (uuCompare(&str1, &str2) == 0);
	uuFree(&str1);
	uuFree(&str2);

	ASSERT (uuCreateFromCSTR(&str1, "A nice test string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&str2, "A nice 5est string", -1) == 0);
	ASSERT (uuCompare(&str1, &str2) != 0);
	uuFree(&str1);
	uuFree(&str2);


	ASSERT (uuCreateFromCSTR(&str1, "A nice testring", -1) == 0);
	ASSERT (uuCreateFromCSTR(&str2, "A nice 5est string", -1) == 0);
	ASSERT (uuCompare(&str1, &str2) != 0);
	uuFree(&str1);
	uuFree(&str2);
}

void test_Clone()
{
	UUStr str1;
	UUSTR_STACK(str2, 1024);

	ASSERT (uuCreateFromCSTR(&str1, "A nice test string", -1) == 0);
	uuClone(&str2, &str1);
	ASSERT (uuCompare(&str1, &str2) == 0);
	uuFree(&str1);
	uuFree(&str2);
}

void test_FindFirstOf()
{
	UUStr haystack;
	UUStr needleFound;
	UUStr needleFound2;
	UUStr needleNotFound;

	ASSERT (uuCreateFromCSTR(&haystack, "A test nice test string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&needleFound, "test", -1) == 0);
	ASSERT (uuCreateFromCSTR(&needleNotFound, "cookie", -1) == 0);
	ASSERT (uuCreateFromCSTR(&needleFound2, "string", -1) == 0);

	ASSERT (uuFindFirstOf(&haystack, &needleFound) == 2);
	ASSERT (uuFindFirstOf(&haystack, &needleFound2) == 17);
	ASSERT (uuFindFirstOf(&haystack, &needleNotFound) == -1);
}

void test_FindLastOf()
{
	UUStr haystack;
	UUStr needleFound;
	UUStr needleFound2;
	UUStr needleNotFound;

	ASSERT (uuCreateFromCSTR(&haystack, "A test nice test string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&needleFound, "test", -1) == 0);
	ASSERT (uuCreateFromCSTR(&needleFound2, "string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&needleNotFound, "cookie", -1) == 0);

	ASSERT (uuFindLastOf(&haystack, &needleFound) == 12);
	ASSERT (uuFindLastOf(&haystack, &needleFound2) == 17);
	ASSERT (uuFindLastOf(&haystack, &needleNotFound) == -1);
}


void test_SubString()
{
	UUStr haystack;
	UUStr substr;
	UUStr needle;
	UUStr cmpstr;

	ASSERT (uuCreateFromCSTR(&haystack, "A nice test string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&needle, "test", -1) == 0);
	ASSERT (uuCreateFromCSTR(&cmpstr, "test str", -1) == 0);

	ssize_t offset = uuFindFirstOf(&haystack, &needle);

	ASSERT (uuSubString(&substr, &haystack, offset, 8) == 0);
	ASSERT (uuCompare(&substr, &cmpstr) == 0);
	ASSERT (uuSubString(&substr, &haystack, offset, 25) == 0);

	uuFree(&haystack);
	uuFree(&needle);
	uuFree(&cmpstr);
}

void test_FirstOffsetOf()
{
	UUStr haystack;
	ASSERT (uuCreateFromCSTR(&haystack, "A nice tZst ZstringZ", -1) == 0);
	ASSERT (uuFirstOffsetOf(&haystack, 'Z') == 8);
	uuFree (&haystack);
}

void test_LastOffsetOf()
{
	UUStr haystack;
	ASSERT (uuCreateFromCSTR(&haystack, "ZA nice tZst string", -1) == 0);
	ASSERT (uuLastOffsetOf(&haystack, 'Z') == 9);
	uuFree (&haystack);
}


void test_Replace()
{
	UUStr input;
	UUStr what;
	UUStr with;
	UUStr expected;

	ASSERT (uuCreateFromCSTR(&input, "A nice test string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&expected, "A nice tasy pizza string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&what, "test", -1) == 0);
	ASSERT (uuCreateFromCSTR(&with, "tasy pizza", -1) == 0);
	ASSERT (uuReplace(&input, &what, &with) == 0);
	ASSERT (uuCompare(&input, &expected) == 0);

	uuFree(&input);
	uuFree(&what);
	uuFree(&with);
	uuFree(&expected);
}

void test_CharAt()
{
	UUStr str1;
	ssize_t offset1;
	ssize_t offset2;
	ssize_t offset3;
	ASSERT (uuCreateFromCSTR(&str1, "A nice test string", -1) == 0);

	offset1 = uuFirstOffsetOf(&str1, 'n');
	ASSERT (uuCharAt(&str1, offset1) == 'n');

	offset2 = uuFirstOffsetOf(&str1, 't');
	ASSERT (uuCharAt(&str1, offset2) == 't');

	offset3 = uuFirstOffsetOf(&str1, 'g');
	ASSERT (uuCharAt(&str1, offset3) == 'g');

	uuFree(&str1);
}

void test_IsEmpty()
{
	UUStr strEmpty;
	UUStr strNotEmpty;
	ASSERT (uuCreateFromCSTR(&strEmpty, "", -1) == 0);
	ASSERT (uuCreateFromCSTR(&strNotEmpty, "Not empty", -1) == 0);

	ASSERT (uuIsEmpty(&strEmpty));
	ASSERT (!uuIsEmpty(&strNotEmpty));

	uuFree(&strEmpty);
	uuFree(&strNotEmpty);
}

void test_CreateFromUTF16Bytes()
{
	UUStr str;
	ssize_t bo = 0;
	ASSERT (uuCreateFromWSTR(&str, L"Räksmörgås", 10) == 0);
	ASSERT (uuByteLength(&str) == 13);

	ASSERT (uuReadNextChar(&str, &bo) == L'R');
	ASSERT (uuReadNextChar(&str, &bo) == L'ä');
	ASSERT (uuReadNextChar(&str, &bo) == L'k');
	ASSERT (uuReadNextChar(&str, &bo) == L's');
	ASSERT (uuReadNextChar(&str, &bo) == L'm');
	ASSERT (uuReadNextChar(&str, &bo) == L'ö');
	ASSERT (uuReadNextChar(&str, &bo) == L'r');
	ASSERT (uuReadNextChar(&str, &bo) == L'g');
	ASSERT (uuReadNextChar(&str, &bo) == L'å');
	ASSERT (uuReadNextChar(&str, &bo) == L's');

	uuFree(&str);
}


void test_ConvertToUTF16()
{
	UUStr str;
	ssize_t chOutput;
	UTF16 output[128];

	//FIXME: Test for overflow of output buffer here
	ASSERT (uuCreateFromWSTR(&str, L"Räksmörgås", 10) == 0);
	ASSERT (uuConvertToUTF16(&str, output, sizeof(output), &chOutput) == 0);
	uuFree(&str);
	ASSERT (chOutput == 10);

	ASSERT(output[0] == L'R');
	ASSERT(output[1] == L'ä');
	ASSERT(output[2] == L'k');
	ASSERT(output[3] == L's');
	ASSERT(output[4] == L'm');
	ASSERT(output[5] == L'ö');
	ASSERT(output[6] == L'r');
	ASSERT(output[7] == L'g');
	ASSERT(output[8] == L'å');
	ASSERT(output[9] == L's');
	ASSERT(output[10] == L'\0');

}

void test_ConvertToUTF32()
{
	UUStr str;
	ssize_t chOutput;
	UCS32 output[128];

	//FIXME: Test for overflow of output buffer here
	ASSERT (uuCreateFromWSTR(&str, L"Räksmörgås", 10) == 0);
	ASSERT (uuConvertToUCS32(&str, output, sizeof(output), &chOutput) == 0);
	uuFree(&str);
	ASSERT (chOutput == 10);

	ASSERT(output[0] == L'R');
	ASSERT(output[1] == L'ä');
	ASSERT(output[2] == L'k');
	ASSERT(output[3] == L's');
	ASSERT(output[4] == L'm');
	ASSERT(output[5] == L'ö');
	ASSERT(output[6] == L'r');
	ASSERT(output[7] == L'g');
	ASSERT(output[8] == L'å');
	ASSERT(output[9] == L's');
	ASSERT(output[10] == L'\0');
}

void test_CreateEmpty()
{
	UUStr str;

	uuCreateEmpty(&str, 512);

	ASSERT(uuByteLength(&str) == 0);
	ASSERT(uuCharLength(&str) == 0);

	uuAppend(&str, "John");

	ASSERT(uuByteLength(&str) == 4);
	ASSERT(uuCharLength(&str) == 4);

	uuFree(&str);
}

void test_Append()
{
	UUSTR_STACK_CSTR(str1, "John");
	UUSTR_STACK_CSTR(str2, " ");
	UUSTR_STACK_CSTR(str3, "Doe");
	UUSTR_STACK(total, 1024);
	UUSTR_STACK_CSTR(expected, "John Doe");

	ASSERT(uuAppend(&total, &str1) == 0);
	ASSERT(uuAppend(&total, &str2) == 0);
	ASSERT(uuAppend(&total, &str3) == 0);
	ASSERT(uuCompare(&total, &expected) == 0);

	uuClear(&total);

	ASSERT(uuAppend(&total, "John") == 0);
	ASSERT(uuAppend(&total, " ") == 0);
	ASSERT(uuAppend(&total, "Doe") == 0);
	ASSERT(uuCompare(&total, &expected) == 0);

	uuFree(&str1);
	uuFree(&str2);
	uuFree(&str3);
	uuFree(&total);

	UUSTR_STACK(refuseTotal, 1);
	ASSERT(uuAppend(&refuseTotal, "Z") == 0);
	ASSERT(uuAppend(&refuseTotal, "John") == -1);
	uuFree(&refuseTotal);
}

void test_AdjustCapacity()
{
	UUSTR_HEAP(heap, 32);
	UUSTR_STACK(stack, 32);

	ASSERT(uuAdjustCapacity(&stack, 31337) == -1);
	ASSERT(uuAdjustCapacity(&heap, 31337) == 0);

	uuFree(&stack);
	uuFree(&heap);
}

void test_CSTR()
{
	UUSTR_STACK_CSTR(str, "John Doe");
	ASSERT(strcmp (uuCSTR(&str), "John Doe") == 0);
	uuFree(&str);
}

void test_ByteLength()
{
	UUSTR_STACK_CSTR(str, "John Doe");
	ASSERT(uuByteLength(&str) == 8);
	uuFree(&str);
}

void test_CreateFromWSTR()
{
	ssize_t bo = 0;
	UUStr str;
	uuCreateFromWSTR (&str, L"Räksmörgås", -1);

	ASSERT (uuReadNextChar(&str, &bo) == L'R');
	ASSERT (uuReadNextChar(&str, &bo) == L'ä');
	ASSERT (uuReadNextChar(&str, &bo) == L'k');
	ASSERT (uuReadNextChar(&str, &bo) == L's');
	ASSERT (uuReadNextChar(&str, &bo) == L'm');
	ASSERT (uuReadNextChar(&str, &bo) == L'ö');
	ASSERT (uuReadNextChar(&str, &bo) == L'r');
	ASSERT (uuReadNextChar(&str, &bo) == L'g');
	ASSERT (uuReadNextChar(&str, &bo) == L'å');
	ASSERT (uuReadNextChar(&str, &bo) == L's');
	uuFree(&str);
}


void test_StartsWith()
{
	UUSTR_STACK_CSTR(str, "John Doe");
	UUSTR_STACK_CSTR(john, "John");
	UUSTR_STACK_CSTR(filip, "Filip");

	ASSERT(!uuStartsWith(&john, &str));
	ASSERT(!uuStartsWith(&str, &filip));
	ASSERT(uuStartsWith(&str, &john));

}

static UCS32 toUpper(UCS32 chr)
{
	return (UCS32) towupper( (wchar_t) chr);
}

void test_Transform()
{
	UUStr expected;
	UUStr str;	
	
	uuCreateFromWSTR(&str, L"abcdef", -1);
	uuCreateFromWSTR(&expected, L"ABCDEF", -1);
	ASSERT(uuTransform(&str, toUpper) == 0);
	ASSERT(uuCompare(&str, &expected) == 0);

	uuFree(&expected);
	uuFree(&str);
}

int main (int argc, char **argv)
{
	test_CreateFromASCIIBytes();
	test_CreateFromUTF8Bytes();
	test_Compare();
	test_Clone();
	test_FindFirstOf();
	test_FindLastOf();
	test_SubString();
	test_FirstOffsetOf();
	test_LastOffsetOf();
	test_Replace();
	test_CharAt();
	test_IsEmpty();
	test_Append();
	test_CreateEmpty();
	test_AdjustCapacity();
	test_CSTR();
	test_ByteLength();
	test_StartsWith();
	test_Transform();

	//FIXME: Test surrogates pair here!
	test_CreateFromWSTR();
	test_CreateFromUTF16Bytes();
	test_ConvertToUTF16();
	test_ConvertToUTF32();

	fprintf (stderr, "All tests succeeded!\n");
	return 0;
}