# Swap without a temporary variable

This is the public code companion for the episode. It contains reproducible examples, a small evidence ledger, and the C committee draft cited in the video. The private video-production workflow remains in the parent repository.

## Reproduce the experiments

The public companion does not require the private video workflow. With a C11 compiler:

```powershell
gcc -std=c11 -Wall -Wextra -Werror verification/swap_lab.c -o build/swap_lab.exe
build/swap_lab.exe

gcc -std=c11 -Wall -Wextra -Werror verification/i_subscript.c -o build/i_subscript.exe
build/i_subscript.exe
```

The first program checks `tmp` and guarded XOR swaps on distinct objects, then shows the unguarded same-object failure and the guarded rejection. The second checks `a[i]`, `i[a]`, `*(a+i)`, and `*(i+a)` on a concrete array.

The output demonstrates these implementations and inputs. It makes no claim that XOR swap is faster or that signed overflow has defined behavior.

See [`sources/EVIDENCE.md`](sources/EVIDENCE.md) for source scope and citations.
