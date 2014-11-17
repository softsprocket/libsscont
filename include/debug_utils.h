#ifndef DEBUG_UTILS_H_
#define DEBUG_UTILS_H_

#ifndef STDIO_HEADER_INCLUDED_
#define STDIO_HEADER_INCLUDED_
#include <stdio.h>
#endif // STDIO_HEADER_INCLUDED_


#define PERR(msg) perror (msg); fprintf (stderr, "%s %d\n", __FILE__, __LINE__)

#define PDEC() fprintf (stderr, "%s %d ", __FILE__, __LINE__)

#define PMSG(msg) fprintf (stderr, "%s %d: %s\n", __FILE__, __LINE__, msg) 

#define PFL() fprintf (stderr, "%s %d\n", __FILE__, __LINE__)

#endif // DEBUG_UTILS_H_

