
extern zend_class_entry *test_oo_ooconstructparams_ce;

ZEPHIR_INIT_CLASS(Test_Oo_OoConstructParams);

PHP_METHOD(Test_Oo_OoConstructParams, __construct);

ZEND_BEGIN_ARG_INFO_EX(arginfo_test_oo_ooconstructparams___construct, 0, 0, 0)
	ZEND_ARG_INFO(0, a)
	ZEND_ARG_INFO(0, b)
ZEND_END_ARG_INFO()

ZEPHIR_INIT_FUNCS(test_oo_ooconstructparams_method_entry) {
	PHP_ME(Test_Oo_OoConstructParams, __construct, arginfo_test_oo_ooconstructparams___construct, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
