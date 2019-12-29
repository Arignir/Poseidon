/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2019 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\******************************************************************************/

#ifndef _LIB_STRING_H_
# define _LIB_STRING_H_

# include <poseidon/poseidon.h>

/*
** Feel free to complete this list as needed.
*/

size_t  strlen(char const *) __pure;
char    *strcpy(char *, char const *);
char    *strncpy(char *, char const *, size_t);
char    *strstr(char const *haystack, char const *needle) __pure;
char    *strcat(char *dest, char const *src);
int     strcmp(char const *s1, char const *s2) __pure;
int     strncmp(char const *s1, char const *s2, size_t n) __pure;

void    *memset(void *src, int c, size_t n);
void    *memcpy(void *dest, void const *src, size_t n);
int     memcmp(void const *s1, void const *s2, size_t n) __pure;
void    *memchr(void const *src, int c, size_t n) __pure;
void    *memmove(void *dest, void const *src, size_t n);

#endif /* !_LIB_STRING_H_ */
