# Live Demonstration, Teamwork, and Viva Guide

This guide follows the uploaded PPT's 90-second live demonstration plan, testing scenarios, GitHub requirements, and future-enhancement points.

## 90-second live demo

### 1. Launch the tool
```bash
make
./build/linux_debugging_system
```
Choose option 1 for the intentional leak test.

### 2. Show the leak case
Or run it directly:
```bash
./build/linux_debugging_system leak
```
Explain: "The program allocates one tracked 100-byte block and intentionally does not release it. The report should show one outstanding block and 100 potential leaked bytes."

### 3. Inspect the report
```bash
cat reports/memory_report.txt
```
Point out the outstanding block count, total potential leaked bytes, allocation source file and line, and saved report.

### 4. Show a clean case
```bash
./build/linux_debugging_system clean
```
Explain: "This test releases the same tracked block, so the report should show zero outstanding blocks and zero potential leaked bytes."

### Optional: partial release and repeated release
```bash
./build/linux_debugging_system partial
./build/linux_debugging_system invalid-free
```
The partial test should leave only the 200-byte block. The repeated-release test should show that the second call is a safe no-op because the pointer variable was cleared to `NULL`.

## Build and test before evaluation
```bash
make clean
make
make test
```
Only report test results as verified after running these commands on the presentation machine.

## GitHub checklist
- [ ] `src/main.c`, `src/leak_tracker.c`, and `src/leak_tracker.h` are included.
- [ ] `tests/test_plan.md` and `tests/run_tests.sh` are included.
- [ ] `README.md` includes setup, compile, run, and test instructions.
- [ ] `PROJECT_REPORT.md` and `Project_Report.docx` are included.
- [ ] The same PPT used for evaluation is included.
- [ ] Do not commit generated build files or temporary reports unless requested.

## Team contribution plan
The PPT lists these work areas. Adjust assignments together and do not claim work a member did not perform.
- **Implementation:** tracking API, block registry, and report generation.
- **Testing & integration:** leak/no-leak tests and connection with the existing Linux Debugging System menu.
- **Documentation:** README, project report, and architecture explanation.
- **Demo & presentation:** PPT, live run, and explaining actual results.

All team members should understand the project flow and limitations.

## Viva questions and short answers

1. **What is a memory leak?** Allocated memory that the program no longer needs but fails to release.
2. **What is dynamic memory allocation?** Requesting memory while a program runs, for example using `malloc()` or `calloc()`.
3. **What does `malloc()` do?** It requests a block of memory and returns a pointer, or `NULL` if allocation fails.
4. **What does `calloc()` do?** It allocates space for multiple elements and initializes the allocated bytes to zero.
5. **What does `free()` do?** It releases dynamically allocated memory.
6. **How does your tool track memory?** It records pointers and sizes for allocations routed through the tracking wrappers.
7. **How does it identify potential leaks?** It lists active registry entries when a report is requested.
8. **Why report both blocks and bytes?** Block count shows how many allocations remain; bytes show their combined size.
9. **Why record file and line?** They help locate the allocation site in the source code.
10. **Does it detect leaks in every Linux program?** No. Only allocations routed through this tracking API are visible in the baseline.
11. **Why say “potential” leak?** Some software intentionally keeps memory allocated until process exit.
12. **What is the purpose of the clean test?** It checks that a tracked allocation released through the tracker is no longer reported.
13. **How is a repeated release handled?** The `TRACK_FREE` macro clears the pointer variable to `NULL` after release, so repeating the call is a safe no-op. A non-null pointer unknown to the registry is warned about and ignored.
14. **What are the current limitations?** Wrapper-only tracking, fixed registry capacity, and no automatic interception of arbitrary binaries.
15. **What are future enhancements?** Add more allocation APIs, improve invalid-free detection, integrate the detector in the existing menu, and compare with Valgrind or AddressSanitizer.

## Important explanation to use if asked about integration
"This package has a runnable command-line entry point for the memory-detector demonstration. To merge it into our original Linux Debugging System, we need to add the tracker source to that system's compile command and call its test/report function from the existing menu. The original system source tree was not part of this package, so the merge point is documented rather than silently claiming those files were changed."
