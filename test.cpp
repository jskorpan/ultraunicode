#include "./public_include/uunicode.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

#include <string>

void testAssert(const char *expr, int line, const char *file)
{
	fprintf (stderr, "%s:%d: %s\n", file, line, expr);
	getchar();
	abort();
}

#define ASSERT(_x) if (!(_x)) testAssert(#_x, __LINE__, __FILE__);

void test_Compare()
{
	UUSTACKCSTR(str1, "A nice ÅÄÖtest string");
	UUSTACKCSTR(str2, "A nice ÅÄÖtest string");
	ASSERT (uuCompare(&str1, &str2) == 0);

	uuClear(&str1);
	uuClear(&str2);

	uuAppend(&str1, "A nice test string");
	uuAppend(&str2, "A nice 5est string");

	ASSERT (uuCompare(&str1, &str2) != 0);

	uuClear(&str1);
	uuClear(&str2);

	uuAppend(&str1, "A nice testring");
	uuAppend(&str2, "A nice 5est string");

	ASSERT (uuCompare(&str1, &str2) != 0);
	UUFREE(str1);
	UUFREE(str2);
}

void test_FindFirstOf()
{
	UUSTACKCSTR(haystack, "A test nice test string");
	UUSTACKCSTR(needleFound, "test");
	UUSTACKCSTR(needleNotFound, "cookie");
	UUSTACKCSTR(needleFound2, "string");

	ASSERT (uuFindFirstOf(&haystack, &needleFound) == 2);
	ASSERT (uuFindFirstOf(&haystack, &needleFound2) == 17);
	ASSERT (uuFindFirstOf(&haystack, &needleNotFound) == -1);
}

void test_FindLastOf()
{
	UUSTACKCSTR(haystack, "A test nice test string");
	UUSTACKCSTR(needleFound, "test");
	UUSTACKCSTR(needleFound2, "string");
	UUSTACKCSTR(needleNotFound, "cookie");

	ASSERT (uuFindLastOf(&haystack, &needleFound) == 12);
	ASSERT (uuFindLastOf(&haystack, &needleFound2) == 17);
	ASSERT (uuFindLastOf(&haystack, &needleNotFound) == -1);
}


void test_SubString()
{

	UUSTACKCSTR(haystack, "A nice test string");
	UUSTACKCSTR(needle, "test");
	UUSTACKCSTR(cmpstr, "test str");
	UUSTACK(substr, 512);

	ssize_t offset = uuFindFirstOf(&haystack, &needle);

	ASSERT (uuSubString(&substr, &haystack, offset, 8) == 0);
	ASSERT (uuCompare(&substr, &cmpstr) == 0);
	ASSERT (uuSubString(&substr, &haystack, offset, 25) == 0);

	UUFREE(haystack);
	UUFREE(needle);
	UUFREE(cmpstr);
}

void test_FirstOffsetOf()
{
	UUSTACKCSTR(haystack, "A nice tZst ZstringZ");
	ASSERT (uuFirstOffsetOf(&haystack, 'Z') == 8);
	UUFREE (haystack);
}

void test_LastOffsetOf()
{
	UUSTACKCSTR(haystack, "ZA nice tZst string");
	ASSERT (uuLastOffsetOf(&haystack, 'Z') == 9);
	UUFREE (haystack);
}


void test_Replace()
{
	UUSTACK(input, 1024);
	UUSTACKCSTR(expected, "A nice tasy pizza string");
	UUSTACKCSTR(what, "test");
	UUSTACKCSTR(with, "tasy pizza");

	ASSERT(uuAppend(&input, "A nice test string") == 0);

	ASSERT (uuReplace(&input, &what, &with) == 0);
	ASSERT (uuCompare(&input, &expected) == 0);

	UUFREE(input);
	UUFREE(what);
	UUFREE(with);
	UUFREE(expected);
}

void test_CharAt()
{
	ssize_t offset1;
	ssize_t offset2;
	ssize_t offset3;

	UUSTACKCSTR(str1, "A nice test string");

	offset1 = uuFirstOffsetOf(&str1, 'n');
	ASSERT (uuCharAt(&str1, offset1) == 'n');

	offset2 = uuFirstOffsetOf(&str1, 't');
	ASSERT (uuCharAt(&str1, offset2) == 't');

	offset3 = uuFirstOffsetOf(&str1, 'g');
	ASSERT (uuCharAt(&str1, offset3) == 'g');

	UUFREE(str1);
}

void test_IsEmpty()
{
	UUSTACK(strEmpty, 16);
	UUSTACKCSTR(strNotEmpty, "Not empty");

	ASSERT (uuIsEmpty(&strEmpty));
	ASSERT (!uuIsEmpty(&strNotEmpty));

	UUFREE(strEmpty);
	UUFREE(strNotEmpty);
}

void test_AppendFromUTF16Bytes()
{
	ssize_t bo = 0;
	UUSTACK(str, 1024);
	ASSERT (uuAppend(&str, L"Räksmörgås") == 0);
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

	UUFREE(str);
}


void test_ConvertToUTF16()
{
	ssize_t chOutput;
	UTF16 output[128];

	//FIXME: Test for overflow of output buffer here
	UUSTACKWSTR(str, L"Räksmörgås");

	ASSERT (uuConvertToUTF16(&str, output, sizeof(output), &chOutput) == 0);
	UUFREE(str);
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
	ssize_t chOutput;
	UCS32 output[128];

	//FIXME: Test for overflow of output buffer here
	UUSTACKWSTR(str, L"Räksmörgås");

	ASSERT (uuConvertToUCS32(&str, output, sizeof(output), &chOutput) == 0);
	UUFREE(str);
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
	UUHEAP(str, 512);

	ASSERT(uuByteLength(&str) == 0);
	ASSERT(uuCharLength(&str) == 0);

	uuAppend(&str, "John");

	ASSERT(uuByteLength(&str) == 4);
	ASSERT(uuCharLength(&str) == 4);

	UUFREE(str);
}

void test_Append()
{
	UUSTACKCSTR(str1, "John");
	UUSTACKCSTR(str2, " ");
	UUSTACKCSTR(str3, "Doe");
	UUSTACK(total, 1024);
	UUSTACKCSTR(expected, "John Doe");

	ASSERT(uuAppend(&total, &str1) == 0);
	ASSERT(uuAppend(&total, &str2) == 0);
	ASSERT(uuAppend(&total, &str3) == 0);
	ASSERT(uuCompare(&total, &expected) == 0);

	uuClear(&total);

	ASSERT(uuAppend(&total, "John") == 0);
	ASSERT(uuAppend(&total, " ") == 0);
	ASSERT(uuAppend(&total, "Doe") == 0);
	ASSERT(uuCompare(&total, &expected) == 0);

	UUFREE(str1);
	UUFREE(str2);
	UUFREE(str3);
	UUFREE(total);

	UUSTACK(refuseTotal, 1);
	ASSERT(uuAppend(&refuseTotal, "Z") == 0);
	ASSERT(uuAppend(&refuseTotal, "John") == -1);
	UUFREE(refuseTotal);
}

void test_AdjustCapacity()
{
	UUHEAP(heap, 32);
	UUSTACK(stack, 32);

	ASSERT(uuAdjustCapacity(&stack, 31337) == -1);
	ASSERT(uuAdjustCapacity(&heap, 31337) == 0);

	UUFREE(stack);
	UUFREE(heap);
}

void test_CSTR()
{
	UUSTACKCSTR(str, "John Doe");
	ASSERT(strcmp (uuCSTR(&str), "John Doe") == 0);
	UUFREE(str);
}

void test_ByteLength()
{
	UUSTACKCSTR(str, "John Doe");
	ASSERT(uuByteLength(&str) == 8);
	UUFREE(str);
}

void test_AppendFromWSTR()
{
	ssize_t bo = 0;
	UUHEAP(str, 512);
	uuAppend(&str, L"Räksmörgås");

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
	UUFREE(str);
}


void test_StartsWith()
{
	UUSTACKCSTR(str, "John Doe");
	UUSTACKCSTR(john, "John");
	UUSTACKCSTR(filip, "Filip");

	ASSERT(!uuStartsWith(&john, &str));
	ASSERT(!uuStartsWith(&str, &filip));
	ASSERT(uuStartsWith(&str, &john));
}

class TestClass
{
	UUMEMBER_SDECL(m_username, 1024);
	UUMEMBER_DECL(m_password);
	
public:
	TestClass()
	{
		UUMEMBER_SINIT(m_username);
		UUMEMBER_INIT(m_password);

		uuCopy(&m_username, "hejsan");
		uuCopy(&m_password, "hello");
	}

	~TestClass()
	{
		UUMEMBER_FREE(m_username);
		UUMEMBER_FREE(m_password);
	}
};

void test_Conversion()
{
	UCS32 output[100];
	ssize_t cchOutput;

	UUSTACKWSTR(uuFromWide, L"123nåäöमानक हिन्दीربي/عربى汉语/漢語");

	uuConvertToUCS32(&uuFromWide, output, sizeof(output), &cchOutput);
	
	ssize_t bo = 0;

	for (ssize_t index = 0; index < cchOutput; index ++)
	{
		ASSERT(output[index] == uuReadNextChar(&uuFromWide, &bo));
	}

}


void test_MacroTest()
{
	UUSTACKCSTR(stack1, "Testing from the hood");
	UUSTACK(stack2, 1024);
	UUHEAP(heap1, 31337);
	UUSTACKWSTR(stackwide, L"åäö");

	UTF16 output[10];
	ssize_t outchr;

	uuConvertToUTF16(&stackwide, output, sizeof(output), &outchr);

	TestClass *ptr = new TestClass();

	delete ptr;


	UUSTACK(buffer, 1024);
	uuAppend(&buffer, "There are ");
	uuAppend(&buffer, 31337, 10);
	uuAppend(&buffer, " terrible kids in your neighbourhood");


}


static UCS32 toUpper(UCS32 chr)
{
	return (UCS32) towupper( (wchar_t) chr);
}

void test_Transform()
{
	UUSTACK(buffer, 1024);
	UUSTACKWSTR(str, L"abcdef");
	UUSTACKWSTR(expected, L"ABCDEF");

	uuAppend(&buffer, &str);

	ASSERT(uuTransform(&buffer, toUpper) == 0);
	ASSERT(uuCompare(&buffer, &expected) == 0);

	UUFREE(expected);
	UUFREE(str);
}

void test_AppendLong()
{
	UUSTACK(pos, 1024);
	UUSTACKCSTR(posRef, "31337");
	ASSERT (uuAppend(&pos, 31337, 10) == 0);
	ASSERT (uuCompare(&pos, &posRef) == 0);

	UUSTACK(neg, 1024);
	UUSTACKCSTR(negRef, "pre-31337post");
	ASSERT (uuAppend(&neg, "pre") == 0);
	ASSERT (uuAppend(&neg, -31337, 10) == 0);
	ASSERT (uuAppend(&neg, "post") == 0);

	ASSERT (uuCompare(&neg, &negRef) == 0);

	UUSTACK(hex, 1024);
	UUSTACKCSTR(hexRef, "3fa8c3a81");
	ASSERT (uuAppend(&hex, 0x3fa8c3a81, 16) == 0);
	ASSERT (uuCompare(&hex, &hexRef) == 0);
}

extern void benchmark();

int main (int argc, char **argv)
{
	test_Compare();
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
	//test_Transform();
	test_AppendLong();
	test_MacroTest();

	//FIXME: Test surrogates pair here!
	test_AppendFromWSTR();
	test_AppendFromUTF16Bytes();
	test_ConvertToUTF16();
	test_ConvertToUTF32();
	test_Conversion();

	fprintf (stderr, "All tests succeeded!\n");

	benchmark();

	return 0;
}