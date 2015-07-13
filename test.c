/*
 *  test.c
 *  CTest
 *
 *  Created by Benjamin on 6/1/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include "ctest.h"

char* err_lookup(int n);

char* warn_lookup(int n);

int success();

int warn();

int fail();

int main() {
	ctest_t* harn = ctest_init("CTest",stdin);
	
	setlookup(harn, &err_lookup, &warn_lookup);
	
	add_mandatory(harn, create_test("Success Test", &success));
		
	add_mandatory(harn, create_test("Failing Test", &fail));
	
	add_mandatory(harn, create_test("Warning Test", &warn));
	
	run_tests(harn,2);
	return 0;
}

int success() {
	return 0;
}

int warn() {
	return 1;
}

int fail() {
	return -1;
}

char* err_lookup(int n) {
	return "error.";
}

char* warn_lookup(int n) {
	return "warn.";
}
