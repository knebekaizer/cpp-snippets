/*
**  GETSTRNG.C -- Demonstration of dynamic memory allocation to
**                receive string of unknown length.
**
**  Ron Sires 1/31/1989, released to the public domain.
**  Bob Stout 3/15/1992, restructure
**  Bob Stout 9/27/2009, republish
*/
 
#include <stdlib.h>
#include <stdio.h>
#include "getstrng.h"
 
#define BLOCKSIZ    16

/*
**  getstring() - Fetch a string of indeterminant length
**
**  Parameters: None
**
**  Returns: String or NULL if memory allocation error
**
**  Notes: 1 - It is the repsonsibility of the caller to free
**             the memory buffer returned by this function!
**         2 - Error/exception handling is the responsibility of
**             the caller.
**         3 - If reentrancy isn't required, this may be simpified
**             by the use of a static buffer.
*/


char *getstring(void)
{
      int    newchar;
      size_t i;
      char *buffer, *newbuf;
      size_t bufsize;


      /* Get initial BLOCKSIZ buffer to receive string.   */


      if ((buffer = (char *) calloc(BLOCKSIZ, sizeof(char))) == NULL)
            return NULL;
      bufsize = BLOCKSIZ;


      /* Get chars from keyboard and put them in buffer.  */


      for (i = 0; ((newchar = getchar()) != EOF) && (newchar != '\n')
            && (newchar != '\r'); /* no end term */ )
      {
            buffer[i] = (char) newchar;


            if (i >= bufsize - 1)         /* If buffer is full, resize it. */
            {
                  newbuf = (char *) realloc(buffer, bufsize + BLOCKSIZ);
                  if (newbuf == NULL)
                  {
                        free(buffer);
                        return NULL;
                  }
                  buffer = newbuf;
                  bufsize += BLOCKSIZ;
            }


            /* Add terminator to partial string & increment pointer */


            buffer[++i] = NUL;
      }
      return buffer;
}


#ifdef TEST


#include <string.h>


int main(void)
{
      char *string;


      puts("Enter strings of any length or <Enter> to quit\n");
      while (1)
      {
            string = getstring();
            if (NULL == string)
                  puts("\agetstrng() - Insufficient memory");
            printf("You entered:\n\"%s\"\n", string);
            printf("buffer=%p, length=%d\n\n", string, strlen(string));
            if (0 == strlen(string))
                  break;
            free(string);
      };
      return EXIT_SUCCESS;
}


#endif /* TEST */
