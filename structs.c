#pragma once
#include <stdbool.h>
#define M_GET false
#define M_POST true
struct requestData {
	bool reqType;
	char route[1024];
	char userAgent[1024];
	char cookies[1024];
};
