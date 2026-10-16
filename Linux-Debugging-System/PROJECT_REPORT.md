# Project Report

## Linux Debugging System with Memory Leak Detection and Reporting Tool

**Hackathon:** 17 October 2026  
**Institution / Branch:** KL University, CSE  
**Guided by:** Dr. K. Anusha  
**Presented by:** 2520030034 - P. Prathik; 2520030188 - U. Praneeth; 2520030573 - M. Harshith

## 1. Abstract
This project is a C-based Linux prototype that tracks selected dynamic memory allocations, records matching release operations, identifies outstanding tracked blocks, calculates their count and size, and writes a report to the terminal and a text file. The implementation follows the workflow shown in the presentation: test program, tracked API, block registry, leak checker, and report output.

## 2. Problem Statement
Dynamic memory lets a program request memory at runtime. When a program forgets to release memory that is no longer needed, that block can remain allocated until the process exits. Repeated leaks can accumulate in long-running programs, increase memory consumption, put pressure on performance, and make debugging harder.

## 3. Project Goal and Objectives
The goal is to make tracked memory state visible to a developer. The specific objectives are to record tracked allocations, track release operations, identify remaining blocks, calculate block count and bytes, save a readable report, and provide a command-line entry point for the memory detector within the Linux Debugging System workflow.

## 4. Proposed Workflow
1. **Track allocation:** calls routed through `TRACK_MALLOC` or `TRACK_CALLOC` record a pointer and requested size.
2. **Track release:** `TRACK_FREE` finds the matching active entry, updates its state, releases the memory, and clears the pointer variable to `NULL`.
3. **Compare records:** the leak checker searches for registry entries still active when the report is generated.
4. **Create report:** the report includes outstanding block details, total counts, byte totals, and status.
5. **Save output:** the same report is printed to the terminal and written to the selected text file.

## 5. Core Features
- Allocation tracking, including pointer, requested byte size, source file, and source line.
- Release tracking for calls made through the tracking API.
- Identification of outstanding tracked records.
- Calculation of outstanding block count and combined byte size.
- Console reporting and `memory_report.txt` generation.
- A runnable menu and CLI test modes suitable for demonstrating the memory detector.

## 6. System Design
The system consists of a test program, tracked allocation/release wrappers, a block registry, a leak checker, and a report output module. The demo entry point offers intentional-leak, correct-release, partial-release, and repeated-release-safety tests. The project's report does not claim visibility into memory calls that bypass the tracking API.

## 7. Technologies Used
- **C programming:** allocation tracking and reporting logic.
- **Ubuntu Linux:** development and test environment.
- **GCC:** compilation with warnings and debug information.
- **Git / GitHub:** version control and project sharing.
- **Standard C library:** allocation, release, terminal output, and file output.

## 8. Testing and Validation
The test plan follows the presentation's controlled scenarios:

| Test | Setup | Expected result |
|---|---|---|
| Intentional leak | Allocate 100 bytes and do not release it. | 1 outstanding block and 100 bytes reported. |
| Correct release | Allocate 100 bytes and release it. | 0 outstanding blocks and 0 bytes. |
| Partial release | Allocate 100, 200, and 300 bytes; release the first and third. | Only the 200-byte block remains. |
| Repeated release | Release an allocation and attempt a second release. | Warning; second `free()` avoided; no outstanding tracked block. |
| Report creation | Supply a report path. | Report file created with totals and status. |
| Invalid mode | Use an unsupported command. | Usage message and non-zero exit code. |

Run `make test` to execute the automated test script. `TEST_RESULTS.txt` in the delivered package records the checked outcomes from the package build environment.

## 9. Results and Report Format
For the intentional-leak scenario, the expected output is one outstanding block and 100 potential leaked bytes. For the correct-release scenario, the expected output is zero outstanding blocks and zero bytes. The partial-release test should retain only the 200-byte block. The example values are expected test results; actual results should be demonstrated by running the supplied program.

## 10. Limitations and Scope
The `TRACK_FREE(pointer_variable)` macro sets that pointer variable to `NULL` after a successful release, preventing the common repeated-call case from invoking `free()` twice. The baseline sees only calls routed through `TRACK_MALLOC`, `TRACK_CALLOC`, and `TRACK_FREE`. It is not a universal scanner for arbitrary or unmodified binaries. A remaining allocation is reported as a *potential* leak because some programs intentionally retain memory until exit. The registry has a fixed capacity of 4096 simultaneously active records; if the capacity is exceeded, new tracked allocations fail safely and the report indicates incomplete tracking. Valgrind and AddressSanitizer are future comparison/integration options, not claimed capabilities of this baseline.

## 11. Responsible Implementation
The intentional leak is small, runs only in a short-lived local demonstration process, and exists to validate the report path. The prototype does not probe other users' processes or interfere with system services. Testing should remain controlled and reproducible.

## 12. Team Contribution Plan
The presentation identifies four work areas: implementation (tracker, registry, reporting), testing and integration (leak/no-leak cases and Linux menu integration), documentation (README, report, architecture), and demo/presentation. The team should agree on the final member-to-role assignment and ensure every member can explain the complete workflow.

## 13. Future Enhancements
- Record and report allocation source locations consistently.
- Detect invalid or repeated frees more comprehensively.
- Support more allocation APIs.
- Compare or integrate results with Valgrind or AddressSanitizer.
- Merge the memory detector entry point into the team's existing Linux Debugging System menu/source tree.

## 14. Conclusion
The prototype turns selected memory operations into debugging evidence. It tracks allocations and releases, identifies outstanding tracked blocks, calculates count and byte totals, and writes a readable report. Controlled tests make the behavior demonstrable and repeatable, while the documented scope avoids overclaiming what the tracker can detect.
