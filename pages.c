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

#include "pages.h"
#include "config.h"

int
makedirectories(const char *basedir, const char *file)
{
	/*
	 * make directory 'basedir'
	 * tokenise 'file' and make all directories leading to the final file inside 'basedir'
	 */
	// TODO: implement
	char buffer[512];
	struct stat sb = {0};

	if(!(stat(output_dir, &sb) == 0) && !S_ISDIR(sb.st_mode))
	{
		fprintf(stderr, "directory '%s' does not exist, trying to make directory..\n", output_dir);
		if (mkdir(output_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
		{
			fprintf(stderr, "unable to mkdir '%s', unrecoverable failure\n", output_dir);
			return 0;
		}
		fprintf(stderr, "OK\n");
	}

	return 1;
}

int
createfile(const char *file, const char *template)
{
	/*
	 * create file at location 'output_dir'/file' using the template 'template'
	 * overwrite if it exists
	 * assume the caller has put us in the base directory already
	 */

	/*
	 * FIXME: if trying to create a file with subdirectories it will not work
	 * one day tokenise the file path and make them
	 */
	if (!makedirectories(output_dir, file))
	{

		fprintf(stderr, "unable to create directories for '%s/%s', unrecoverable failure\n", output_dir, file);
		return 0;
	}
	char filename[512] = {0};
	snprintf(filename, 512, "%s%s", output_dir, file);

	FILE *out = fopen(filename, "w");
	if (!out)
	{
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		fprintf(stderr, "unable to open file '%s', unrecoverable failure\n", filename);
		return 0;
	}

	FILE *in = fopen(template, "r");
	if (!in)
	{
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		fprintf(stderr, "unable to open file '%s', unrecoverable failure\n", filename);
		fclose(out);
		return 0;
	}

	char c;

	while ((c = fgetc(in)) != EOF)
	{
		fputc(c, out);
	}

	fclose(out);
	fclose(in);
	return 1;
}

long
findstring(const char *file, const char *str)
{
	/*
	 * find first occurance of substring 'str' in file 'output_dir'/'file'
	 * return offset of string beginning
	 * -1 on error (including not found)
	 *  
	 *  return value is a long obtained by ftell(), set the position later using fseek()
	 *  (use SEEK_SET as whence for beginning of file)
	 */

	long offset = -1;

	char filename[512] = {0};
	snprintf(filename, 512, "%s%s", output_dir, file);

	FILE *in = fopen(filename, "r");
	if (!in)
	{
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		fprintf(stderr, "unable to open file '%s', unrecoverable failure\n", filename);
		return -1;
	}

	/* hacky */
	char buffer[1024] = {0};
	char *strpos = NULL;
	while (fgets(buffer, 1024, in) != NULL)
	{
		strpos = strstr(buffer, str);
		if (strpos)
		{
			/* found our substring */
			break;
		}
		memset(buffer, 0, 1024);
		continue;
	}
	if (!strpos)
	{
		fprintf(stderr, "unable to find substring '%s' in '%s'\n", str, filename);
		fclose(in);
		return -1;
	}

	/* we have the line that holds our substring, find out where it is in the file */
	long curpos = ftell(in);
	if (!curpos)
	{
		fprintf(stderr, "ftell() failed\n");
		fclose(in);
		return -1;
	}
	// strpos-buffer = first byte of str in line
	offset = (curpos - strlen(buffer)+1) + strpos-buffer;

	fclose(in);
	return offset;
}

int
deletebytes(const char *file, long offset, size_t bytes)
{
	/*
	 * delete 'bytes' bytes from 'output_dir'/'file' starting at offset 'offset'
	 * return 1 on success. 0 on failure
	 */

	char filename[512] = {0};
	snprintf(filename, 512, "%s%s", output_dir, file);

	FILE *in = fopen(filename, "r");
	if (!in)
	{
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		fprintf(stderr, "unable to open file '%s', unrecoverable failure\n", filename);
		return 0;
	}

	FILE *out = fopen("tmp.tmp", "w");
	if (!out)
	{
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		fprintf(stderr, "unable to open temporary file, unrecoverable failure\n");
		fclose(in);
		return 0;
	}

	long curpos = 1;
	char c;
	/* read bytes from 'in' and write them to 'out', skipping 'bytes' bytes starting at 'offset' */
	while ((c = fgetc(in)) != EOF)
	{
		if (curpos >= offset && curpos <= ((offset-1)+bytes))
		{
			; // byte is one we want to skip
		}
		else
			fputc(c, out);

		curpos++;
	}

	/* delete 'filename' and move our temp file 'out' to take its place */
	fclose(in);
	if (remove(filename) == -1) 
	{

		fprintf(stderr, "unable to delete file '%s', unrecoverable failure\n", filename);
		fclose(out);
		return 0;
	}

	if (rename("tmp.tmp", filename) == -1)
	{
		fprintf(stderr, "unable to rename tmp file to '%s', unrecoverable failure\n", filename);
		fclose(out);
		return 0;
	}

	fclose(out);
	return 1;
}

int
writeatbyte(const char *dest, struct fileorstring *source, long offset)
{
	/*
	 * write  file 'source' into 'output_dir'/'file' starting at offset 'offset'
	 * return 1 on success. 0 on failure
	 */
	char filename[512] = {0};
	snprintf(filename, 512, "%s%s", output_dir, dest);

	FILE *in = fopen(filename, "r");
	if (!in)
	{
		fprintf(stderr, "unable to open file '%s', unrecoverable failure\n", filename);
		return 0;
	}

	FILE *tmp = fopen("tmp.tmp", "w");
	if (!tmp)
	{
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		fprintf(stderr, "unable to open temp file, unrecoverable failure\n");
		fclose(in);
		return 0;
	}

	FILE *src = NULL;
	if (source->file != NULL)
	{
		src = fopen(source->file, "r");
		if (!src)
		{
			fprintf(stderr, "Error opening file: %s\n", strerror(errno));
			fprintf(stderr, "unable to open file '%s', unrecoverable failure\n", source->file);
			fclose(in);
			fclose(tmp);
			return 0;
		}
	}

	/*
	 * read file 'in' until byte 'offset', writing into file 'tmp'
	 * if source->file isn't NULL, write file 'src' into 'tmp'
	 * otherwise write 'source->str' into file 'tmp'
	 * finish writing file 'in' into 'tmp'
	 * delete file 'filename'
	 * rename file 'tmp' 'filename'
	 */

	int c, t, a;
	long curpos = 1;
	int i = 0;
	while ((c = fgetc(in)) != EOF)
	{
		if (curpos == offset)
		{
			/* if we have a file open to get bytes to write from */
			if (src != NULL)
			{
				while ((t = fgetc(src)) != EOF)
				{
					/* prevent writing end of line '\n' if the next char is EOF */
					a = fgetc(src);
					ungetc(a, src);
					if (a == EOF && t == '\n')
						continue;
					fputc(t, tmp);
				}
			}
			/* otherwise we read from source->str */
			else
			{
				while ((t = source->str[i]) != '\0')
				{
					fputc(source->str[i], tmp);
					i++;
				}
			}
		}
		fputc(c, tmp);
		curpos++;
	}

	fclose(tmp);
	fclose(in);
	if (src)
		fclose(src);

	if (remove(filename) == -1)
	{
		fprintf(stderr, "unable to delete file '%s', unrecoverable failure\n", filename);
		return 0;
	}

	if (rename("tmp.tmp", filename) == -1)
	{
		fprintf(stderr, "unable to rename tmp file to '%s', unrecoverable failure\n", filename);
		return 0;
	}

	return 1;
}

int
replaceinpage(const char *outfile, const char *toreplace, struct fileorstring *source)
{
	/*
	 * replace 'toreplace' in file 'outfile' taking 'infile' as input
	 * return 1 on success, 0 on failure
	 */

	/* special case for ignoring a call so we can reuse the function */
	if (source->file == NULL && source->str == NULL)
		return 1;

	long substrpos = -1;

	substrpos = findstring(outfile, toreplace);

	if (substrpos < 0)
	{
		return 0;
	}

	if (!deletebytes(outfile, substrpos, strlen(toreplace)))
	{
		return 0;
	}

	if (!writeatbyte(outfile, source, substrpos))
	{
		return 0;
	}
	return 1;
}

char
*gettime(char *buffer, size_t size)
{
	/*
	 * put the current date into a buffer and return it
	 */

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	snprintf(buffer, size, "%d/%d/%d",  tm.tm_mday, tm.tm_mon+1, tm.tm_year + 1900);
	return buffer;
}

int
createtmpfile(const char *name, const char *content, size_t size)
{
	FILE *tmp = fopen(name, "w");
	if (!tmp)
	{
		fprintf(stderr, "unable to create temp file '%s', unrecoverable failure\n", name);
		return 0;
	}
	fwrite(content, 1, size, tmp);
	fclose(tmp);
	return 1;
}

int
genericpage(int flags, const char *output, const char *ind, const char *tit, const char *inf)
{
	if (!createfile(output, template_file))
	{
		fprintf(stderr, "unable to generate genericpage\n");
		return 0;
	}

	char date[255];
	struct fileorstring index = {ind, NULL};
	struct fileorstring title = {NULL, tit};
	struct fileorstring info = {NULL, inf};
	struct fileorstring time = {NULL, gettime(date, sizeof(date))};
	if (!replaceinpage(output, content_string, &index) ||
			!replaceinpage(output, title_string, &title) ||
			!replaceinpage(output, info_string, &info) ||
			!replaceinpage(output, time_string, &time))
	{
		fprintf(stderr, "unable to generate output\n");
		return 0;
	}

	return 1;

}

int
frontpage(int flags)
{
	return genericpage(flags, frontpage_index_output, frontpage_index, frontpage_title, frontpage_info);
}

int
likespage(int flags)
{
	return genericpage(flags, likes_content_output, likes_content, likes_title, likes_info);
}

int
dislikespage(int flags)
{
	return genericpage(flags, dislikes_content_output, dislikes_content, dislikes_title, dislikes_info);
}

int
interestingpage(int flags)
{
	return genericpage(flags, interesting_content_output, interesting_content, interesting_title, interesting_info);
}

int
opinionspage(int flags)
{
	return genericpage(flags, opinions_content_output, opinions_content, opinions_title, opinions_info);
}

int
opinions_animepage(int flags)
{
	return genericpage(flags, opinions_anime_content_output, opinions_anime_content, opinions_anime_title, opinions_anime_info);
}

int
opinions_everythingpage(int flags)
{
	return genericpage(flags, opinions_everything_content_output, opinions_everything_content, opinions_everything_title, opinions_everything_info);
}

int
portfoliopage(int flags)
{
	return genericpage(flags, portfolio_content_output, portfolio_content, portfolio_title, portfolio_info);
}

/* posts functions */

int
postscompare(const void *a, const void *b)
{
	return (*(int *)a - *(int *)b);
}

int
createdirectpages(const int *posts, size_t totalposts)
{
	/*
	 * create direct post files for each post
	 */

	char file[512];
	char source[512];
	for (int x = 1; x < totalposts; x++)
	{
		memset(source, 0, 1);
		memset(file, 0, 1);
		/* output file */
		snprintf(file, 512, "%s%d.html", direct_output_dir, posts[x]);

		/* source file */
		snprintf(source, 512, "%s%d.txt", posts_content, posts[x]);

		if (!createfile(file, template_file))
		{
			fprintf(stderr, "unable to create file '%s', unrecoverable failure\n", file);
			return 0;
		}

		if (!genericpage(NONE, file, source, posts_title, posts_info))
		{
			fprintf(stderr, "unable to generate direct post page '%s', unrecoverable failure\n", file);
			return 0;
		}
		//printf("%d\n", posts[x]);
	}
	return 1;
}
char
*generatepagebar(char *bar, size_t size, const int *posts, size_t totalposts, int currentpage, int pagecount)
{
	/*
	 * create a navigation bar for the posts pages highlighting the 'currentpage' link
	 * store it in bar and return it, NULL on error
	 */
	char buff[size];
	int i;
	size_t freespace = size;
	/* gross hack, sxprintf returns the number of chars written, keep track of our freespace */
	freespace -= snprintf(buff, freespace, "<div class='middle'> <a href='%d'>prev</a> ", (currentpage == 1) ? pagecount : currentpage-1);
	if (freespace <= 0) {fprintf(stderr, "out of space in buffer for generatepagebar()\n"); return NULL;}
	strncat(bar, buff, freespace);
	for (int i = 1; i <= pagecount; i++)
	{
		// FIXME: this doesn't need to be an if/else..
		if (currentpage == i)
		{
			freespace -= snprintf(buff, freespace, "<strong><i>%d</i></strong> ", i);
		}
		else
		{
			freespace -= snprintf(buff, freespace, "<a href='%d'>%d</a> ", i, i);
		}
		strncat(bar, buff, freespace);
	}
	freespace -= snprintf(buff, freespace, "<a href='%d'>next</a></div>", (currentpage == pagecount) ? 1 : currentpage+1);
	if (freespace <= 0) {fprintf(stderr, "out of space in buffer for generatepagebar()\n"); return NULL;}
	strncat(bar, buff, freespace);
	return bar;
}

int
writeposts(const int *posts, size_t totalposts, const char *outfile, int currentpage, int pagecount)
{
	/*
	 * write posts into 'output_dir'/'outfile'
	 * return 1 on success, 0 on failure
	 */
	char outfilename[512] = {0};
	snprintf(outfilename, 512, "%s%s", output_dir, outfile);
	char pagebar[512] = {0};
	if (generatepagebar(pagebar, 512, posts, totalposts, currentpage, pagecount) == NULL)
	{
			fprintf(stderr, "unable to generate pagebar for %s, unrecoverable failure\n", outfilename);
			return 0;
	}

	int start;
	int stop;
	start = 1 + (posts_per_page * currentpage) - posts_per_page;
	stop = start + posts_per_page;
	while (stop > (totalposts))
		stop--;

	int post;
	/* write posts to a temp file */
	char source[512];
	FILE *postfile;
	FILE *tmp = fopen("post.tmp", "a");
	if (!tmp)
	{
		fprintf(stderr, "unable to open temp file, unrecoverable failure\n");
		return 0;
	}
	fprintf(tmp, "%s\n", pagebar);
	for (int x = start; x < stop; x++)
	{
		/*
		 * post is the index into posts[] we actually want
		 * we do this because we want page 1 to contain the last blog posts not actually the first and so on
		 */
		post = totalposts - posts[x];
		fprintf(tmp, "post #%d<br>\n<a href='direct/%d.html'>direct link</a><br>\n", post, post);
		snprintf(source, 512, "%s%d.txt", posts_content, post);
		postfile = fopen(source, "r");
		if (!postfile)
		{
			fprintf(stderr, "unable to open temp file, unrecoverable failure\n");
			fclose(tmp);
			return 0;
		}
		char c;
		while ((c = fgetc(postfile)) != EOF)
		{
			fputc(c, tmp);
		}
		fprintf(tmp, "<hr>\n");
		fclose(postfile);
	}
	fprintf(tmp, "%s\n", pagebar);
	fclose(tmp);
	struct fileorstring replacement = {"post.tmp", NULL};
	if (!replaceinpage(outfile, content_string, &replacement))
	{
		fprintf(stderr, "unable to replace content in %s, unrecoverable failure\n", outfilename);
		return 0;
	}

	if (remove("post.tmp") == -1)
	{
		fprintf(stderr, "unable to remove temp file (post.tmp), unrecoverable failure\n");
		return 0;
	}

	return 1;
}

int
generatepostpages(const int *posts, size_t totalposts, int pagecount)
{
	/*
	 * create each blog page, there will be 'pagecount' of them
	 * containing 'posts_per_page' posts each
	 * return 1 on success, 0 on failure
	 */

	char outfilename[512] = {0};
	int postswritten = 0;

	for (int i = 1; i <= pagecount; i++)
	{
		/* loop through and create every post page required */
		snprintf(outfilename, 512, "%s%d.html", posts_output_dir, i);
		if (!createfile(outfilename, template_file))
		{
			fprintf(stderr, "unable to create file '%s', unrecoverable failure\n", outfilename);
			return 0;
		}

		if (!genericpage(NONE, outfilename, NULL, posts_title, posts_info))
		{
			fprintf(stderr, "unable to create generic page '%s', unrecoverable failure\n", outfilename);
			return 0;
		}
		if (!writeposts(posts, totalposts, outfilename, i, pagecount))
		{
			fprintf(stderr, "unable to create write posts to page %d', unrecoverable failure\n", i);
			return 0;
		}
	}

	return 1;
}

int
postspage(int flags)
{
	/*
	 * TODO: document
	 */

	/* get all files in the directory that are .txt */
	int posts[512]; // logical maximum number of posts
	size_t totalposts = 0;
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(posts_content)) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (strstr(ent->d_name, ".txt") != NULL)
			{
				int ign = 0;
				/* check if post should be ignored */
				for (int i = 0; i < sizeof(ignore)/sizeof(ignore[0]); i++)
				{
					if (atoi(ent->d_name) == ignore[i])
					{
						printf("ignoring post %d\n", atoi(ent->d_name));
						ign = 1;
					}
				}
				if (ign)
					continue;
				posts[totalposts] = atoi(ent->d_name); // gross
				totalposts++;
			}
		}
				closedir(dir);
	}
	else
	{
		/* could not open directory */
		fprintf(stderr, "Error opening directory: %s\n", strerror(errno));
		fprintf(stderr, "unable to open directory '%s', unrecoverable failure\n", posts_content);
		return 0;
	}

	/* sort posts */
	qsort(posts, totalposts, sizeof(int), postscompare);

	/* create direct link pages */
	if (!createdirectpages(posts, totalposts))
	{
		fprintf(stderr, "unable to create direct post pages, unrecoverable failure\n");
		return 0;
	}

	/* determine how many pages we need, if total posts divided by 'posts_per_page' isn't 0, we need 'posts_per_page'+1 pages */
	int pagecount = (totalposts%posts_per_page == 0) ? totalposts/posts_per_page : totalposts/posts_per_page+1;
	if (!generatepostpages(posts, totalposts, pagecount))
	{
		fprintf(stderr, "unable to create post pages, unrecoverable failure\n");
		return 0;
	}

	/* generate rss feed if required */
	if (flags & RSS)
	{
		generaterss(posts, totalposts);
	}
	return 1;
}

/* rss */

char
*striphtml(char *str, size_t size)
{
	/*
	 * strip html tags from 'str'
	 * dont copy newline
	 * return pointer to 'str'
	 */
	int idx = 0;
	int opened = 0; // false
	for(size_t i = 0; i < size; i++)
	{
		if(str[i] == '<')
		{
			opened = 1; // true
		}
		else if (str[i] == '>')
		{
			opened = 0; // false
		}
		else if (!opened)
		{
			str[idx++] = str[i];
		}
	}
	str[idx] = '\0';
	if (str[idx-1] == '\n')
		str[idx-1] = '\0';
	return str;
}

char
*rfc822date(char *date, size_t size)
{
	/*
	 * convert a dumb d/m/y date string into something rfc822 compliant
	 * absolutely not portable
	 */
	struct tm tm = {0};
	char buff[size];
	strptime(date, "%d/%m/%Y", &tm);
	strftime(buff, size, "%d %b %Y", &tm);
	strncpy(date, buff, size);
	snprintf(date, size, "%s 00:00:00 GMT", buff);
	return date;
}

char
*getimage(const char *line, char *buff, size_t size)
{
	long pos = 0;
	size_t linesize = strlen(line);
	while (((line[pos] != '>') && (line[pos] != '\0'))
				   && pos < linesize
				   && pos < size)
	{
		buff[pos] = line[pos];
		pos++;
	}
	buff[pos++] = '>';
	buff[pos++] = '\0';
	/* check src attribute for relative link and make it absolute */
	char b1[8] = {0};
	char newbuff[size-pos]; // ensure we dont exceed size, pos bytes already written
	char finalbuff[size];
	memset(finalbuff, 0, size);
	char *src = strstr(buff, "src=\"");
	pos = 5; // size of src="
	if (src)
	{
		src += pos;
		strncpy(b1, src, 4);
		if (strcmp(b1, "http") != 0) // naively assume an http means a url
		{
			fprintf(stderr, "%s: relative path in rss feed, fixing..\n", src);
			snprintf(newbuff, size-pos, "%s/%s", base_url, src);
			strncpy(finalbuff, buff, src-buff);
			strncat(finalbuff, newbuff, size-pos);
			strncpy(buff, finalbuff, size);
		}
	}

	return buff;
}

int
writerss(FILE *out, int post)
{
	/*
	 * create rss item using 'post_content'/'posts'.txt into FILE 'out'
	 * return 1 on success, 0 on fail
	 */

	if (!out)
		return 0;
	char buff[512];
	snprintf(buff, 512, "%s/%d.txt", posts_content, post);
	FILE *in = fopen(buff, "r");
	if (!in)
	{
		fprintf(stderr, "unable to open file %s, unrecoverable failure\n", buff);
		return 0;
	}

	char line[4096];
	char title[512] = {0};
	char date[512] = {0};
	char description[4096] = {0};
	char item[8195] = {0};
	char image[512] = {0};
	int hasimg = 0;
	int pos = 0;
	while ((fgets(line, 4096, in)) != NULL)
	{
		if (pos == 0)
		{
			/* should be the title */
			strncpy(title, line, 512);
		}
		else if (pos == 1)
		{
			/* should be the date */
			strncpy(date, line, 512);
		}

		else if (pos >= 2 && (title[0] == '\n' || date[0] == '\n'))
		{
			fprintf(stderr, "post %s has a broken format, please fix it\n", buff);
			fclose(in);
			return 0;
		}
		else if (pos == 3)
		{
			strncpy(description, line, 100);
		}

		/* try to find images */
		else if (pos > 3)
		{
			char *img;
			if ((img = strstr(line, "<img")))
			{
				hasimg = 1;
				getimage(img, image, 1024);
				break;
			}
		}
		pos++;
	}
	striphtml(title, strlen(title));
	striphtml(date, strlen(date));
	striphtml(description, strlen(description));
	strcat(description, " ...");
	rfc822date(date, 512);
	//printf("title is: %s date is: %s: desc is: %s\n", title, date, description);
	if (hasimg)
	{
		snprintf(item, 8195, "<item>\n\t<title>%s</title>\n\t<pubDate>%s</pubDate>\n\t<author>%s</author>\n\t<link>https://danieljon.es/posts/direct/%d</link>\n\t<guid>https://danieljon.es/posts/direct/%d</guid>\n\t<description>%s\n<br><![CDATA[%s]]> </description>\n</item>\n", title, date, author_string, post, post, description, image);
	}
	else
	{
		snprintf(item, 8195, "<item>\n\t<title>%s</title>\n\t<pubDate>%s</pubDate>\n\t<author>%s</author>\n\t<link>https://danieljon.es/posts/direct/%d</link>\n\t<guid>https://danieljon.es/posts/direct/%d</guid>\n\t<description>%s</description>\n</item>\n", title, date, author_string, post, post, description);
	}

	fprintf(out, "%s", item);
	fclose(in);
	return 1;
}

int
generaterss(const int *posts, size_t totalposts)
{
	/*
	 * generate an rss feed for our blog/posts
	 */

	if (!createfile(rss_output, rss_template))
	{
		fprintf(stderr, "unable to create %s, unrecoverable failure\n", rss_output);
		return 0;
	}

	/* open a temporary file for writing items to */
	FILE *tmp = fopen("rss.tmp", "w+");
	if (!tmp)
	{
		fprintf(stderr, "unable to create %s, unrecoverable failure\n", "rss.tmp");
		return 0;
	}

	/* loop through each post and write an rss file to our open file */
	int i;
	int towrite = post_count;
	while (towrite > totalposts)
		towrite--;
	for (i = 1; i < towrite; i++)
	{
		if (!writerss(tmp, (totalposts - posts[i])))
			return 0;
	}

	struct fileorstring src = {"rss.tmp", NULL};

	/* we need to flush the file, so just close it */
	fclose(tmp);

	if (!replaceinpage(rss_output, rss_string, &src))
	{
		fprintf(stderr, "unable to replace %s in %s, unrecoverable failure\n", rss_string, rss_output);
		return 0;
	}

	remove("rss.tmp");
	return 1;
}
