#ifndef LEAK_TRACKER_H
#define LEAK_TRACKER_H

#include <stddef.h>
#include <stdio.h>

/* Use TRACK_FREE with a pointer variable allocated by TRACK_MALLOC or
 * TRACK_CALLOC. The macro clears that variable to NULL after a successful release. */
void *tracked_malloc(size_t size, const char *file, int line);
void *tracked_calloc(size_t count, size_t size, const char *file, int line);
void *tracked_free(void *ptr, const char *file, int line);
void leak_report(FILE *out);
int leak_report_to_file(const char *path);

#define TRACK_MALLOC(size) tracked_malloc((size), __FILE__, __LINE__)
#define TRACK_CALLOC(count, size) tracked_calloc((count), (size), __FILE__, __LINE__)
#define TRACK_FREE(ptr) ((ptr) = tracked_free((ptr), __FILE__, __LINE__))

#endif
