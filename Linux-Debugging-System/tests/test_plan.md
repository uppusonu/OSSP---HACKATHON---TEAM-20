# Testing and Validation Plan

These cases follow the Testing & Validation slide of the project presentation. Expected values below are assertions to verify by running the program, not substitutes for actual run output.

| ID | Test | Action | Expected result |
|---|---|---|---|
| T01 | Intentional leak | Allocate one 100-byte block and do not release it. | 1 outstanding block; 100 potential leaked bytes; leak status displayed. |
| T02 | Correct release | Allocate 100 bytes and release the same tracked pointer. | 0 outstanding blocks; 0 potential leaked bytes; no tracked leaks status. |
| T03 | Partial release | Allocate 100, 200, and 300 bytes; release 100 and 300 byte blocks. | Only the 200-byte block remains listed. |
| T04 | Repeated release | Release one tracked pointer and then attempt to release it again. | Warning printed; second `free()` is not attempted; 0 outstanding blocks. |
| T05 | Report file | Run a test with a report path. | File exists and contains counts, byte totals, outstanding block details, and status. |
| T06 | Invalid command | Run with an unsupported mode. | Usage instructions printed and non-zero status returned. |

## Run tests

```bash
make test
```

## Validation checklist
- [ ] Compare the reported block count with the test's intended remaining blocks.
- [ ] Verify byte totals match the test allocations still outstanding.
- [ ] Confirm `reports/memory_report.txt` or the selected path is created.
- [ ] Verify source file and line information are shown for outstanding blocks.
- [ ] Test invalid/repeated release safely.
- [ ] Do not claim the detector sees arbitrary uninstrumented programs.
