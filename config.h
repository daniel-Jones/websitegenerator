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

#ifndef CONFIG_H
#define CONFIG_H

#include "pages.h"

enum flag
{
	NONE	= 0,
};

struct page
{
	int (*function)();
	int flags;
};

/* customisation */
static const char 	*output_dir = "output/"; /* the directory all generated files will save to, ensure '/' appended */
static const char 	*title_string = "{TITLE}";
static const char 	*content_string = "{CONTENT}";
static const char 	*info_string = "{INFO}";
static const char 	*time_string = "{TIME}";
static const char 	*template_file = "template.txt";
static const char 	*frontpage_title = "Index";
static const char 	*frontpage_info = "<h1>Index</h1>";
static const char 	*frontpage_index = "content/index.txt";
static const char 	*frontpage_index_output = "index.html";

/* each page to be generated go into this array */
static const struct page pages[] = {
/*	function	flags 	*/
	{frontpage,	NONE},
};
#endif
