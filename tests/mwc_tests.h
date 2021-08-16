#ifndef MWC_TESTS_H
#include "./munit.h" // The external unit tests framework - µnit

/*
** Test that the `mwc_random_t` exists
*/
static MunitResult
test_random_exists(const MunitParameter params[], void* data) {
    mwc_random_t rnd = {0};
    
    munit_assert_not_null(&rnd);
    return MUNIT_OK;
}

/*
** Test that the `mwc_random_t` has a q attribute
*/
static MunitResult
test_q_exists(const MunitParameter params[], void* data) {
    mwc_random_t rnd = {0};
    
    munit_assert_not_null(&rnd.mwc_q);
    munit_assert_uint(sizeof(rnd.mwc_q) / sizeof(uint32_t), ==, MWC_CYCLE);
    return MUNIT_OK;
}

/*
** Test that the `mwc_random_t` has a carry attribute.
*/
static MunitResult
test_carry_exists(const MunitParameter params[], void* data) {
    mwc_random_t rnd = {0};
    
    munit_assert_not_null(&rnd.mwc_carry);
    return MUNIT_OK;
}

/*
** Test that the `mwc_random_t` has a cycle variable.
*/
static MunitResult
test_cycle_exists(const MunitParameter params[], void* data) {
    mwc_random_t rnd = {0};
    
    munit_assert_not_null(&rnd.mwc_current_cycle);
    return MUNIT_OK;
}

/*
** Test that a default MWC_SYSTEM_RAND32 implementation exists and works as
** expected.
*/
static MunitResult
test_has_system_rand(const MunitParameter params[], void* data) {
    
  uint32_t rnd_value1 = MWC_SYSTEM_RAND32();
  uint32_t rnd_value2 = MWC_SYSTEM_RAND32();
  uint32_t rnd_value3 = MWC_SYSTEM_RAND32();
  // Should start as zero
  munit_logf(MUNIT_LOG_INFO, "First system random: %u", rnd_value1);
  munit_logf(MUNIT_LOG_INFO, "Second system random: %u", rnd_value2);
  munit_logf(MUNIT_LOG_INFO, "Third system random: %u", rnd_value3);

  munit_assert_true(rnd_value1 != rnd_value2);
  munit_assert_true(rnd_value2 != rnd_value3);
  munit_assert_true(rnd_value1 != rnd_value3);

  return MUNIT_OK;
}

/*
** Test that a create function exists and works as expected
*/
static MunitResult
test_create_exists(const MunitParameter params[], void* data) {
  mwc_random_t rnd = mwc_create();
    
  munit_assert_uint(rnd.mwc_current_cycle, ==, MWC_CYCLE - 1);
  munit_assert_uint(rnd.mwc_carry, !=, 0);
  munit_assert_uint(rnd.mwc_carry, <, MWC_C_MAX);
  munit_assert_uint(rnd.mwc_carry, ==, rnd.mwc_initial_carry);
  // All values must be different from each other
  for (size_t i = 0; i < MWC_CYCLE; ++i) {
    for (size_t j = 0; j < MWC_CYCLE; ++j) {
      if (j == i) continue;
      munit_assert_uint(rnd.mwc_q[i], !=, rnd.mwc_q[j]);
    }
  }
  // Initial Q must be the same as the Q returned from the create function
  //
  for (size_t i = 0; i < MWC_CYCLE; ++i) {
      munit_assert_uint(rnd.mwc_q[i], ==, rnd.mwc_initial_q[i]);
  }
  return MUNIT_OK;
}

/*
** Test that an init function exists and works as expected
*/
static MunitResult
test_init_exists(const MunitParameter params[], void* data) {
  mwc_random_t rnd = mwc_init(mwc_create());
    
  munit_assert_uint(rnd.mwc_current_cycle, ==, MWC_CYCLE - 1);
  munit_assert_uint(rnd.mwc_carry, !=, 0);
  munit_assert_uint(rnd.mwc_carry, <, MWC_C_MAX);
  munit_assert_uint(rnd.mwc_carry, ==, rnd.mwc_initial_carry);

  // Initial Q must be the same as the Q returned from the create function
  //
  for (size_t i = 0; i < MWC_CYCLE; ++i) {
      munit_assert_uint(rnd.mwc_q[i], ==, rnd.mwc_initial_q[i]);
  }
  return MUNIT_OK;
}

/*
** Test that a read function exists and can return an immediate value
*/
static MunitResult
test_read_exists(const MunitParameter params[], void* data) {
  mwc_random_t rnd = mwc_init(mwc_create());
  uint32_t value = mwc_read_random(rnd);
    
  munit_assert_uint(value, !=, 0);
  
  // The function should be pure
  uint32_t value2 = mwc_read_random(rnd);
  uint32_t value3 = mwc_read_random(rnd);
  munit_logf(MUNIT_LOG_INFO, "Multiple random values: %u, %u, %u", value, value2, value3);
  munit_assert_uint(value, ==, value2);
  munit_assert_uint(value, ==, value3);

  return MUNIT_OK;
}

/*
** Test that a gen function exists and can generate a new random value
*/
static MunitResult
test_gen_exists(const MunitParameter params[], void* data) {
  mwc_random_t rnd = mwc_create();
  rnd = mwc_init(rnd);
  uint32_t value1 = mwc_read_random(rnd);
  rnd = mwc_next_random(rnd);
  uint32_t value2 = mwc_read_random(rnd);
  rnd = mwc_next_random(rnd);
  uint32_t value3 = mwc_read_random(rnd);
  munit_logf(MUNIT_LOG_INFO, "Multiple random values: %u, %u, %u", value1, value2, value3);
    
  munit_assert_uint(value1, !=, 0);
  munit_assert_uint(value2, !=, 0);
  munit_assert_uint(value3, !=, 0);

  // Values must be different from each other
  munit_assert_uint(value1, !=, value2);
  munit_assert_uint(value1, !=, value3);
  munit_assert_uint(value2, !=, value3);
  
  // The function should be pure
  uint32_t new_value1 = mwc_read_random(rnd);
  uint32_t new_value2 = mwc_read_random(rnd);
  munit_logf(MUNIT_LOG_INFO, "Multiple execution values: %u, %u, %u", value3, new_value1, new_value2);
  munit_assert_uint(value3, ==, new_value2);
  munit_assert_uint(value3, ==, new_value1);

  return MUNIT_OK;
}

/*
** Test simple uniform distribution expeactations.
*/
static MunitResult
test_is_uniform(const MunitParameter params[], void* data) {
  mwc_random_t rnd = mwc_init(mwc_create());

  // Generate 10k random values
#define MWC_TEST_ITERATIONS 10000
  uint32_t values[MWC_TEST_ITERATIONS] = {0};
  // Their sum must average at 0.5
  double sum = 0;
  // Number of values generated below the 10% capacity
  uint32_t total_below_10 = 0;
  for (size_t i = 0; i < MWC_TEST_ITERATIONS; ++i) {
    values[i] = mwc_read_random(rnd);
    rnd = mwc_next_random(rnd);
    sum += ((double)values[i]) / ((double)UINT32_MAX);

    if (values[i] < (UINT32_MAX / 10)) {
      total_below_10++;
    }
  }

  // The average should be 0.5 (when adjusted to the 0.0-1.0 range)
  double avg = sum / ((double)MWC_TEST_ITERATIONS);
  munit_logf(MUNIT_LOG_INFO, "Avg: %f, Sum: %f", avg, sum);
  munit_assert_double_equal(avg, 0.5, 2);

  // There must be at least 10% values below the 10% of the maximum
  munit_logf(MUNIT_LOG_INFO, "Total below 10%%: %u", total_below_10);
  double avg10 = ((double)total_below_10) / ((double)(MWC_TEST_ITERATIONS / 10));
  munit_assert_double_equal(avg10, 1.0, 1);

  return MUNIT_OK;
}

/*
** The main() function is included to be able to run the mwc tests directly in
** the CLI. This function is the unit tests entry-point.
*/
int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
    // The array of tests to run
    static MunitTest all_tests[] = {
        { (char*) "test_random_exists", test_random_exists,
            0, 0, MUNIT_TEST_OPTION_NONE, 0 },
        { (char*) "test_q_exists", test_q_exists,
            0, 0, MUNIT_TEST_OPTION_NONE, 0 },
        { (char*) "test_carry_exists",
          test_carry_exists,
            0, 0, MUNIT_TEST_OPTION_NONE, 0 },
        { (char*) "test_cycle_exists",
          test_cycle_exists,
            0, 0, MUNIT_TEST_OPTION_NONE, 0 },
        { (char*) "test_has_system_rand",
          test_has_system_rand,
            0, 0, MUNIT_TEST_OPTION_NONE, 0 },
        { (char*) "test_create_exists",
          test_create_exists,
            0, 0, MUNIT_TEST_OPTION_NONE, 0 },
        { (char*) "test_init_exists",
          test_init_exists,
            0, 0, MUNIT_TEST_OPTION_NONE, 0 },
        { (char*) "test_read_exists",
          test_read_exists,
            0, 0, MUNIT_TEST_OPTION_NONE, 0 },
        { (char*) "test_gen_exists",
          test_gen_exists,
            0, 0, MUNIT_TEST_OPTION_NONE, 0 },

        { (char*) "test_is_uniform",
          test_is_uniform,
            0, 0, MUNIT_TEST_OPTION_NONE, 0 },

        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    };
    // Single test:
    static MunitTest single_test[] = {
        { (char*) "test_random_exists",
            test_random_exists,
            0, 0, MUNIT_TEST_OPTION_NONE, 0 },
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL },
    };
    const MunitSuite test_suite = {
        // This string will be prepended to all test names in this suite
        (char*) "mwc/",
        // The array of test suites.
        // single_test,
        all_tests,
        // Suites can contain other test suites. This isn't necessary for mwc
        NULL,
        // Number of iterations of each test
        1,
        // Use the munit default settings
        MUNIT_SUITE_OPTION_NONE
    };
    return munit_suite_main(&test_suite, (void*) "munit", argc, argv);
}
#endif /* MWC_TESTS_H */
