/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

#pragma once

#include "poseidon/poseidon.h"

/*
** Feel free to complete this list as needed.
*/

[[gnu::pure]] size_t strlen(char const *);
char *strcpy(char *, char const *);
char *strncpy(char *, char const *, size_t);
[[gnu::pure]] char *strstr(char const *haystack, char const *needle);
char *strcat(char *dest, char const *src);
[[gnu::pure]] int strcmp(char const *s1, char const *s2);
[[gnu::pure]] int strncmp(char const *s1, char const *s2, size_t n);

void *memset(void *src, int c, size_t n);
void *memcpy(void *dest, void const *src, size_t n);
[[gnu::pure]] int memcmp(void const *s1, void const *s2, size_t n);
[[gnu::pure]] void *memchr(void const *src, int c, size_t n);
void *memmove(void *dest, void const *src, size_t n);
