#!/usr/bin/env bash
set -euo pipefail

BIN="./build/linux_debugging_system"
mkdir -p reports
pass() { printf '[PASS] %s\n' "$1"; }
fail() { printf '[FAIL] %s\n' "$1" >&2; exit 1; }

out=$("$BIN" leak reports/test_leak.txt 2>&1)
grep -q 'Potential leaked blocks  : 1' <<<"$out" || fail 'leak block count'
grep -q 'Potential leaked memory  : 100 bytes' <<<"$out" || fail 'leak byte count'
grep -q 'Status: MEMORY LEAK DETECTED.' <<<"$out" || fail 'leak status'
test -s reports/test_leak.txt || fail 'leak report file'
pass 'intentional leak and report file'

out=$("$BIN" clean reports/test_clean.txt 2>&1)
grep -q 'Potential leaked blocks  : 0' <<<"$out" || fail 'clean block count'
grep -q 'Potential leaked memory  : 0 bytes' <<<"$out" || fail 'clean byte count'
grep -q 'Status: NO TRACKED MEMORY LEAKS DETECTED.' <<<"$out" || fail 'clean status'
pass 'correct release'

out=$("$BIN" partial reports/test_partial.txt 2>&1)
grep -q 'Potential leaked blocks  : 1' <<<"$out" || fail 'partial block count'
grep -q 'Potential leaked memory  : 200 bytes' <<<"$out" || fail 'partial byte count'
pass 'partial release'

out=$("$BIN" invalid-free reports/test_invalid_free.txt 2>&1)
grep -q 'second release is a safe no-op' <<<"$out" || fail 'repeated release safety message'
grep -q 'Potential leaked blocks  : 0' <<<"$out" || fail 'invalid-free outstanding count'
pass 'repeated release is a safe no-op without double-free'

if "$BIN" unsupported >/dev/null 2>&1; then fail 'invalid mode should return non-zero'; fi
pass 'invalid command handling'

grep -q 'MEMORY LEAK DETECTION REPORT' reports/test_leak.txt || fail 'report content'
pass 'report content checks'

printf '\nAll automated checks passed.\n'
