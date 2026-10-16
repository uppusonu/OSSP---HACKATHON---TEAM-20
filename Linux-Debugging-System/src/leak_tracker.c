#include "leak_tracker.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TRACKED_BLOCKS 4096

typedef struct {
    void *ptr;
    size_t size;
    const char *alloc_file;
    int alloc_line;
    int active;
} Allocation;

static Allocation registry[MAX_TRACKED_BLOCKS];
static size_t total_allocations;
static size_t total_released_blocks;
static size_t total_bytes_allocated;
static size_t total_bytes_released;
static size_t tracking_overflow;
static size_t invalid_release_attempts;

static int record_allocation(void *ptr, size_t size, const char *file, int line) {
    for (size_t i = 0; i < MAX_TRACKED_BLOCKS; i++) {
        if (!registry[i].active) {
            registry[i].ptr = ptr;
            registry[i].size = size;
            registry[i].alloc_file = file;
            registry[i].alloc_line = line;
            registry[i].active = 1;
            total_allocations++;
            total_bytes_allocated += size;
            return 1;
        }
    }
    tracking_overflow++;
    return 0;
}

void *tracked_malloc(size_t size, const char *file, int line) {
    void *ptr = malloc(size);
    if (!ptr) return NULL;
    if (!record_allocation(ptr, size, file, line)) {
        /* Fail safely rather than return memory the tracker cannot account for. */
        free(ptr);
        return NULL;
    }
    return ptr;
}

void *tracked_calloc(size_t count, size_t size, const char *file, int line) {
    if (size != 0 && count > SIZE_MAX / size) return NULL;
    size_t bytes = count * size;
    void *ptr = calloc(count, size);
    if (!ptr) return NULL;
    if (!record_allocation(ptr, bytes, file, line)) {
        free(ptr);
        return NULL;
    }
    return ptr;
}

void *tracked_free(void *ptr, const char *file, int line) {
    if (!ptr) return NULL;
    for (size_t i = 0; i < MAX_TRACKED_BLOCKS; i++) {
        if (registry[i].active && registry[i].ptr == ptr) {
            size_t bytes = registry[i].size;
            registry[i].active = 0;
            registry[i].ptr = NULL;
            total_released_blocks++;
            total_bytes_released += bytes;
            free(ptr);
            return NULL;
        }
    }
    /* Do not call free() for an unknown pointer: that could cause a double/invalid free. */
    invalid_release_attempts++;
    fprintf(stderr,
            "WARNING: ignored untracked release at %s:%d.\n",
            file ? file : "(unknown)", line);
    return ptr;
}

void leak_report(FILE *out) {
    if (!out) out = stdout;
    size_t leaked_blocks = 0;
    size_t leaked_bytes = 0;

    fprintf(out, "\n==================================================\n");
    fprintf(out, "       MEMORY LEAK DETECTION REPORT\n");
    fprintf(out, "==================================================\n");
    fprintf(out, "Total allocations        : %zu\n", total_allocations);
    fprintf(out, "Total released blocks    : %zu\n", total_released_blocks);
    fprintf(out, "Bytes allocated          : %zu\n", total_bytes_allocated);
    fprintf(out, "Bytes released           : %zu\n", total_bytes_released);
    fprintf(out, "Untracked release attempts : %zu\n", invalid_release_attempts);
    fprintf(out, "Tracking table overflow  : %zu\n", tracking_overflow);
    fprintf(out, "--------------------------------------------------\n");
    fprintf(out, "Outstanding tracked blocks:\n");

    for (size_t i = 0; i < MAX_TRACKED_BLOCKS; i++) {
        if (registry[i].active) {
            leaked_blocks++;
            leaked_bytes += registry[i].size;
            fprintf(out, "  #%zu: %zu bytes allocated at %s:%d\n",
                    leaked_blocks, registry[i].size,
                    registry[i].alloc_file ? registry[i].alloc_file : "(unknown)",
                    registry[i].alloc_line);
        }
    }

    fprintf(out, "--------------------------------------------------\n");
    fprintf(out, "Potential leaked blocks  : %zu\n", leaked_blocks);
    fprintf(out, "Potential leaked memory  : %zu bytes\n", leaked_bytes);
    if (tracking_overflow != 0) {
        fprintf(out, "Status: INCOMPLETE - TRACKING CAPACITY EXCEEDED.\n");
    } else if (leaked_blocks != 0) {
        fprintf(out, "Status: MEMORY LEAK DETECTED.\n");
    } else {
        fprintf(out, "Status: NO TRACKED MEMORY LEAKS DETECTED.\n");
    }
    fprintf(out, "Note: only allocations through TRACK_MALLOC/TRACK_CALLOC\n");
    fprintf(out, "and releases through TRACK_FREE are visible to this tracker.\n");
    fprintf(out, "==================================================\n");
}

int leak_report_to_file(const char *path) {
    if (!path || !*path) return -1;
    FILE *out = fopen(path, "w");
    if (!out) return -1;
    time_t now = time(NULL);
    char *stamp = ctime(&now);
    if (stamp) fprintf(out, "Generated: %s", stamp);
    leak_report(out);
    int result = fclose(out);
    return result == 0 ? 0 : -1;
}
