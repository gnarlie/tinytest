/* TinyTest: A really really really tiny and simple no-hassle C unit-testing framework.
 *
 * Features:
 *   - No library dependencies. Not even itself. Just a header file.
 *   - Simple ANSI C. Should work with virtually every C or C++ compiler on
 *     virtually any platform.
 *   - Reports assertion failures, including expressions and line numbers.
 *   - Stops test on first failed assertion.
 *   - ANSI color output for maximum visibility.
 *   - Easy to embed in apps for runtime tests (e.g. environment tests).
 *
 * Example Usage:
 *
 *    #include "tinytest.h"
 *    #include "mylib.h"
 *
 *    void test_sheep()
 *    {
 *      ASSERT("Sheep are cool", are_sheep_cool());
 *      ASSERT_EQUALS(4, sheep.legs);
 *    }
 *
 *    void test_cheese()
 *    {
 *      ASSERT("Cheese is tangy", cheese.tanginess > 0);
 *      ASSERT_STRING_EQUALS("Wensleydale", cheese.name);
 *    }
 *
 *    int main()
 *    {
 *      RUN(test_sheep);
 *      RUN(test_cheese);
 *      return TEST_REPORT();
 *    }
 *
 * To run the tests, compile the tests as a binary and run it.
 *
 * Project home page: http://github.com/joewalnes/tinytest
 *
 * 2010-2014, -Joe Walnes <joe@walnes.com> http://joewalnes.com
 *
 *
 */

#ifndef _TINYTEST_INCLUDED
#define _TINYTEST_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* Main assertion method */
#define TT_ASSERT(msg, expression, evaluated) if (!tt_assert(__FILE__, __LINE__, (msg), (#expression), (evaluated) ? 1 : 0)) return;
#define ASSERT(msg, expression) TT_ASSERT(msg, expression, (expression))

/* Convenient assertion methods */
/* TODO: Generate readable error messages for assert_equals or assert_str_equals */
#define ASSERT_EQUALS(expected, actual) ASSERT((#actual), (expected) == (actual))

#define ASSERT_STRING_EQUALS(expected, actual) ASSERT((actual), strcmp((expected),(actual)) == 0)

#define ASSERT_INT_EQUALS(expected, actual) {int a = (actual), e = (expected); TT_ASSERT(tt_to_str(#actual " is %d, expected %s, which is %d", a, #expected, e), expected == actual, a == e);}

#define TEST_REPORT() tt_report()

#define TT_COLOR_CODE 0x1B
#define TT_COLOR_RED "[1;31m"
#define TT_COLOR_GREEN "[1;32m"
#define TT_COLOR_RESET "[0m"

int tt_passes __attribute__ ((weak));
int tt_fails __attribute__ ((weak));
int tt_current_test_failed __attribute__ ((weak));
const char* tt_current_msg __attribute__ ((weak));
const char* tt_current_test __attribute__ ((weak));
const char* tt_current_expression __attribute__ ((weak));
const char* tt_current_file __attribute__ ((weak));
int tt_current_line __attribute__ ((weak));

struct tt_test {
    void (*test_function)();
    const char * name;
    const char * suite;
    struct tt_test * next;
};

struct tt_test* tt_head __attribute__((weak));

#define TEST(name) \
    static void test_##name(); \
    __attribute__((__constructor__))    \
    static void register##name() {tt_register((#name), __FILE__, test_##name);} \
    static void test_##name()


static void tt_register(const char* name, const char * suite, void (*test_function)())
{
    struct tt_test * at = malloc(sizeof(struct tt_test));
    at->name = name;
    at->suite = suite;
    at->test_function = test_function;

    struct tt_test *after = tt_head, *before = NULL;
    while (after && strcmp(after->suite, suite) < 0) {
        before = after;
        after = after->next;
    }

    at->next = after;
    if (before)
    {
        before->next = at;
    }
    else
    {
        tt_head = at;
    }
}

static void tt_execute(const char* name, void (*test_function)())
{
  tt_current_test_failed = 0;
  tt_current_test = name;
  test_function();
  if (tt_current_test_failed) {
    tt_fails++;
  } else {
    tt_passes++;
  }
}

static int tt_assert(const char* file, int line, const char* msg, const char* expression, int pass)
{
  tt_current_msg = msg;
  tt_current_expression = expression;
  tt_current_file = file;
  tt_current_line = line;
  tt_current_test_failed = !pass;
  if (!pass) {
    printf("%s:%d: In test %s:\n    %s (%s)\n",
      file, line, tt_current_test, tt_current_msg, tt_current_expression);
  }
  return pass;
}

static int tt_report(void)
{
  if (tt_fails) {
    printf("%c%sFAILED%c%s [%s] (passed:%d, failed:%d, tests:%d)\n",
      TT_COLOR_CODE, TT_COLOR_RED, TT_COLOR_CODE, TT_COLOR_RESET,
      tt_current_file, tt_passes, tt_fails, tt_passes + tt_fails);
    tt_passes = tt_fails = 0;
    return -1;
  } else {
    printf("%c%sPASSED%c%s [%s] (tests:%d)\n", 
      TT_COLOR_CODE, TT_COLOR_GREEN, TT_COLOR_CODE, TT_COLOR_RESET,
      tt_current_file, tt_passes);
    tt_passes = tt_fails = 0;
    return 0;
  }
}

static const char * tt_to_str(const char * fmt, ...)
{
    static char __thread buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf,sizeof(buf), fmt, args);
    va_end(args);

    return buf;
}

static int tt_run_all()
{
    int rc = 0;

    for (struct tt_test* t = tt_head; t; t = t->next) {
        tt_execute(t->name, t->test_function);
        if (!t->next || 0 != strcmp(t->next->suite, t->suite)) {
            rc |= TEST_REPORT();
        }
    }

    while (tt_head) {
        struct tt_test * t = tt_head;
        tt_head = t->next;
        free(t);
    }

    return rc;
}

#endif
