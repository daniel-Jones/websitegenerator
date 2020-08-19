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
	NONE		= 1<<0,
	RSS		= 1<<1,
	RSSIMAGES 	= 1<<2,
	PINNED		= 1<<3,
	GIRLNUMBERS	= 1<<4,
	READMORETAG	= 1<<5,
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
static const char 	*rss_string = "{ITEMS}";
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
static const char 	*likes_content_output = "opinions/like.html";

/* dislikes */
static const char 	*dislikes_title = "Things I dislike";
static const char 	*dislikes_info = "<h1>Things that I dislike</h1>These are my opinions.<br>I don't much care how you feel about them.<br><br>";
static const char 	*dislikes_content = "content/dislike.txt";
static const char 	*dislikes_content_output = "opinions/dislike.html";

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

/* posts */
static const char 	*posts_title = "Daniel's posts";
static const char 	*posts_info = "<h1>Posts</h1>My posts about programming and things.<br>Date format is day/month/year because I'm sane.<br><br>";
static const char 	*posts_content = "content/blog/";
static const char 	*posts_output_dir = "posts/";
static const char 	*direct_output_dir = "posts/direct/";
static const int 	posts_per_page = 10;
static const int 	ignore[] = {}; /* ignore these posts */
static const int	pinned[] = {116, 115, 107}; /* pinned posts */
static const char 	*read_more_tag = "[READMORE]";

/* rss */
static const char 	*rss_output = "posts/posts.rss";
static const char 	*rss_template = "rss.txt";
static const int 	post_count = 10;
static const char 	*base_url = "https://danieljon.es/posts/";
static const char 	*author_string = "daniel@danieljon.es (Daniel Jones)";

/* girlnumbers for blog post count */
static const char 	*girlnumber_url = "https://danieljon.es/";
static const char 	*girlnumber_dir = "/media/numbers/";
static const char 	*girlnumber_extension = ".gif";

/* each page to be generated go into this array */
static const struct page pages[] = {
/*	function			flags 	*/
	{frontpage,			NONE},
	{likespage,			NONE},
	{dislikespage,			NONE},
	{interestingpage,		NONE},
	{opinionspage,			NONE},
	{opinions_animepage,		NONE},
	{opinions_everythingpage,	NONE},
	{portfoliopage,			NONE},
	{postspage,			RSS|RSSIMAGES|PINNED|GIRLNUMBERS|READMORETAG},	/* flags: RSS feed, RSS images, pinned posts, girl numbers, read more tag */
};

#endif
