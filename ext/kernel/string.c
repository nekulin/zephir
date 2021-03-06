
#include "php.h"
#include "php_test.h"
#include "php_main.h"

#include "ext/standard/php_smart_str.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_rand.h"
#include "ext/standard/php_lcg.h"
#include "ext/standard/base64.h"
#include "ext/standard/md5.h"

#if HAVE_BUNDLED_PCRE
#include "ext/pcre/php_pcre.h"
#endif

#include "kernel/main.h"
#include "kernel/memory.h"

#define PH_RANDOM_ALNUM 0
#define PH_RANDOM_ALPHA 1
#define PH_RANDOM_HEXDEC 2
#define PH_RANDOM_NUMERIC 3
#define PH_RANDOM_NOZERO 4

/**
 * Fast call to php strlen
 */
void zephir_fast_strlen(zval *return_value, zval *str){

	zval copy;
	int use_copy = 0;

	if (Z_TYPE_P(str) != IS_STRING) {
		zend_make_printable_zval(str, &copy, &use_copy);
		if (use_copy) {
			str = &copy;
		}
	}

	ZVAL_LONG(return_value, Z_STRLEN_P(str));

	if (use_copy) {
		zval_dtor(str);
	}
}

/**
 * Fast call to php strlen
 */
void zephir_fast_strtolower(zval *return_value, zval *str){

	zval copy;
	int use_copy = 0;
	char *lower_str;
	unsigned int length;

	if (Z_TYPE_P(str) != IS_STRING) {
		zend_make_printable_zval(str, &copy, &use_copy);
		if (use_copy) {
			str = &copy;
		}
	}

	length = Z_STRLEN_P(str);
	lower_str = estrndup(Z_STRVAL_P(str), length);
	php_strtolower(lower_str, length);

	if (use_copy) {
		zval_dtor(str);
	}

	ZVAL_STRINGL(return_value, lower_str, length, 0);
}

/**
 * Fast call to php join  function
 */
void zephir_fast_join(zval *result, zval *glue, zval *pieces TSRMLS_DC){

	if (Z_TYPE_P(glue) != IS_STRING || Z_TYPE_P(pieces) != IS_ARRAY) {
		ZVAL_NULL(result);
		zend_error(E_WARNING, "Invalid arguments supplied for join()");
		return;
	}

	php_implode(glue, pieces, result TSRMLS_CC);
}

/**
 * Appends to a smart_str a printable version of a zval
 */
void zephir_append_printable_zval(smart_str *implstr, zval **tmp TSRMLS_DC) {

	zval tmp_val;
	unsigned int str_len;

	switch ((*tmp)->type) {
		case IS_STRING:
			smart_str_appendl(implstr, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
			break;

		case IS_LONG:
			smart_str_append_long(implstr, Z_LVAL_PP(tmp));
			break;

		case IS_BOOL:
			if (Z_LVAL_PP(tmp) == 1) {
				smart_str_appendl(implstr, "1", sizeof("1") - 1);
			}
			break;

		case IS_NULL:
			break;

		case IS_DOUBLE: {
			char *stmp;
			str_len = spprintf(&stmp, 0, "%.*G", (int) EG(precision), Z_DVAL_PP(tmp));
			smart_str_appendl(implstr, stmp, str_len);
			efree(stmp);
		}
			break;

		case IS_OBJECT: {
			int copy;
			zval expr;
			zend_make_printable_zval(*tmp, &expr, &copy);
			smart_str_appendl(implstr, Z_STRVAL(expr), Z_STRLEN(expr));
			if (copy) {
				zval_dtor(&expr);
			}
		}
			break;

		default:
			tmp_val = **tmp;
			zval_copy_ctor(&tmp_val);
			convert_to_string(&tmp_val);
			smart_str_appendl(implstr, Z_STRVAL(tmp_val), Z_STRLEN(tmp_val));
			zval_dtor(&tmp_val);
			break;
	}
}

/**
 * Fast join function
 * This function is an adaption of the php_implode function
 *
 */
void zephir_fast_join_str(zval *return_value, char *glue, unsigned int glue_length, zval *pieces TSRMLS_DC){

	zval         **tmp;
	HashTable      *arr;
	HashPosition   pos;
	smart_str      implstr = {0};
	unsigned int   numelems, i = 0;

	if (Z_TYPE_P(pieces) != IS_ARRAY) {
		ZVAL_NULL(return_value);
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid arguments supplied for fast_join()");
		return;
	}

	arr = Z_ARRVAL_P(pieces);
	numelems = zend_hash_num_elements(arr);

	if (numelems == 0) {
		RETURN_EMPTY_STRING();
	}

	zend_hash_internal_pointer_reset_ex(arr, &pos);

	while (zend_hash_get_current_data_ex(arr, (void **) &tmp, &pos) == SUCCESS) {
		zephir_append_printable_zval(&implstr, tmp TSRMLS_CC);
		if (++i != numelems) {
			smart_str_appendl(&implstr, glue, glue_length);
		}
		zend_hash_move_forward_ex(arr, &pos);
	}
	smart_str_0(&implstr);

	if (implstr.len) {
		RETURN_STRINGL(implstr.c, implstr.len, 0);
	} else {
		smart_str_free(&implstr);
		RETURN_EMPTY_STRING();
	}
}

/**
 * Convert dash/underscored texts returning camelized
 */
void zephir_camelize(zval *return_value, const zval *str){

	unsigned int i;
	smart_str camelize_str = {0};
	char *marker, ch;

	if (Z_TYPE_P(str) != IS_STRING) {
		zend_error(E_WARNING, "Invalid arguments supplied for camelize()");
		RETURN_EMPTY_STRING();
		return;
	}

	marker = Z_STRVAL_P(str);
	for (i = 0; i < Z_STRLEN_P(str); i++) {
		ch = *marker;
		if (ch == '\0') {
			break;
		}
		if (i == 0 || ch == '-' || ch == '_') {
			if (ch == '-' || ch == '_') {
				i++;
				marker++;
				ch = *marker;
			}
			if (ch >= 'a' && ch <= 'z') {
				smart_str_appendc(&camelize_str, (*marker) - 32);
			} else {
				smart_str_appendc(&camelize_str, (*marker));
			}
			marker++;
			continue;
		}
		if (ch >= 'A' && ch <= 'Z') {
			smart_str_appendc(&camelize_str, (*marker) + 32);
		} else {
			smart_str_appendc(&camelize_str, (*marker));
		}
		marker++;
	}
	smart_str_0(&camelize_str);

	if (camelize_str.len) {
		RETURN_STRINGL(camelize_str.c, camelize_str.len, 0);
	} else {
		RETURN_EMPTY_STRING();
	}

}

/**
 * Convert dash/underscored texts returning camelized
 */
void zephir_uncamelize(zval *return_value, const zval *str){

	unsigned int i;
	smart_str uncamelize_str = {0};
	char *marker, ch;

	if (Z_TYPE_P(str) != IS_STRING) {
		zend_error(E_WARNING, "Invalid arguments supplied for camelize()");
		return;
	}

	marker = Z_STRVAL_P(str);
	for (i = 0; i < Z_STRLEN_P(str); i++) {
		ch = *marker;
		if (ch == '\0') {
			break;
		}
		if (ch >= 'A' && ch <= 'Z') {
			if (i > 0) {
				smart_str_appendc(&uncamelize_str, '_');
			}
			smart_str_appendc(&uncamelize_str, (*marker) + 32);
		} else {
			smart_str_appendc(&uncamelize_str, (*marker));
		}
		marker++;
	}
	smart_str_0(&uncamelize_str);

	if (uncamelize_str.len) {
		RETURN_STRINGL(uncamelize_str.c, uncamelize_str.len, 0);
	} else {
		RETURN_EMPTY_STRING();
	}
}

/**
 * Fast call to explode php function
 */
void zephir_fast_explode(zval *result, zval *delimiter, zval *str){

	if (Z_TYPE_P(str) != IS_STRING || Z_TYPE_P(delimiter) != IS_STRING) {
		ZVAL_NULL(result);
		zend_error(E_WARNING, "Invalid arguments supplied for explode()");
		return;
	}

	array_init(result);
	php_explode(delimiter, str, result, LONG_MAX);
}

/**
 * Fast call to explode php function
 */
void zephir_fast_explode_str(zval *result, const char *delimiter, int delimiter_length, zval *str){

	zval delimiter_zval;

	if (Z_TYPE_P(str) != IS_STRING) {
		ZVAL_NULL(result);
		zend_error(E_WARNING, "Invalid arguments supplied for explode()");
		return;
	}

	ZVAL_STRINGL(&delimiter_zval, delimiter, delimiter_length, 0);

	array_init(result);
	php_explode(&delimiter_zval, str, result, LONG_MAX);
}

/**
 * Check if a string is contained into another
 */
int zephir_memnstr(const zval *haystack, const zval *needle) {

	char *found = NULL;

	if (Z_TYPE_P(haystack) != IS_STRING || Z_TYPE_P(needle) != IS_STRING) {
		zend_error(E_WARNING, "Invalid arguments supplied for memnstr()");
		return 0;
	}

	if (Z_STRLEN_P(haystack) >= Z_STRLEN_P(needle)) {
		found = php_memnstr(Z_STRVAL_P(haystack), Z_STRVAL_P(needle), Z_STRLEN_P(needle), Z_STRVAL_P(haystack) + Z_STRLEN_P(haystack));
	}

	if (found) {
		return 1;
	}

	return 0;
}

/**
 * Check if a string is contained into another
 */
int zephir_memnstr_str(const zval *haystack, char *needle, unsigned int needle_length) {

	char *found = NULL;

	if (Z_TYPE_P(haystack) != IS_STRING) {
		zend_error(E_WARNING, "Invalid arguments supplied for memnstr()");
		return 0;
	}

	if (Z_STRLEN_P(haystack) >= needle_length) {
		found = php_memnstr(Z_STRVAL_P(haystack), needle, needle_length, Z_STRVAL_P(haystack) + Z_STRLEN_P(haystack));
	}

	if (found) {
		return 1;
	}

	return 0;
}

/**
 * Inmediate function resolution for strpos function
 */
void zephir_fast_strpos(zval *return_value, const zval *haystack, const zval *needle) {

	char *found = NULL;

	if (Z_TYPE_P(haystack) != IS_STRING || Z_TYPE_P(needle) != IS_STRING) {
		ZVAL_NULL(return_value);
		zend_error(E_WARNING, "Invalid arguments supplied for strpos()");
		return;
	}

	if (!Z_STRLEN_P(needle)) {
		ZVAL_NULL(return_value);
		zend_error(E_WARNING, "Empty delimiter");
		return;
	}

	found = php_memnstr(Z_STRVAL_P(haystack), Z_STRVAL_P(needle), Z_STRLEN_P(needle), Z_STRVAL_P(haystack) + Z_STRLEN_P(haystack));

	if (found) {
		ZVAL_LONG(return_value, found-Z_STRVAL_P(haystack));
	} else {
		ZVAL_BOOL(return_value, 0);
	}

}

/**
 * Inmediate function resolution for strpos function
 */
void zephir_fast_strpos_str(zval *return_value, const zval *haystack, char *needle, unsigned int needle_length) {

	char *found = NULL;

	if (Z_TYPE_P(haystack) != IS_STRING) {
		ZVAL_NULL(return_value);
		zend_error(E_WARNING, "Invalid arguments supplied for strpos()");
		return;
	}

	found = php_memnstr(Z_STRVAL_P(haystack), needle, needle_length, Z_STRVAL_P(haystack) + Z_STRLEN_P(haystack));

	if (found) {
		ZVAL_LONG(return_value, found-Z_STRVAL_P(haystack));
	} else {
		ZVAL_BOOL(return_value, 0);
	}

}

/**
 * Inmediate function resolution for stripos function
 */
void zephir_fast_stripos_str(zval *return_value, zval *haystack, char *needle, unsigned int needle_length) {

	char *found = NULL;
	char *needle_dup, *haystack_dup;

	if (Z_TYPE_P(haystack) != IS_STRING) {
		ZVAL_NULL(return_value);
		zend_error(E_WARNING, "Invalid arguments supplied for stripos()");
		return;
	}

	haystack_dup = estrndup(Z_STRVAL_P(haystack), Z_STRLEN_P(haystack));
	php_strtolower(haystack_dup, Z_STRLEN_P(haystack));

	needle_dup = estrndup(needle, needle_length);
	php_strtolower(needle_dup, needle_length);

	found = php_memnstr(haystack_dup, needle, needle_length, haystack_dup + Z_STRLEN_P(haystack));

	efree(haystack_dup);
	efree(needle_dup);

	if (found) {
		ZVAL_LONG(return_value, found-Z_STRVAL_P(haystack));
	} else {
		ZVAL_BOOL(return_value, 0);
	}

}


/**
 * Inmediate function resolution for str_replace function
 */
void zephir_fast_str_replace(zval *return_value, zval *search, zval *replace, zval *subject) {

	zval replace_copy, search_copy;
	int copy_replace = 0, copy_search = 0;

	if (Z_TYPE_P(subject) != IS_STRING) {
		ZVAL_NULL(return_value);
		zend_error(E_WARNING, "Invalid arguments supplied for str_replace()");
		return;
	}

	if (Z_TYPE_P(replace) != IS_STRING) {
		zend_make_printable_zval(replace, &replace_copy, &copy_replace);
		if (copy_replace) {
			replace = &replace_copy;
		}
	}

	if (Z_TYPE_P(search) != IS_STRING) {
		zend_make_printable_zval(search, &search_copy, &copy_search);
		if (copy_search) {
			search = &search_copy;
		}
	}

	Z_TYPE_P(return_value) = IS_STRING;
	if (Z_STRLEN_P(subject) == 0) {
		ZVAL_STRINGL(return_value, "", 0, 1);
		return;
	}

	if (Z_STRLEN_P(search) == 1) {
		php_char_to_str_ex(Z_STRVAL_P(subject),
			Z_STRLEN_P(subject),
			Z_STRVAL_P(search)[0],
			Z_STRVAL_P(replace),
			Z_STRLEN_P(replace),
			return_value,
			1,
			NULL);
	} else {
		if (Z_STRLEN_P(search) > 1) {
			Z_STRVAL_P(return_value) = php_str_to_str_ex(Z_STRVAL_P(subject), Z_STRLEN_P(subject),
				Z_STRVAL_P(search), Z_STRLEN_P(search),
				Z_STRVAL_P(replace), Z_STRLEN_P(replace), &Z_STRLEN_P(return_value), 1, NULL);
		} else {
			MAKE_COPY_ZVAL(&subject, return_value);
		}
	}

	if (copy_replace) {
		zval_dtor(replace);
	}

	if (copy_search) {
		zval_dtor(search);
	}

}

/**
 * Fast call to PHP trim() function
 */
void zephir_fast_trim(zval *return_value, zval *str, int where TSRMLS_DC) {

	zval copy;
	int use_copy = 0;

	if (Z_TYPE_P(str) != IS_STRING) {
		zend_make_printable_zval(str, &copy, &use_copy);
		if (use_copy) {
			str = &copy;
		}
	}

	php_trim(Z_STRVAL_P(str), Z_STRLEN_P(str), NULL, 0, return_value, where TSRMLS_CC);

	if (use_copy) {
		zval_dtor(str);
	}

}

/**
 * Fast call to PHP strip_tags() function
 */
void zephir_fast_strip_tags(zval *return_value, zval *str) {

	zval copy;
	int use_copy = 0;
	char *stripped;
	size_t len;

	if (Z_TYPE_P(str) != IS_STRING) {
		zend_make_printable_zval(str, &copy, &use_copy);
		if (use_copy) {
			str = &copy;
		}
	}

	stripped = estrndup(Z_STRVAL_P(str), Z_STRLEN_P(str));
	len = php_strip_tags(stripped, Z_STRLEN_P(str), NULL, NULL, 0);

	if (use_copy) {
		zval_dtor(str);
	}

	ZVAL_STRINGL(return_value, stripped, len, 0);
}

/**
 * Fast call to PHP strtoupper() function
 */
void zephir_fast_strtoupper(zval *return_value, zval *str) {

	zval copy;
	int use_copy = 0;
	char *lower_str;
	unsigned int length;

	if (Z_TYPE_P(str) != IS_STRING) {
		zend_make_printable_zval(str, &copy, &use_copy);
		if (use_copy) {
			str = &copy;
		}
	}

	length = Z_STRLEN_P(str);
	lower_str = estrndup(Z_STRVAL_P(str), length);
	php_strtoupper(lower_str, length);

	if (use_copy) {
		zval_dtor(str);
	}

	ZVAL_STRINGL(return_value, lower_str, length, 0);
}

/**
 * Checks if a zval string starts with a zval string
 */
int zephir_start_with(const zval *str, const zval *compared, zval *ignore_case){

	int ignore = 1;
	unsigned int i, number;
	char *op1_cursor, *op2_cursor;

	if (Z_TYPE_P(str) != IS_STRING || Z_TYPE_P(compared) != IS_STRING) {
		return 0;
	}

	if (!Z_STRLEN_P(compared) || !Z_STRLEN_P(str)) {
		return 0;
	}

	if (Z_STRLEN_P(compared) > Z_STRLEN_P(str)) {
		return 0;
	}

	if (ignore_case) {
		ignore = zend_is_true(ignore_case);
	}

	if (Z_STRLEN_P(compared) < Z_STRLEN_P(str)) {
		number = Z_STRLEN_P(compared);
	} else {
		number = Z_STRLEN_P(str);
	}

	op1_cursor = Z_STRVAL_P(str);
	op2_cursor = Z_STRVAL_P(compared);
	for (i = 0; i < number; i++) {
		if (ignore) {
			if ((*op1_cursor) != (*op2_cursor)) {
				return 0;
			}
		} else {
			if ((*op1_cursor) != (*op2_cursor)) {
				if (((*op1_cursor) + 32) != (*op2_cursor)) {
					if ((*op1_cursor - 32) != (*op2_cursor)) {
						return 0;
					}
				}
			}
		}
		op1_cursor++;
		op2_cursor++;
	}

	return 1;
}

/**
 * Checks if a zval string starts with a string
 */
int zephir_start_with_str(const zval *str, char *compared, unsigned int compared_length){

	char *op1_cursor, *op2_cursor;
	unsigned int i, number;

	if (Z_TYPE_P(str) != IS_STRING) {
		return 0;
	}

	if (compared_length > Z_STRLEN_P(str)) {
		return 0;
	}

	if (compared_length < Z_STRLEN_P(str)) {
		number = compared_length;
	} else {
		number = Z_STRLEN_P(str);
	}

	op1_cursor = Z_STRVAL_P(str);
	op2_cursor = compared;
	for (i = 0; i < number; i++){
		if ((*op1_cursor) != (*op2_cursor)) {
			return 0;
		}
		op1_cursor++;
		op2_cursor++;
	}

	return 1;
}

/**
 * Checks if a string starts with other  string
 */
int zephir_start_with_str_str(char *str, unsigned int str_length, char *compared, unsigned int compared_length){

	char *op1_cursor, *op2_cursor;
	unsigned int i, number;

	if (compared_length > str_length) {
		return 0;
	}

	if (compared_length < str_length) {
		number = compared_length;
	} else {
		number = str_length;
	}

	op1_cursor = str;
	op2_cursor = compared;
	for (i = 0; i < number; i++){
		if ((*op1_cursor) != (*op2_cursor)) {
			return 0;
		}
		op1_cursor++;
		op2_cursor++;
	}

	return 1;
}

/**
 * Checks if a zval string ends with a zval string
 */
int zephir_end_with(const zval *str, const zval *compared, zval *ignore_case){

	int ignore = 1, number = 0;
	unsigned int i;
	char *op1_cursor, *op2_cursor;

	if (Z_TYPE_P(str) != IS_STRING || Z_TYPE_P(compared) != IS_STRING) {
		return 0;
	}

	if (!Z_STRLEN_P(compared) || !Z_STRLEN_P(str)) {
		return 0;
	}

	if (Z_STRLEN_P(compared) > Z_STRLEN_P(str)) {
		return 0;
	}

	if (ignore_case) {
		ignore = zend_is_true(ignore_case);
	}

	op1_cursor = Z_STRVAL_P(str);
	op2_cursor = Z_STRVAL_P(compared);

	op1_cursor += (Z_STRLEN_P(str) - 1);
	op2_cursor += (Z_STRLEN_P(compared) - 1);

	if (Z_STRLEN_P(compared) < Z_STRLEN_P(str)) {
		number = Z_STRLEN_P(compared);
	} else {
		number = Z_STRLEN_P(str);
	}

	for (i = number; i > 0; i--) {
		if (ignore) {
			if ((*op1_cursor) != (*op2_cursor)) {
				return 0;
			}
		} else {
			if ((*op1_cursor) != (*op2_cursor)) {
				if (((*op1_cursor) + 32) != (*op2_cursor)) {
					if ((*op1_cursor - 32) != (*op2_cursor)) {
						return 0;
					}
				}
			}
		}
		op1_cursor--;
		op2_cursor--;
	}

	return 1;
}

/**
 * Checks if a zval string ends with a *char string
 */
int zephir_end_with_str(const zval *str, char *compared, unsigned int compared_length){

	int number = 0;
	unsigned int i;
	char *op1_cursor, *op2_cursor;

	if (Z_TYPE_P(str) != IS_STRING) {
		return 0;
	}

	if (!compared_length || !Z_STRLEN_P(str)) {
		return 0;
	}

	if (compared_length > Z_STRLEN_P(str)) {
		return 0;
	}

	op1_cursor = Z_STRVAL_P(str);
	op2_cursor = compared;

	op1_cursor += (Z_STRLEN_P(str) - 1);
	op2_cursor += (compared_length - 1);

	if (compared_length < Z_STRLEN_P(str)) {
		number = compared_length;
	} else {
		number = Z_STRLEN_P(str);
	}

	for (i = number; i > 0; i--) {
		if ((*op1_cursor) != (*op2_cursor)) {
			return 0;
		}
		op1_cursor--;
		op2_cursor--;
	}

	return 1;
}

/**
 *
 */
void zephir_random_string(zval *return_value, const zval *type, const zval *length TSRMLS_DC) {

	long i, rand_type, ch;
	smart_str random_str = {0};

	if (Z_TYPE_P(type) != IS_LONG) {
		return;
	}

	if (Z_LVAL_P(type) > PH_RANDOM_NOZERO) {
		return;
	}

	if (Z_TYPE_P(length) != IS_LONG) {
		return;
	}

	/** Generate seed */
	if (!BG(mt_rand_is_seeded)) {
		php_mt_srand(GENERATE_SEED() TSRMLS_CC);
	}

	for (i = 0; i < Z_LVAL_P(length); i++) {

		switch (Z_LVAL_P(type)) {
			case PH_RANDOM_ALNUM:
				rand_type = (long) (php_mt_rand(TSRMLS_C) >> 1);
				RAND_RANGE(rand_type, 0, 3, PHP_MT_RAND_MAX);
				break;
			case PH_RANDOM_ALPHA:
				rand_type = (long) (php_mt_rand(TSRMLS_C) >> 1);
				RAND_RANGE(rand_type, 1, 2, PHP_MT_RAND_MAX);
				break;
			case PH_RANDOM_HEXDEC:
				rand_type = (long) (php_mt_rand(TSRMLS_C) >> 1);
				RAND_RANGE(rand_type, 0, 1, PHP_MT_RAND_MAX);
				break;
			case PH_RANDOM_NUMERIC:
				rand_type = 0;
				break;
			case PH_RANDOM_NOZERO:
				rand_type = 5;
				break;
			default:
				continue;
		}

		switch (rand_type) {
			case 0:
				ch = (long) (php_mt_rand(TSRMLS_C) >> 1);
				RAND_RANGE(ch, '0', '9', PHP_MT_RAND_MAX);
				break;
			case 1:
				ch = (long) (php_mt_rand(TSRMLS_C) >> 1);
				RAND_RANGE(ch, 'a', 'f', PHP_MT_RAND_MAX);
				break;
			case 2:
				ch = (long) (php_mt_rand(TSRMLS_C) >> 1);
				RAND_RANGE(ch, 'a', 'z', PHP_MT_RAND_MAX);
				break;
			case 3:
				ch = (long) (php_mt_rand(TSRMLS_C) >> 1);
				RAND_RANGE(ch, 'A', 'Z', PHP_MT_RAND_MAX);
				break;
			case 5:
				ch = (long) (php_mt_rand(TSRMLS_C) >> 1);
				RAND_RANGE(ch, '1', '9', PHP_MT_RAND_MAX);
				break;
			default:
				continue;
		}

		smart_str_appendc(&random_str, (unsigned int) ch);
	}


	smart_str_0(&random_str);

	if (random_str.len) {
		RETURN_STRINGL(random_str.c, random_str.len, 0);
	} else {
		smart_str_free(&random_str);
		RETURN_EMPTY_STRING();
	}

}

/**
 * Removes slashes at the end of a string
 */
void zephir_remove_extra_slashes(zval *return_value, const zval *str) {

	char *cursor, *removed_str;
	unsigned int i;

	if (Z_TYPE_P(str) != IS_STRING) {
		RETURN_EMPTY_STRING();
		return;
	}

	if (Z_STRLEN_P(str) > 1) {
		cursor = Z_STRVAL_P(str);
		cursor += (Z_STRLEN_P(str) - 1);
		for (i = Z_STRLEN_P(str); i > 0; i--) {
			if ((*cursor) == '/') {
				cursor--;
				continue;
			}
			break;
		}
	} else {
		i = Z_STRLEN_P(str);
	}

	removed_str = emalloc(i + 1);
	memcpy(removed_str, Z_STRVAL_P(str), i);
	removed_str[i] = '\0';

	RETURN_STRINGL(removed_str, i, 0);

}

/**
 * This function is not external in the Zend API so we redeclare it here in the extension
 */
int zephir_spprintf(char **message, int max_len, char *format, ...)
{
    va_list arg;
    int len;

    va_start(arg, format);
    len = vspprintf(message, max_len, format, arg);
    va_end(arg);
    return len;
}

/**
 * Makes a substr like the PHP function. This function doesn't support negative lengths
 */
void zephir_substr(zval *return_value, zval *str, unsigned long from, unsigned long length) {

	if (Z_TYPE_P(str) != IS_STRING) {

		if (Z_TYPE_P(str) == IS_NULL || Z_TYPE_P(str) == IS_BOOL) {
			RETURN_FALSE;
		}

		if (Z_TYPE_P(str) == IS_LONG) {
			RETURN_EMPTY_STRING();
		}

		zend_error(E_WARNING, "Invalid arguments supplied for zephir_substr()");
		RETURN_FALSE;
	}

	if (Z_STRLEN_P(str) < from){
		RETURN_FALSE;
	}

	if (!length || (Z_STRLEN_P(str) < (length + from))) {
		length = Z_STRLEN_P(str) - from;
	}

	if (length <= 0){
		RETURN_EMPTY_STRING();
	}

	RETURN_STRINGL(Z_STRVAL_P(str) + from, length, 1);
}

void zephir_append_printable_array(smart_str *implstr, zval *value TSRMLS_DC) {

	zval         **tmp;
	HashTable      *arr;
	HashPosition   pos;
	unsigned int numelems, i = 0, str_len;

	arr = Z_ARRVAL_P(value);
	numelems = zend_hash_num_elements(arr);

	smart_str_appendc(implstr, '[');

	if (numelems > 0) {
		zend_hash_internal_pointer_reset_ex(arr, &pos);
		while (zend_hash_get_current_data_ex(arr, (void **) &tmp, &pos) == SUCCESS) {

			/**
			 * We don't serialize objects
			 */
			if (Z_TYPE_PP(tmp) == IS_OBJECT) {
				smart_str_appendc(implstr, 'O');
				{
					char stmp[MAX_LENGTH_OF_LONG + 1];
					str_len = slprintf(stmp, sizeof(stmp), "%ld", Z_OBJVAL_PP(tmp).handle);
					smart_str_appendl(implstr, stmp, str_len);
				}
			} else {
				if (Z_TYPE_PP(tmp) == IS_ARRAY) {
					zephir_append_printable_array(implstr, *tmp TSRMLS_CC);
				} else {
					zephir_append_printable_zval(implstr, tmp TSRMLS_CC);
				}
			}

			if (++i != numelems) {
				smart_str_appendc(implstr, ',');
			}

			zend_hash_move_forward_ex(arr, &pos);
		}
	}

	smart_str_appendc(implstr, ']');
}

/**
 * Creates a unique key to be used as index in a hash
 */
void zephir_unique_key(zval *return_value, zval *prefix, zval *value TSRMLS_DC) {

	smart_str implstr = {0};

	if (Z_TYPE_P(prefix) == IS_STRING) {
		smart_str_appendl(&implstr, Z_STRVAL_P(prefix), Z_STRLEN_P(prefix));
	}

	if (Z_TYPE_P(value) == IS_ARRAY) {
		zephir_append_printable_array(&implstr, value TSRMLS_CC);
	} else {
		zephir_append_printable_zval(&implstr, &value TSRMLS_CC);
	}

	smart_str_0(&implstr);

	if (implstr.len) {
		RETURN_STRINGL(implstr.c, implstr.len, 0);
	} else {
		smart_str_free(&implstr);
		RETURN_NULL();
	}

}

/**
 * Returns the PHP_EOL (if the passed parameter is TRUE)
 */
zval *zephir_eol(int eol TSRMLS_DC) {

	zval *local_eol;

	/**
	 * Initialize local var
	 */
    ZEPHIR_INIT_VAR(local_eol);

    /**
     * Check if the eol is true and return PHP_EOL or empty string
     */
    if (eol) {
	    ZVAL_STRING(local_eol, PHP_EOL, 1);
    } else {
        ZVAL_EMPTY_STRING(local_eol);
    }

    return local_eol;
}

/**
 * Base 64 encode
 */
void zephir_base64_encode(zval *return_value, zval *data) {

	zval copy;
	char *encoded;
	int use_copy = 0, length;

	if (Z_TYPE_P(data) != IS_STRING) {
		zend_make_printable_zval(data, &copy, &use_copy);
		if (use_copy) {
			data = &copy;
		}
	}

	encoded = (char *) php_base64_encode((unsigned char *)(Z_STRVAL_P(data)), Z_STRLEN_P(data), &length);

	if (use_copy) {
		zval_dtor(data);
	}

	if (encoded) {
		RETURN_STRINGL(encoded, length, 0);
	} else {
		RETURN_NULL();
	}
}

/**
 * Base 64 decode
 */
void zephir_base64_decode(zval *return_value, zval *data) {

	zval copy;
	char *decoded;
	int use_copy = 0, length;

	if (Z_TYPE_P(data) != IS_STRING) {
		zend_make_printable_zval(data, &copy, &use_copy);
		if (use_copy) {
			data = &copy;
		}
	}

	decoded = (char *) php_base64_decode((unsigned char *)(Z_STRVAL_P(data)), Z_STRLEN_P(data), &length);

	if (use_copy) {
		zval_dtor(data);
	}

	if (decoded) {
		RETURN_STRINGL(decoded, length, 0);
	} else {
		RETURN_NULL();
	}
}

void zephir_md5(zval *return_value, zval *str) {

	PHP_MD5_CTX ctx;
	unsigned char digest[16];
	char hexdigest[33];
	zval copy;
	int use_copy = 0;

	if (Z_TYPE_P(str) != IS_STRING) {
		zend_make_printable_zval(str, &copy, &use_copy);
		if (use_copy) {
			str = &copy;
		}
	}

	PHP_MD5Init(&ctx);
	PHP_MD5Update(&ctx, Z_STRVAL_P(str), Z_STRLEN_P(str));
	PHP_MD5Final(digest, &ctx);

	make_digest(hexdigest, digest);

	ZVAL_STRINGL(return_value, hexdigest, 32, 1);
}

#if HAVE_BUNDLED_PCRE

/**
 * Execute preg-match without function lookup in the PHP userland
 */
void zephir_preg_match(zval *return_value, zval *regex, zval *subject, zval *matches TSRMLS_DC)
{
	zval copy;
	int use_copy = 0;
	pcre_cache_entry *pce;

	if (Z_TYPE_P(regex) != IS_STRING) {
		zend_error(E_WARNING, "Invalid arguments supplied for zephir_preg_match()");
		RETURN_FALSE;
	}

	if (Z_TYPE_P(subject) != IS_STRING) {
		zend_make_printable_zval(subject, &copy, &use_copy);
		if (use_copy) {
			subject = &copy;
		}
	}

	/* Compile regex or get it from cache. */
	if ((pce = pcre_get_compiled_regex_cache(Z_STRVAL_P(regex), Z_STRLEN_P(regex) TSRMLS_CC)) == NULL) {

		if (use_copy) {
			zval_dtor(subject);
		}

		RETURN_FALSE;
	}

	php_pcre_match_impl(pce, Z_STRVAL_P(subject), Z_STRLEN_P(subject), return_value, matches, 0, 0, 0, 0 TSRMLS_CC);

	if (use_copy) {
		zval_dtor(subject);
	}
}

#endif
