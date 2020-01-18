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
#define _XOPEN_SOURCE

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>

struct fileorstring
{
	const char *file;
	const char *str;
};

/* helpers */
int createfile(const char *file, const char *template);
long findstring(const char *file, const char *str);
int deletebytes(const char *file, long offset, size_t bytes);
int writeatbyte(const char *dest, struct fileorstring *source, long offset);
int replaceinpage(const char *outfile, const char *toreplace, struct fileorstring *source);
char *gettime(char *buffer, size_t size);
int createtmpfile(const char *name, const char *content, size_t size);
int makedirectories(const char *basedir, const char *file);
int postscompare(const void *a, const void *b);
int genericpage(int flags, const char *ind, const char *out, const char *tit, const char *inf);
int createdirectpages(const int *posts, size_t totalposts);
char *generatepagebar(char *bar, size_t size, const int *posts, size_t totalposts, int currentpage, int pagecount);
int generatepostpages(const int *posts, size_t totalposts, int pagecount);
int writeposts(const int *posts, size_t totalposts, const char *outfile, int currentpage, int pagecount);
int generaterss(const int *posts, size_t totalposts);
int writerss(FILE *out, int post);
char *striphtml(char *str, size_t size);
char *rfc822date(char *date, size_t size);
char *getimage(const char *line, char *buff, size_t size);

/* generators (to be put into the pages array) */
int frontpage(int flags);
int likespage(int flags);
int dislikespage(int flags);
int interestingpage(int flags);
int opinionspage(int flags);
int opinions_animepage(int flags);
int opinions_everythingpage(int flags);
int portfoliopage(int flags);
int postspage(int flags);

#endif
