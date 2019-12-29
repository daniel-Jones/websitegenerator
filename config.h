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

/* frontpage */
static const char 	*frontpage_title = "Index";
static const char 	*frontpage_info = "<h1>Index</h1>";
static const char 	*frontpage_index = "content/index.txt";
static const char 	*frontpage_index_output = "index.html";

/* opinions */
static const char 	*opinions_title = "Opinions";
static const char 	*opinions_info = "<h1>Opinions</h1>Thoughts and opinions on various subjects and media that I have consumed.<br><br>";
static const char 	*opinions_content = "content/opinions/index.txt";
static const char 	*opinions_content_output = "opinions/index.html";

/* opinions-anime */
static const char 	*opinions_anime_title = "Anime";
static const char 	*opinions_anime_info = "<h1>Anime</h1>Thoughts on some of my favourite anime series. Spoilers ahead.<br><br>";
static const char 	*opinions_anime_content = "content/opinions/anime.txt";
static const char 	*opinions_anime_content_output = "opinions/anime.html";

/* opinions_everything */
static const char 	*opinions_everything_title = "Everything else";
static const char 	*opinions_everything_info = "<h1>Everything else</h1>Thoughts on everything else that I need to get off my chest.<br><br>";
static const char 	*opinions_everything_content = "content/opinions/everything.txt";
static const char 	*opinions_everything_content_output = "opinions/everything.html";

/* likes */
static const char 	*likes_title = "Things I like";
static const char 	*likes_info = "<h1>Things that I like</h1>These are my opinions.<br>I don't much care how you feel about them.<br><br>";
static const char 	*likes_content = "content/like.txt";
static const char 	*likes_content_output = "like.html";

/* dislikes */
static const char 	*dislikes_title = "Things I dislike";
static const char 	*dislikes_info = "<h1>Things that I dislike</h1>These are my opinions.<br>I don't much care how you feel about them.<br><br>";
static const char 	*dislikes_content = "content/dislike.txt";
static const char 	*dislikes_content_output = "dislike.html";

/* interesting */
static const char 	*interesting_title = "Interesting";
static const char 	*interesting_info = "<h1>Interesting</h1> Things that are interesting or entertaining to me.<br><br>";
static const char 	*interesting_content = "content/interesting.txt";
static const char 	*interesting_content_output = "interesting.html";

/* portfolio */
static const char 	*portfolio_title = "Daniel's projects";
static const char 	*portfolio_info = "<h1>Projects</h1> Detailing a number of my personal projects <br><br>";
static const char 	*portfolio_content = "content/portfolio.txt";
static const char 	*portfolio_content_output = "portfolio.html";

/* each page to be generated go into this array */
static const struct page pages[] = {
/*	function	flags 	*/
	{frontpage,			NONE},
	{likespage,			NONE},
	{dislikespage,			NONE},
	{interestingpage,		NONE},
	{opinionspage,			NONE},
	{opinions_animepage,		NONE},
	{opinions_everythingpage,	NONE},
	{portfoliopage,			NONE},
};

#endif
