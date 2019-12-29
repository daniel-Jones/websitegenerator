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
createfile(const char *file)
{
	/*
	 * create file at location 'output_dir'/file' using the template
	 * overwrite if it exists
	 * assume the caller has put us in the base directory already
	 */

	/*
	 * FIXME: if trying to create a file with subdirectories it will not work
	 * one day tokenise the file path and make them
	 */
	
	struct stat sb = {0};

	if(!stat(output_dir, &sb) == 0 && !S_ISDIR(sb.st_mode))
	{
		fprintf(stderr, "directory '%s' does not exist, trying to make directory..\n", output_dir);
		if (mkdir(output_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0)
		{
			fprintf(stderr, "unable to mkdir '%s', unrecoverable failure\n", output_dir);
			return 0;
		}
		fprintf(stderr, "OK\n");
	}
	char filename[512] = {0};
	size_t length = 512;
	strncat(filename, output_dir, length-1);
	length = length - strlen(output_dir);
	strncat(filename, file, length-1);

	FILE *out = fopen(filename, "w");
	if (!out)
	{
		fprintf(stderr, "Error opening file: %s\n", strerror(errno));
		fprintf(stderr, "unable to open file '%s', unrecoverable failure\n", filename);
		return 0;
	}

	FILE *in = fopen(template_file, "r");
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
	size_t length = 512;
	strncat(filename, output_dir, length-1);
	length = length - strlen(output_dir);
	strncat(filename, file, length-1);

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
	size_t length = 512;
	strncat(filename, output_dir, length-1);
	length = length - strlen(output_dir);
	strncat(filename, file, length-1);

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
	size_t length = 512;
	strncat(filename, output_dir, length-1);
	length = length - strlen(output_dir);
	strncat(filename, dest, length-1);

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

	char c, t, a;
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
frontpage(int flags)
{
	if (!createfile(frontpage_index_output))
	{
		fprintf(stderr, "unable to generate frontpage\n");
		return 0;
	}

	char date[255];
	struct fileorstring index = {frontpage_index, NULL};
	struct fileorstring title = {NULL, frontpage_title};
	struct fileorstring info = {NULL, frontpage_info};
	struct fileorstring time = {NULL, gettime(date, sizeof(date))};
	if (!replaceinpage(frontpage_index_output, content_string, &index) ||
	    !replaceinpage(frontpage_index_output, title_string, &title) ||
	    !replaceinpage(frontpage_index_output, info_string, &info) ||
	    !replaceinpage(frontpage_index_output, time_string, &time))
	{
		fprintf(stderr, "unable to generate frontpage\n");
		return 0;
	}

	return 1;
}

int
likespage(int flags)
{
	if (!createfile(likes_content_output))
	{
		fprintf(stderr, "unable to generate likespage\n");
		return 0;
	}

	char date[255];
	struct fileorstring index = {likes_content, NULL};
	struct fileorstring title = {NULL, likes_title};
	struct fileorstring info = {NULL, likes_info};
	struct fileorstring time = {NULL, gettime(date, sizeof(date))};
	if (!replaceinpage(likes_content_output, content_string, &index) ||
	    !replaceinpage(likes_content_output, title_string, &title) ||
	    !replaceinpage(likes_content_output, info_string, &info) ||
	    !replaceinpage(likes_content_output, time_string, &time))
	{
		fprintf(stderr, "unable to generate likespage\n");
		return 0;
	}

	return 1;
}

int
dislikespage(int flags)
{
	if (!createfile(dislikes_content_output))
	{
		fprintf(stderr, "unable to generate dislikespage\n");
		return 0;
	}

	char date[255];
	struct fileorstring index = {dislikes_content, NULL};
	struct fileorstring title = {NULL, dislikes_title};
	struct fileorstring info = {NULL, dislikes_info};
	struct fileorstring time = {NULL, gettime(date, sizeof(date))};
	if (!replaceinpage(dislikes_content_output, content_string, &index) ||
	    !replaceinpage(dislikes_content_output, title_string, &title) ||
	    !replaceinpage(dislikes_content_output, info_string, &info) ||
	    !replaceinpage(dislikes_content_output, time_string, &time))
	{
		fprintf(stderr, "unable to generate dislikespage\n");
		return 0;
	}

	return 1;
}

int
interestingpage(int flags)
{
	if (!createfile(interesting_content_output))
	{
		fprintf(stderr, "unable to generate interestingspage\n");
		return 0;
	}

	char date[255];
	struct fileorstring index = {interesting_content, NULL};
	struct fileorstring title = {NULL, interesting_title};
	struct fileorstring info = {NULL, interesting_info};
	struct fileorstring time = {NULL, gettime(date, sizeof(date))};
	if (!replaceinpage(interesting_content_output, content_string, &index) ||
	    !replaceinpage(interesting_content_output, title_string, &title) ||
	    !replaceinpage(interesting_content_output, info_string, &info) ||
	    !replaceinpage(interesting_content_output, time_string, &time))
	{
		fprintf(stderr, "unable to generate interestingpage\n");
		return 0;
	}

	return 1;
}

int
opinionspage(int flags)
{
	if (!createfile(opinions_content_output))
	{
		fprintf(stderr, "unable to generate opinionspage\n");
		return 0;
	}

	char date[255];
	struct fileorstring index = {opinions_content, NULL};
	struct fileorstring title = {NULL, opinions_title};
	struct fileorstring info = {NULL, opinions_info};
	struct fileorstring time = {NULL, gettime(date, sizeof(date))};
	if (!replaceinpage(opinions_content_output, content_string, &index) ||
	    !replaceinpage(opinions_content_output, title_string, &title) ||
	    !replaceinpage(opinions_content_output, info_string, &info) ||
	    !replaceinpage(opinions_content_output, time_string, &time))
	{
		fprintf(stderr, "unable to generate opinionspage\n");
		return 0;
	}

	return 1;
}

int
opinions_animepage(int flags)
{
	if (!createfile(opinions_anime_content_output))
	{
		fprintf(stderr, "unable to generate opinions_animepage\n");
		return 0;
	}

	char date[255];
	struct fileorstring index = {opinions_anime_content, NULL};
	struct fileorstring title = {NULL, opinions_anime_title};
	struct fileorstring info = {NULL, opinions_anime_info};
	struct fileorstring time = {NULL, gettime(date, sizeof(date))};
	if (!replaceinpage(opinions_anime_content_output, content_string, &index) ||
	    !replaceinpage(opinions_anime_content_output, title_string, &title) ||
	    !replaceinpage(opinions_anime_content_output, info_string, &info) ||
	    !replaceinpage(opinions_anime_content_output, time_string, &time))
	{
		fprintf(stderr, "unable to generate opinions_animepage\n");
		return 0;
	}

	return 1;
}

int
opinions_everythingpage(int flags)
{
	if (!createfile(opinions_everything_content_output))
	{
		fprintf(stderr, "unable to generate opinions_everythingpage\n");
		return 0;
	}

	char date[255];
	struct fileorstring index = {opinions_everything_content, NULL};
	struct fileorstring title = {NULL, opinions_everything_title};
	struct fileorstring info = {NULL, opinions_everything_info};
	struct fileorstring time = {NULL, gettime(date, sizeof(date))};
	if (!replaceinpage(opinions_everything_content_output, content_string, &index) ||
	    !replaceinpage(opinions_everything_content_output, title_string, &title) ||
	    !replaceinpage(opinions_everything_content_output, info_string, &info) ||
	    !replaceinpage(opinions_everything_content_output, time_string, &time))
	{
		fprintf(stderr, "unable to generate opinions_everythingpage\n");
		return 0;
	}

	return 1;
}

int
portfoliopage(int flags)
{
	if (!createfile(portfolio_content_output))
	{
		fprintf(stderr, "unable to generate portfoliopage\n");
		return 0;
	}

	char date[255];
	struct fileorstring index = {portfolio_content, NULL};
	struct fileorstring title = {NULL, portfolio_title};
	struct fileorstring info = {NULL, portfolio_info};
	struct fileorstring time = {NULL, gettime(date, sizeof(date))};
	if (!replaceinpage(portfolio_content_output, content_string, &index) ||
	    !replaceinpage(portfolio_content_output, title_string, &title) ||
	    !replaceinpage(portfolio_content_output, info_string, &info) ||
	    !replaceinpage(portfolio_content_output, time_string, &time))
	{
		fprintf(stderr, "unable to generate portfoliopage\n");
		return 0;
	}

	return 1;
}

