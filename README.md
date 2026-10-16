Linux Debugging System — Memory Leak Detection and Reporting Tool

Hackathon: 17 October 2026
Institution: KL University, CSE
Guided by: Dr. K. Anusha
Presented by: 2520030034 - P. Prathik; 2520030188 - U. Praneeth; 2520030573 - M. Harshith

Project summary

This C-based Linux prototype tracks dynamic memory allocations made through its tracked allocation functions, compares them with tracked release operations, identifies outstanding tracked blocks, calculates their count and total size, and saves a readable report. It is designed to support the Memory Leak Detection and Reporting Tool feature of the Linux Debugging System.

Features aligned with the presentation

Allocation tracking: records pointer, byte size, source file, and line for calls through TRACK_MALLOC or TRACK_CALLOC.

Release tracking: marks a tracked block released through TRACK_FREE.

Leak identification: lists registry entries still active when the report is requested.

Byte calculation: reports outstanding block count and total potential leaked bytes.

Report generation: prints results to the terminal and saves them to reports/memory_report.txt (or a chosen path).

Linux Debugging System entry point: running the program without arguments opens a small demonstration menu; the command-line modes can be called from the existing system's menu.

Requirements

Ubuntu/Linux

GCC and make

A terminal

Compile

From the extracted project directory:

make

Equivalent direct compile command:

mkdir -p build reports
gcc -Wall -Wextra -Wpedantic -std=c11 -g -Isrc src/main.c src/leak_tracker.c -o build/linux_debugging_system

Run the live menu

./build/linux_debugging_system

Choose one of the displayed test options.

Run reproducible test cases

./build/linux_debugging_system leak
./build/linux_debugging_system clean
./build/linux_debugging_system partial
./build/linux_debugging_system invalid-free

Choose a specific report path if needed:

./build/linux_debugging_system leak reports/leak_case.txt
cat reports/leak_case.txt

Run automated checks:

make test

How the tracker works

The test program requests memory through TRACK_MALLOC or TRACK_CALLOC.

The tracking layer records the returned pointer, size, source file, and source line in a registry.

TRACK_FREE finds the corresponding active record, marks it released, calls free(), and clears the pointer variable to NULL.

The TRACK_FREE(pointer_variable) macro clears the variable to NULL after a successful release, making a repeated call a safe no-op. An untracked non-null pointer produces a warning and is not passed to free().

The report lists active records and totals their memory sizes.

Integrating into an existing Linux Debugging System menu

The submitted package includes a demonstration menu as the memory detector's command-line entry point. To connect it to an existing Linux Debugging System source tree, add src/leak_tracker.c to that project's compile command and include src/leak_tracker.h wherever allocations are instrumented. Replace selected allocation/release pairs with:

char *buffer = TRACK_MALLOC(100);
if (buffer != NULL) {
    /* use buffer */
    TRACK_FREE(buffer);
}

Call leak_report(stdout) or leak_report_to_file("reports/memory_report.txt") before the relevant test finishes. A menu option can call the appropriate demo/test function or launch the detector command. The existing Linux Debugging System source files were not part of this package, so the menu-merge step must be made in that source tree; this package itself is runnable and includes a detector menu for demonstration.

Test cases and expected outcomes

Test

Expected result

leak

1 outstanding tracked block; 100 potential leaked bytes

clean

0 outstanding tracked blocks; 0 potential leaked bytes

partial

1 outstanding tracked block; 200 potential leaked bytes

invalid-free

Second release is a safe no-op; no second call to free(); 0 outstanding blocks

help / invalid mode

Usage text; unknown mode returns a non-zero exit status

report file

Report is saved at the given path

Important scope and limitations

The baseline detects only allocations routed through TRACK_MALLOC / TRACK_CALLOC and releases routed through TRACK_FREE. Direct malloc() calls elsewhere are invisible unless changed to the tracking API.

An outstanding block is reported as a potential leak; some programs intentionally retain allocations until exit.

The registry supports up to 4096 simultaneous tracked blocks. If full, the allocation is failed safely and the final report marks tracking as incomplete.

This project is a controlled educational prototype, not a replacement for production-grade tools such as Valgrind or AddressSanitizer.

Repository layout

Linux-Debugging-System/
├── src/
│   ├── main.c
│   ├── leak_tracker.c
│   └── leak_tracker.h
├── tests/
│   ├── test_plan.md
│   └── run_tests.sh
├── reports/
├── README.md
├── PROJECT_REPORT.md
├── Project_Report.docx
├── Hackathon_Presentation.pptx
├── DEMO_AND_VIVA.md
├── Makefile
└── .gitignore

Responsible testing

The intentional-leak mode allocates a small block, omits its release on purpose, writes a report, and then exits. Use it only as a short-lived local demonstration. The tracker does not probe other users' processes or interfere with system services.
