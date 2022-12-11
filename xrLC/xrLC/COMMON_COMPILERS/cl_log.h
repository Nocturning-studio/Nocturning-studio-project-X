#pragma	once

#include <windows.h>

void clMsg			(const char *format, ...);
void Status			(const char *format, ...);
void Progress		(const float F);
void Phase			(const char *phase_name);

void logThread	    (void *dummy);
void logCallback	(LPCSTR c);
