
#ifndef PHALCON_RELEASE
#if defined(linux)

#include <execinfo.h>
#include <Zend/zend.h>
#include <ext/standard/php_smart_str.h>

/**
 * A buffer for backtrace. It is better to have it allocated statically
 * in order not to face out of memory conditions later
 */
void *backtrace_buf[4096];

void zephir_print_backtrace(void)
{
	int i;
	int stack_size       = backtrace(backtrace_buf, sizeof(backtrace_buf) / sizeof(void*));
	char **stack_symbols = backtrace_symbols(backtrace_buf, stack_size);
	char buf[50];
	smart_str s;

	s.c = NULL;

	for (i = 0; i < stack_size; ++i) {
		snprintf(buf, sizeof(buf), "#%d  %p [", i, backtrace_buf[i]);
		smart_str_appends(&s, buf);
		smart_str_appends(&s, stack_symbols[i]);
		smart_str_appends(&s, "]\n");
	}

	smart_str_0(&s);

	fprintf(stderr, "%s\n", s.c);
	smart_str_free(&s);
}

#else

void zephir_print_backtrace(void)
{
	/**
	 * Not implemented yet for anything other than Linux
	 */
}

#endif
#endif /* PHALCON_RELEASE */
