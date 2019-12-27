/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PAGES_H
#define PAGES_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

struct fileorstring
{
	const char *file;
	const char *str;
};

/* helpers */
int createfile(const char *file);
long findstring(const char *file, const char *str);
int deletebytes(const char *file, long offset, size_t bytes);
int writeatbyte(const char *dest, struct fileorstring *source, long offset);
int replaceinpage(const char *outfile, const char *toreplace, struct fileorstring *source);
char *gettime(char *buffer, size_t size);
int createtmpfile(const char *name, const char *content, size_t size);

/* generators (to be put into the pages array) */
int frontpage(int flags);

#endif
