//
// PDF to text program for PDFio.
//
// Copyright © 2022 by Michael R Sweet.
//
// Licensed under Apache License v2.0.  See the file "LICENSE" for more
// information.
//
// Usage:
//
//   ./pdfiototext FILENAME.pdf > FILENAME.txt
//

#include "pdfio.h"
#include <string.h>


//
// 'main()' - Main entry.
//

int					// O - Exit status
main()
{
  pdfio_file_t	*file;			// PDF file
  size_t	i, j,			// Looping vars
		num_pages,		// Number of pages
		num_streams;		// Number of streams for page
  pdfio_obj_t	*obj;			// Current page object
  pdfio_stream_t *st;			// Current page content stream
  char		buffer[1024],		// String buffer
		*pdf_file_buffer;		// In memory PDF buffer
  bool		first;			// First string token?
  FILE          *pdf_file;         // In memory PDF
  size_t        pdf_file_size;     // In memory PDF size


  // Read file into memory
  pdf_file = fopen("./test.pdf", "r");
  fseek(pdf_file, 0, SEEK_END);
  pdf_file_size = ftell(pdf_file);
  fseek(pdf_file, 0, SEEK_SET);
  pdf_file_buffer = malloc(pdf_file_size + 1);

  // Read file content into buffer
  size_t read_size = fread(pdf_file_buffer, 1, pdf_file_size, pdf_file);
  if (read_size != pdf_file_size) {
      perror("In memory file read failed");
      free(pdf_file_buffer);
      fclose(pdf_file);
      return 1;
  }

  pdf_file_buffer[pdf_file_size] = '\0';

  printf("file size %ld\n", pdf_file_size);

  // Close the file
  fclose(pdf_file);

  // Open the PDF file...
  if ((file = pdfioMemBufOpen(pdf_file_buffer, pdf_file_size, NULL, NULL, NULL, NULL)) == NULL)
    return (1);

//  printf("%s: %u pages\n", argv[1], (unsigned)pdfioFileGetNumPages(file));

  // Try grabbing content from all of the pages...
  for (i = 0, num_pages = pdfioFileGetNumPages(file); i < num_pages; i ++)
  {
    if ((obj = pdfioFileGetPage(file, i)) == NULL)
      continue;

    num_streams = pdfioPageGetNumStreams(obj);

//    printf("%s: page%u=%p, num_streams=%u\n", argv[1], (unsigned)i, obj, (unsigned)num_streams);

    for (j = 0; j < num_streams; j ++)
    {
      if ((st = pdfioPageOpenStream(obj, j, true)) == NULL)
	continue;

//      printf("%s: page%u st%u=%p\n", argv[1], (unsigned)i, (unsigned)j, st);

      first = true;
      while (pdfioStreamGetToken(st, buffer, sizeof(buffer)))
      {
	if (buffer[0] == '(')
	{
	  if (first)
	    first = false;
	  else
	    putchar(' ');

	  fputs(buffer + 1, stdout);
	}
	else if (!strcmp(buffer, "Td") || !strcmp(buffer, "TD") || !strcmp(buffer, "T*") || !strcmp(buffer, "\'") || !strcmp(buffer, "\""))
	{
	  putchar('\n');
	  first = true;
	}
      }

      if (!first)
	putchar('\n');

      pdfioStreamClose(st);
    }
  }

  pdfioFileClose(file);

  return (0);
}
