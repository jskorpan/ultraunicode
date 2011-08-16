#include "./public_include/uunicode.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

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
	UUStr str2;

	ASSERT (uuCreateFromCSTR(&str1, "A nice test string", -1) == 0);
	uuClone(&str2, &str1);
	ASSERT (uuCompare(&str1, &str2) == 0);
	uuFree(&str1);
	uuFree(&str2);
}

void test_Find()
{
	UUStr haystack;
	UUStr needleFound;
	UUStr needleNotFound;

	ASSERT (uuCreateFromCSTR(&haystack, "A nice test string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&needleFound, "test", -1) == 0);
	ASSERT (uuCreateFromCSTR(&needleNotFound, "cookie", -1) == 0);

	ASSERT (uuFind(&haystack, &needleFound) == 7);
	ASSERT (uuFind(&haystack, &needleNotFound) == -1);
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

	ssize_t offset = uuFind(&haystack, &needle);

	ASSERT (uuSubString(&substr, &haystack, offset, 8) == 0);
	ASSERT (uuCompare(&substr, &cmpstr) == 0);
	ASSERT (uuSubString(&substr, &haystack, offset, 25) == 0);

	uuFree(&haystack);
	uuFree(&needle);
	uuFree(&cmpstr);
}

void test_OffsetOf()
{
	UUStr haystack;
	ASSERT (uuCreateFromCSTR(&haystack, "A nice tZst string", -1) == 0);
	ASSERT (uuOffsetOf(&haystack, 0x0000005a) == 8);
	uuFree (&haystack);
}

void test_Replace()
{
	UUStr input;
	UUStr what;
	UUStr with;
	UUStr output;
	UUStr expected;

	ASSERT (uuCreateFromCSTR(&input, "A nice test string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&expected, "A nice tasy pizza string", -1) == 0);
	ASSERT (uuCreateFromCSTR(&what, "test", -1) == 0);
	ASSERT (uuCreateFromCSTR(&with, "tasy pizza", -1) == 0);
	ASSERT (uuReplace(&output, &input, &what, &with) == 0);
	ASSERT (uuCompare(&output, &expected) == 0);

	uuFree(&input);
	uuFree(&what);
	uuFree(&with);
	uuFree(&output);
	uuFree(&expected);
}

void test_CharAt()
{
	UUStr str1;
	ssize_t offset1;
	ssize_t offset2;
	ssize_t offset3;
	ASSERT (uuCreateFromCSTR(&str1, "A nice test string", -1) == 0);

	offset1 = uuOffsetOf(&str1, 'n');
	ASSERT (uuCharAt(&str1, offset1) == 'n');

	offset2 = uuOffsetOf(&str1, 't');
	ASSERT (uuCharAt(&str1, offset2) == 't');

	offset3 = uuOffsetOf(&str1, 'g');
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
}




int main (int argc, char **argv)
{
	test_CreateFromASCIIBytes();
	test_CreateFromUTF8Bytes();
	test_Compare();
	test_Clone();
	test_Find();
	test_SubString();
	test_OffsetOf();
	test_Replace();
	test_CharAt();
	test_IsEmpty();

	//FIXME: Test surrogates pair here!
	test_CreateFromUTF16Bytes();
	test_ConvertToUTF16();
	test_ConvertToUTF32();

	return 0;
}