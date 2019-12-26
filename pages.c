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
	 * create file at location 'file' using the template
	 * overwrite if it exists
	 * assume the caller has put us in the base directory already
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

		fprintf(stderr, "unable to open file '%s', unrecoverable failure\n", filename);
		return 0;
	}

	FILE *in = fopen(template_file, "r");
	if (!in)
	{
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
		fprintf(stderr, "unable to open file '%s', unrecoverable failure\n", filename);
		return 0;
	}

	FILE *out = fopen("tmp.tmp", "w");
	if (!out)
	{
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
frontpage(int flags)
{
	if (!createfile(frontpage_index_output))
	{
		fprintf(stderr, "unable to generate frontpage\n");
		return 0;
	}

	long substrpos = -1;

	substrpos = findstring(frontpage_index_output, "cirno");

	if (substrpos < 0)
	{
		fprintf(stderr, "unable to generate frontpage\n");
		return 0;
	}

	printf("first occurance of '%s' is at byte %ld\n", "cirno", substrpos);

	
	if (!deletebytes(frontpage_index_output, substrpos, strlen("cirno")))
	{
		fprintf(stderr, "unable to generate frontpage\n");
		return 0;
	}
	return 1;
}
