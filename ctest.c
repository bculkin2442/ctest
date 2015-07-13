/*
 *  ctest.c
 *  CTest
 *
 *  Created by Benjamin on 5/31/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "ctest.h"

test_t* create_test(char* name, testfun_t test) {
	test_t* new = malloc(sizeof(test_t));
	if(new == NULL) {
		printf("Failed allocating new test named %s. Exiting now\n", name);
		exit(EXIT_FAILURE);
	}
	new->name = name;
	new->test = test;
	return new;
}

ctest_t* ctest_init(char* name,FILE * outp) {
	ctest_t* new = malloc(sizeof(ctest_t));
	if(new == NULL) {
		printf("Failed allocating harness for %s. Exiting now\n", name);
		exit(EXIT_FAILURE);
	}
	new->mandatory = malloc(sizeof(holder_t));\
	new->optional = malloc(sizeof(holder_t));
	if(new->mandatory == NULL || new->optional == NULL) {
		printf("Failed allocating test holder for harness %s. Exiting now\n", name);
		exit(EXIT_FAILURE);
	}
	new->mandatory->base = new->mandatory;
	new->optional->base = new->optional;
	new->name = name;
	new->outp = outp;
	return new;
}

void setlookup(ctest_t* harn,lookupfun_t err, lookupfun_t warn) {
	harn->error_convert = err;
	harn->warn_convert = warn;
}

void add_mandatory(ctest_t* harn,test_t* test) {
	if(harn->mandatory->test == NULL) {
		harn->mandatory->test = test;
	} else {
		if(harn->mandatory->next == NULL) {
			harn->mandatory->next = malloc(sizeof(holder_t));
			if(harn->mandatory->next == NULL) {
				printf("Error allocating new holder for test %s in harness %s. Exiting now\n", test->name, harn->name);
				exit(EXIT_FAILURE);
			}
			// Switch the current test node to the most current, but remember the base
			harn->mandatory->next->base = harn->mandatory->base;
			harn->mandatory = harn->mandatory->next;
			harn->mandatory->test = test;
		} else {
			while(harn->mandatory->next != NULL) {
				harn->mandatory = harn->mandatory->next;
				if(harn->mandatory->test == NULL) {
					harn->mandatory->test = test;
					break;
				}
			}
			harn->mandatory->next = malloc(sizeof(holder_t));
			if(harn->mandatory->next == NULL) {
				printf("Error allocating new holder for test %s in harness %s. Exiting now\n", test->name, harn->name);
				exit(EXIT_FAILURE);
			}
			// Switch the current test node to the most current, but remember the base
			harn->mandatory->next->base = harn->mandatory->base;
			harn->mandatory = harn->mandatory->next;
			harn->mandatory->test = test;
		}
	}
}

void add_optional(ctest_t* harn,test_t* test) {
	if(harn->optional->test == NULL) {
		harn->optional->test = test;
	} else {
		if(harn->optional->next == NULL) {
			harn->optional->next = malloc(sizeof(holder_t));
			if(harn->optional->next == NULL) {
				printf("Error allocating new holder for test %s in harness %s. Exiting now\n", test->name, harn->name);
				exit(EXIT_FAILURE);
			}
			// Switch the current test node to the most current, but remember the base
			harn->optional->next->base = harn->optional->base;
			harn->optional = harn->optional->next;
			harn->optional->test = test;
		} else {
			while(harn->optional->next != NULL) {
				harn->optional = harn->optional->next;
				if(harn->optional->test == NULL) {
					harn->optional->test = test;
					break;
				}
			}
			harn->optional->next = malloc(sizeof(holder_t));
			if(harn->optional->next == NULL) {
				printf("Error allocating new holder for test %s in harness %s. Exiting now\n", test->name, harn->name);
				exit(EXIT_FAILURE);
			}
			// Switch the current test node to the most current, but remember the base
			harn->optional->next->base = harn->optional->base;
			harn->optional = harn->optional->next;
			harn->optional->test = test;
		}
	}
}

int run_tests(ctest_t * harn, int fails) {
	if(fails < 1) {
		fails = 1;
	}
	int num_fails = 0;
	int num_tests = 1;
	printf("Started running mandatory tests for harness %s.\n", harn->name);
	holder_t* cur_hold = harn->mandatory->base;
	while(num_fails < fails) {
		test_t* cur = cur_hold->test;
		if(cur == NULL) {
			printf("Error: Mandatory test node # %d found with no associated test in harness %s. Exiting now.\n",
				   num_tests, harn->name);
			exit(EXIT_FAILURE);	
		}
		printf("Preparing to run test # %d (%s)\n", num_tests, cur->name);
		int res = cur->test();
		if(res < 0) {
			printf("Test # %d (%s) failed (code %d).\n", num_tests, cur->name, res);
			if(harn->error_convert != NULL)
				printf("Code explanation: %s\n", 
				harn->error_convert(res));
			num_fails++;
		} else {
			if(res != 0) {
				printf("Test # %d (%s) passed with a warning (code %d).\n", num_tests, cur->name, res);
				if(harn->warn_convert != NULL)
					printf("Code explanation: %s\n", 
					harn->warn_convert(res));
			} else {
				printf("Test # %d (%s) passed.\n", num_tests, cur->name);
			}
		}
		if(cur_hold->next == NULL) {
			printf("Finished running %d mandatory tests. %d out of %d (%f)%% passed.\n", 
			num_tests, num_tests - num_fails, 
			num_tests, 100 - ((double) num_fails / 
			num_tests * 100));
			break;
		} else {
			cur_hold = cur_hold->next;
			num_tests++;
		}
	}
	
	if(num_fails >= fails) {
		printf("Error: Too many (%d) tests failed to continue running tests.\n", num_fails);
		return num_fails;
	} else {
		printf("Started running optional tests for harness %s.\n", harn->name);
		cur_hold = harn->optional->base;
		int opt_num = 1;
		if(cur_hold->test == NULL) {
			printf("No optional tests.\n");
			return num_fails;
		}
		while(num_fails < fails) {
			test_t* cur = cur_hold->test;
			if(cur == NULL) {
				printf("Error: Optional test node # %d found with no associated test in harness %s. Exiting now.\n",
					   opt_num, harn->name);
				exit(EXIT_FAILURE);	
			}
			printf("Preparing to run test # %d, (%s)\n", opt_num, cur->name);
			int res = cur->test();
			if(res < 0) {
				printf("Test # %d (%s) failed (code %d).\n", opt_num, cur->name, res);
				if(harn->error_convert != NULL)
					printf("Code explanation: %s\n", 
					harn->error_convert(res));
			} else {
				if(res != 0) {
					printf("Test # %d (%s) passed with a warning (code %d).\n", opt_num, cur->name, res);
					if(harn->warn_convert != NULL)
						printf("Code explanation: %s\n", 
						harn->warn_convert(res));
				} else {
					printf("Test # %d (%s) passed.\n", opt_num, cur->name);
				}
			}
			if(cur_hold->next == NULL) {
				printf("Finished running %d optional tests. %d out of %d (%d)%% passed.\n", opt_num,
					   opt_num - num_fails, opt_num, ((opt_num - num_fails) / opt_num));
				break;
			} else {
				cur_hold = cur_hold->next;
				opt_num++;
			}
		}
	}
	return num_fails;
}
