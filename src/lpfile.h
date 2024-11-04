#include <stdio.h>
#ifndef LPFILE_H
#define LPFILE_H

struct lp;

int load_input_lpfile(char const* path, struct lp **body);

int test_input_lpfile(char const* path);

#endif