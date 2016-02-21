/*
 * test.cpp
 *
 *  Created on: 2012-11-16
 *      Author: kill
 */

#include <cstdio>
#include "fcore/FUtil.h"
#include "fcore/FString.h"
#include "fprotocol/FPHttp.h"

using namespace std;
using namespace freeyxm;

int main_test(void)
{
	string str1("  12 zc "), str2("ab  "), str3("cd "), str4("  "), str5("");

	printf("str1=[%s]\n", FString::trim(str1).c_str());
	printf("str2=[%s]\n", FString::trim(str2).c_str());
	printf("str3=[%s]\n", FString::trim(str3).c_str());
	printf("str4=[%s]\n", FString::trim(str4).c_str());
	printf("str5=[%s]\n", FString::trim(str5).c_str());

	return 0;
}
