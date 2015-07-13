/*
 *  ctest.h
 *  CTest header file
 *
 *  Created by Benjamin on 5/31/14.
 *  
 *
 */
#ifndef CTEST_H 
#define CTEST_H
// Test function definition. A test function will succede on any positive result including zero,
//   but any non-zero positive number will be given as a warning.
typedef int (*testfun_t)(void);

// Lookup function definition.
//	Defines a function for resolving a integer
//	to an explanatory string
typedef char* (*lookupfun_t)(int);

// Contains basic identifying information about a test
//  as well as the test itself
typedef struct test_t {
	// Name of the given test.
	char* name;
	// Actual test function
	testfun_t test;
} test_t;

// Construct a test from its base parts
// Arguments
//   name - The name assigned to this test
//   test - The function that specifies this test
// Returns
//   A valid test structure
extern test_t* create_test(char* name, testfun_t test);

// Holds a bunch of tests in a list
typedef struct holder_t {
	// Currrent test being held
	test_t* test;
	// Next holder in chain
	struct holder_t* next;
	// Base of this holder chain, for easy traversal
	struct holder_t* base;
} holder_t;

// Main test harness for CTest
//  Contains info about the thing being tested
//  as well all of the tests for it
typedef struct ctest_t {
	// Name of the thing we are testing
	char* name;
	// Mandatory tests - tests that MUST pass
	holder_t* mandatory;
	// Optional tests - tests that SHOULD pass
	holder_t* optional;
	// Lookup to convert errors to strings
	lookupfun_t error_convert;
	// Lookup to convert warnings to strings
	lookupfun_t warn_convert;
	// Place to send test results
	FILE* outp;
} ctest_t;

// Create and initialize a test harness
// Parameters
//   name - Name of what is being tested
//   outp - Source to send test results to
// Return
//   A valid pointer to a harness.
extern ctest_t* ctest_init(char* name, FILE* outp);

// Specify the lookup functions for a test harness
// Parameters
//	 harn - harness to set lookup functions of
//	 err  - lookup function to use for errors
//	 warn - lookup function to use for warnings
extern void setlookup(ctest_t* harn, lookupfun_t err, lookupfun_t warn);

// Add a mandatory test that MUST succede
// Parameters
//   harn - harness to add the mandatory test to
//   test - mandatory test to add
extern void add_mandatory(ctest_t* harn, test_t* test);

// Add a optional test that SHOULD succede
// Parameters
//   harn - harness to add the optional test to
//   test - optional test to add
extern void add_optional(ctest_t* harn, test_t* test);

// Run all of the tests associated with a harness
// Parameters
//   harn - The harness to run tests on
//   fails - Number of mandatory test failures needed to stop running tests.
//     if negative or zero, defaults to 1
// Returns
//   The number of mandatory tests that failed
extern int run_tests(ctest_t* harn, int fails);
#endif
