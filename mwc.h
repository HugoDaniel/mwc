#ifndef MWC_H
/*
**                                               
**           ____                                
**         ,'  , `.            .---.   ,----..   
**      ,-+-,.' _ |           /. ./|  /   /   \  
**   ,-+-. ;   , ||       .--'.  ' ; |   :     : 
**  ,--.'|'   |  ;|      /__./ \ : | .   |  ;. / 
** |   |  ,', |  ':  .--'.  '   \' . .   ; /--`  
** |   | /  | |  || /___/ \ |    ' ' ;   | ;     
** '   | :  | :  |, ;   \  \;      : |   : |     
** ;   . |  ; |--'   \   ;  `      | .   | '___  
** |   : |  | ,       .   \    .\  ; '   ; : .'| 
** |   : '  |/         \   \   ' \ | '   | '/  : 
** ;   | |`-'           :   '  |--"  |   :    /  
** |   ;/                \   \ ;      \   \ .'   
** '---'                  '---"        `---`     
**
** This is an implementation of the Marsaglia's Multiply With Carry random
** number generator. It provides a pure API on top of the implementation in
** its Wikipedia article in:
** https://en.wikipedia.org/wiki/Multiply-with-carry_pseudorandom_number_generator
**
** All of it is defined in this single .h file that you can include in your
** projects.
**
** The API makes use of a system random number generator to create the initial
** seeds. By default the `arc4random()` from stdlib.h is used. If your
** system does not support it, or if you want to use something different, then
** define the MWC_SYSTEM_RAND32 macro value to the desired method.
** 
** This random number generator is split in 4 functions. 
**
** - mwc_create(): creates the mwc_random_t data type, this is not a pure
**     method because it makes use of the MWC_SYSTEM_RAND32 function.
** - mwc_init(): initializes the created mwc_random_t with the startup
**     values
** - mwc_read_random(state): returns the random value (uint32_t), this is a
**     pure method, it always produces the same random number if the state is
**     not advanced by the `mwc_next_random()`
** - mwc_next_random(): generates another random value that can then be read
**     by the `mwc_read_random()`
**
** A simple usage example:
**
**   mwc_random_t rnd = mwc_init(mwc_create());
**
**   uint32_t value1 = mwc_read_random(rnd);
**   rnd = mwc_next_random(rnd);
**
**   uint32_t value2 = mwc_read_random(rnd);
**   rnd = mwc_next_random(rnd);
**
**   uint32_t value3 = mwc_read_random(rnd);
**   ...
**
*/

#define MWC_H (1)
#ifdef __cplusplus
extern "C" {
#endif

#ifndef MWC_SYSTEM_RAND32
#include <stdlib.h> // arc4random()
#define MWC_SYSTEM_RAND32 arc4random
#endif

#include <stddef.h> // size_t
#include <stdint.h> // uint32_t, uint64_t

#define MWC_CYCLE 4096         // as Marsaglia recommends
#define MWC_C_MAX 809430660    // as Marsaglia recommends

typedef struct mwc_random_t {
  // The initial seed array, and where each cycle random values are
  // stored. These can be read with the method `mwc_read_random()`.
	uint32_t mwc_q[MWC_CYCLE];
  // Carry is limited to MWC_C_MAX on initialization.
	uint32_t mwc_carry;
	unsigned mwc_current_cycle;
  // Initial carry value is kept to allow the random generator
  // to be reset or to time travel if required.
	uint32_t mwc_initial_carry;
  // The initial state values are also kept, for the same reasons
	uint32_t mwc_initial_q[MWC_CYCLE];
} mwc_random_t;

/*
** Internal function that creates a carry that is guaranteed to be < MWC_C_MAX.
** This function uses the defined MWC_SYSTEM_RAND32 system random.
*/
static inline uint32_t mwc_initial_c(void) {
  uint32_t mwc_initial_carry = 0;

  do
		mwc_initial_carry = MWC_SYSTEM_RAND32();
	while (mwc_initial_carry >= MWC_C_MAX);

  return mwc_initial_carry;
}

/*
** Creates a new mwc random state. 
** Keeps a copy of the initial state to allow `mwc_init()` to
** be able to reset it to the same initial state, allowing for the 
** random numbers generation sequence to be replicated again if needed.
*/
static inline mwc_random_t
mwc_create() {
  uint32_t mwc_c = mwc_initial_c();
  mwc_random_t mwc_state = {
    .mwc_initial_carry = mwc_c,
    .mwc_carry = mwc_c,
    .mwc_current_cycle = MWC_CYCLE -1,
  };

 	for (size_t i = 0; i < MWC_CYCLE; i++) {
    mwc_state.mwc_q[i] = MWC_SYSTEM_RAND32();
    mwc_state.mwc_initial_q[i] = mwc_state.mwc_q[i];
  }

  return mwc_state;
}

/*
** Resets to the initial state.
*/
static inline mwc_random_t
mwc_init(mwc_random_t r) {
  r.mwc_carry = r.mwc_initial_carry;
  r.mwc_current_cycle = MWC_CYCLE -1;

 	for (size_t i = 0; i < MWC_CYCLE; i++) {
    r.mwc_q[i] = r.mwc_initial_q[i];
  }

  return r;
}

/*
** Returns the random value for the supplied mwc_random_t state.
** This is a pure function, it always returns the same value for the
** same input argument.
**
** To generate a new random number call `state = mwc_next_random(state)` before
** calling this method.
*/
static inline uint32_t
mwc_read_random(mwc_random_t const state) {
  return state.mwc_q[state.mwc_current_cycle];
}

/*
** This function creates a new random state for the next random number.
** 
** To retrieve the random number created, call `mwc_read_random(state)` after
** this method.
*/
static inline mwc_random_t
mwc_next_random(mwc_random_t state) {
  uint64_t const a = 18782;	// as Marsaglia recommends
	uint32_t const m = 0xfffffffe;	// as Marsaglia recommends
	uint64_t t;
	uint32_t x;

	state.mwc_current_cycle = (state.mwc_current_cycle + 1) & (MWC_CYCLE - 1);
	t = a * state.mwc_q[state.mwc_current_cycle] + state.mwc_carry;
	/* Let c = t / 0xffffffff, x = t mod 0xffffffff */
	state.mwc_carry = t >> 32;
	x = (uint32_t)t + state.mwc_carry;
	if (x < state.mwc_carry) {
		x++;
		state.mwc_carry++;
	}

  state.mwc_q[state.mwc_current_cycle] = m - x;

  return state;
}


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MWC_H */


/*-- MARK: Tests -------------------------------------------------------------*/
/*
** Include the unit tests framework if mwc is being built for tests
**/
#ifdef MWC_TESTS

#include "./tests/mwc_tests.h"
#endif /* MWC_TESTS */

