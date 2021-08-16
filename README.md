# MWC Pseudo-Random Number Generator

This is a re-implementation of the Multiply With Carry Pseudo-Random Number Generator
as described by its [wikipedia page](https://en.wikipedia.org/wiki/Multiply-with-carry_pseudorandom_number_generator).

Parameters are kept the same, however this implementation differs by providing
a pure API on the random functions in a single .h file and allowing the initial
random generator function to be defined by the user.


API
---

This random number generator is split in 4 functions. All of them are pure
except for the `mwc_create()`.

- `mwc_create()`: creates the `mwc_random_t` data type, this is not a pure
     method because it makes use of the MWC_SYSTEM_RAND32 function.
- `mwc_init()`: initializes the created mwc_random_t with the startup
     values
- `mwc_read_random(state)`: returns the random value (uint32_t), this is a
     pure method, it always produces the same random number if the state is
     not advanced by the `mwc_next_random()`
- `mwc_next_random(state)`: creates a new state with another random value that
     can then be read by the `mwc_read_random()`


Example Usage
-------------

```C
#include "mwc.h"

// Create and initialize the `mwc_random_t` struct
mwc_random_t rnd = mwc_init(mwc_create());

// Pure API, `mwc_read_random()` will not change the `mwc_random_t`
// and always produces the same value.
uint32_t value1 = mwc_read_random(rnd);
// To create a new `mwc_random_t` state, pass the previous state to
// the `mwc_next_random()` function
rnd = mwc_next_random(rnd);

uint32_t value2 = mwc_read_random(rnd);
rnd = mwc_next_random(rnd);

uint32_t value3 = mwc_read_random(rnd);
```

The `mwc_create()` makes use of the system rand function, by default it uses
the `arc4random()` from `<stdlib.h>` (available in BSDs/macOS). If your system
does not have this function or if you want to supply your own seed
initialization function please define the `MWC_SYSTEM_RAND32` preprocessor
value to match your function.

Here is an example

```C
#define MWC_SYSTEM_RAND32 my_rand
#include "mwc.h"
// Defining the system random function used to create the seed array
uint32_t my_rand(void) {
  return 4;
}
```
