#include "./public_include/uunicode.h"


UUStr returnString(UUStr *s1, UUStr *s2)
{
	UUHEAP(ret, 1024);
	uuAppend(&ret, s1);
	uuAppend(&ret, s2);

	return ret;
}

