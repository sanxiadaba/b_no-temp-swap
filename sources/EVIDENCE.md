# Evidence ledger

## XOR swap with aliased pointers

- **Claim:** For this three-step XOR implementation, passing the same object for both pointers changes its value to zero.
- **Primary evidence:** `verification/alias_swap.c`, compiled by `npm run build` with `gcc -std=c11 -Wall -Wextra -Werror`; captured output is `assets/evidence/alias_swap.stdout`.
- **Scope:** This is a reproducible execution of this implementation and input. It does not establish a performance claim or a general C-language rule.

## Signed integer overflow in C

- **Claim:** An arithmetic result outside the representable range of its signed integer type has undefined behavior under the C abstract machine.
- **Primary source:** WG14 N1570 draft, §6.5 paragraph 5, `sources/n1570.pdf`; public mirror: <https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf>.
- **Production rule:** Never use one observed wraparound result as evidence that signed-overflow arithmetic is portable or defined.

## LXGW WenKai typeface

- Bundled font slices and its OFL license are in `assets/fonts/lxgw-wenkai/`.
- Upstream: <https://github.com/lxgw/LxgwWenKai>.
