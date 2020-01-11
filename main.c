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

#include <stdio.h>
#include "config.h"
#include "pages.h"

int
main(void)
{
	for (size_t i = 0; i < sizeof(pages)/sizeof(pages[0]); i++)
	{
		if (!pages[i].function(pages[i].flags))
		{
			fprintf(stderr, "error generating website\n");
			return 1;
		}
	}
	return 0;
}
