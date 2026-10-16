#include "leak_tracker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_REPORT "reports/memory_report.txt"

static int report_and_save(const char *report_path) {
    leak_report(stdout);
    if (leak_report_to_file(report_path) != 0) {
        fprintf(stderr, "Could not write report file: %s\n", report_path);
        return 1;
    }
    printf("Report saved to: %s\n", report_path);
    return 0;
}

static int run_leak_case(const char *report_path) {
    char *block = TRACK_MALLOC(100);
    if (!block) {
        fprintf(stderr, "Allocation failed.\n");
        return 1;
    }
    strcpy(block, "Intentional 100-byte leak demonstration");
    printf("TEST: intentional leak - allocated one 100-byte block and did not release it.\n");
    /* Do not TRACK_FREE(block): this is a short-lived, intentional demo leak. */
    return report_and_save(report_path);
}

static int run_clean_case(const char *report_path) {
    char *block = TRACK_MALLOC(100);
    if (!block) {
        fprintf(stderr, "Allocation failed.\n");
        return 1;
    }
    strcpy(block, "Allocation released correctly");
    TRACK_FREE(block);
    printf("TEST: correct release - allocated and released the same 100-byte block.\n");
    return report_and_save(report_path);
}

static int run_partial_case(const char *report_path) {
    void *a = TRACK_MALLOC(100);
    void *b = TRACK_MALLOC(200);
    void *c = TRACK_MALLOC(300);
    if (!a || !b || !c) {
        fprintf(stderr, "Allocation failed; releasing any successful tracked allocations.\n");
        TRACK_FREE(a); TRACK_FREE(b); TRACK_FREE(c);
        return 1;
    }
    TRACK_FREE(a);
    TRACK_FREE(c);
    printf("TEST: partial release - allocated 100, 200, and 300 bytes; released 100 and 300.\n");
    printf("Expected outstanding block: 1 block, 200 bytes.\n");
    /* Leave b outstanding to demonstrate partial cleanup. */
    return report_and_save(report_path);
}

static int run_invalid_free_case(const char *report_path) {
    void *block = TRACK_MALLOC(100);
    if (!block) {
        fprintf(stderr, "Allocation failed.\n");
        return 1;
    }
    TRACK_FREE(block);
    TRACK_FREE(block); /* The macro clears block to NULL after the successful release. */
    TRACK_FREE(block); /* Safe no-op: block is now NULL, so free() is not called again. */
    printf("TEST: repeated release - second release is a safe no-op because the pointer was cleared to NULL.\n");
    return report_and_save(report_path);
}

static void print_help(const char *program) {
    printf("Linux Debugging System - Memory Leak Detection Tool\n\n");
    printf("Usage: %s [leak|clean|partial|invalid-free|--help] [report-file]\n", program);
    printf("  leak        One un-released 100-byte tracked block\n");
    printf("  clean       Allocate and release a 100-byte tracked block\n");
    printf("  partial     Leave only the 200-byte block outstanding\n");
    printf("  invalid-free Demonstrate safe warning for repeated release\n");
    printf("Report file defaults to %s\n", DEFAULT_REPORT);
}

static int run_menu(const char *program) {
    char input[32];
    printf("===============================================\n");
    printf(" LINUX DEBUGGING SYSTEM\n");
    printf(" Memory Leak Detection and Reporting Tool\n");
    printf("===============================================\n");
    printf("1. Run intentional leak test\n");
    printf("2. Run correct release test\n");
    printf("3. Run partial release test\n");
    printf("4. Test repeated-release safety\n");
    printf("5. Help\n");
    printf("Choose an option (1-5): ");
    if (!fgets(input, sizeof(input), stdin)) return 1;
    switch (input[0]) {
        case '1': return run_leak_case(DEFAULT_REPORT);
        case '2': return run_clean_case(DEFAULT_REPORT);
        case '3': return run_partial_case(DEFAULT_REPORT);
        case '4': return run_invalid_free_case(DEFAULT_REPORT);
        case '5': print_help(program); return 0;
        default: fprintf(stderr, "Invalid selection. Run %s --help.\n", program); return 2;
    }
}

int main(int argc, char **argv) {
    const char *mode = argc > 1 ? argv[1] : NULL;
    const char *report_path = argc > 2 ? argv[2] : DEFAULT_REPORT;

    if (!mode) return run_menu(argv[0]);
    if (strcmp(mode, "--help") == 0 || strcmp(mode, "-h") == 0) {
        print_help(argv[0]);
        return 0;
    }
    if (strcmp(mode, "leak") == 0) return run_leak_case(report_path);
    if (strcmp(mode, "clean") == 0) return run_clean_case(report_path);
    if (strcmp(mode, "partial") == 0) return run_partial_case(report_path);
    if (strcmp(mode, "invalid-free") == 0) return run_invalid_free_case(report_path);

    fprintf(stderr, "Unknown mode: %s\n", mode);
    print_help(argv[0]);
    return 2;
}
