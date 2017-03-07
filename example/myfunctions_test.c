/* Tests for myfunctions.c, using TinyTest. */

#include "../tinytest.h"
#include "myfunctions.h"

TEST(test_addition)
{
  ASSERT_EQUALS(10, add_numbers(3, 7));
  ASSERT_EQUALS(-10, add_numbers(4, -14));
}

TEST(test_that_fails)
{
  /* this assertion fails - to demonstrate what happens */
  ASSERT_EQUALS(10, add_numbers(3, 33));
  /* because of the failure, this line won't even run */
  ASSERT_EQUALS(10, add_numbers(3, 7));
}

TEST(test_multiplication)
{
  ASSERT_EQUALS(12, multiply_numbers(3, 4));
  ASSERT_EQUALS(-12, multiply_numbers(4, -3));
}

/* test runner */
int main()
{
  return tt_run_all();
}
