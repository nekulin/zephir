
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_test.h"
#include "test.h"

#include "Zend/zend_operators.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include "kernel/main.h"
#include "kernel/memory.h"
#include "kernel/object.h"
#include "kernel/array.h"
#include "kernel/operators.h"


/**
 * Test\Router
 *
 * <p>Test\Router is the standard framework router. Routing is the
 * process of taking a URI endpoint (that part of the URI which comes after the base URL) and
 * decomposing it into parameters to determine which module, controller, and
 * action of that controller should receive the request</p>
 *
 *<code>
 *
 *	$router = new Test\Router();
 *
 *	$router->add(
 *		"/documentation/{chapter}/{name}.{type:[a-z]+}",
 *		array(
 *			"controller" => "documentation",
 *			"action"     => "show"
 *		)
 *	);
 *
 *	$router->handle();
 *
 *	echo $router->getControllerName();
 *</code>
 *
 */
ZEPHIR_INIT_CLASS(Test_Router) {

	ZEPHIR_REGISTER_CLASS(Test, Router, router, test_router_method_entry, 0);

	zend_declare_property_null(test_router_ce, SL("_dependencyInjector"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_uriSource"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_namespace"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_module"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_controller"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_action"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_params"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_routes"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_matchedRoute"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_matches"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_bool(test_router_ce, SL("_wasMatched"), 0, ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_defaultNamespace"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_defaultModule"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_defaultController"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_defaultAction"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_defaultParams"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_removeExtraSlashes"), ZEND_ACC_PROTECTED TSRMLS_CC);
	zend_declare_property_null(test_router_ce, SL("_notFoundPaths"), ZEND_ACC_PROTECTED TSRMLS_CC);

	return SUCCESS;

}

/**
 * Test\Router constructor
 *
 * @param boolean defaultRoutes
 */
PHP_METHOD(Test_Router, __construct) {

	zval *routes, *_0, *_1, *_2, *_3, *_4, *_5, *_6;
	zend_bool defaultRoutes;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 1, 0, &defaultRoutes);

	ZEPHIR_INIT_VAR(routes);
	array_init(routes);

	if ((defaultRoutes == true)) {
		//missing comment
		ZEPHIR_INIT_VAR(_0);
		object_init_ex(_0, test_router_route_ce);
		ZEPHIR_INIT_VAR(_1);
		ZVAL_STRING(_1, "#^/([a-zA-Z0-9\\_\\-]+)[/]{0,1}$#", 1);
		ZEPHIR_INIT_VAR(_2);
		array_init(_2);
		add_assoc_long_ex(_2, SS("controller"), 1);
		zephir_call_method_p2_noret(_0, "__construct", _1, _2);

		zephir_array_append(&routes, _0, PH_SEPARATE);

		ZEPHIR_INIT_VAR(_3);
		object_init_ex(_3, test_router_route_ce);
		ZEPHIR_INIT_VAR(_4);
		ZVAL_STRING(_4, "#^/([a-zA-Z0-9\\_\\-]+)/([a-zA-Z0-9\\.\\_]+)(/.*)*$#", 1);
		ZEPHIR_INIT_VAR(_5);
		array_init(_5);
		add_assoc_long_ex(_5, SS("controller"), 1);
		add_assoc_long_ex(_5, SS("action"), 2);
		add_assoc_long_ex(_5, SS("params"), 3);
		zephir_call_method_p2_noret(_3, "__construct", _4, _5);

		zephir_array_append(&routes, _3, PH_SEPARATE);

	}

	ZEPHIR_INIT_VAR(_6);
	array_init(_6);
	zephir_update_property_zval(this_ptr, SL("_params"), _6 TSRMLS_CC);

	zephir_update_property_zval(this_ptr, SL("_routes"), routes TSRMLS_CC);

	ZEPHIR_MM_RESTORE();

}

/**
 * Sets the dependency injector
 *
 * @param Phalcon\DiInterface dependencyInjector
 */
PHP_METHOD(Test_Router, setDI) {

	zval *dependencyInjector;


	zephir_fetch_params(0, 1, 0, &dependencyInjector);

	zephir_update_property_zval(this_ptr, SL("_dependencyInjector"), dependencyInjector TSRMLS_CC);


}

/**
 * Returns the internal dependency injector
 *
 * @return Phalcon\DiInterface
 */
PHP_METHOD(Test_Router, getDI) {

	zval *_0;

	ZEPHIR_MM_GROW();
	ZEPHIR_OBS_VAR(_0);
	zephir_read_property(&_0, this_ptr, SL("_dependencyInjector"), PH_NOISY_CC);
	RETURN_CCTOR(_0);


}

/**
 * Get rewrite info. This info is read from $_GET['_url']. This returns '/' if the rewrite information cannot be read
 *
 * @return string
 */
PHP_METHOD(Test_Router, getRewriteUri) {

	zval *uriSource, *url = NULL, *urlParts, *realUri, *_GET, *_SERVER;

	ZEPHIR_MM_GROW();
	//missing comment
	ZEPHIR_OBS_VAR(uriSource);
	zephir_read_property(&uriSource, this_ptr, SL("_uriSource"), PH_NOISY_CC);

	//missing comment
	if (!(zend_is_true(uriSource))) {
		ZEPHIR_OBS_VAR(url);
		if (zephir_array_isset_string(_GET, SS("_url"))) {
			if (!(zend_is_true(url))) {

				RETURN_CCTOR(url);

			}
		}
	} else {
		//missing comment
		ZEPHIR_OBS_NVAR(url);
		if (zephir_array_isset_string(_SERVER, SS("REQUEST_URI"))) {

			ZEPHIR_INIT_VAR(urlParts);
			ZVAL_NULL(urlParts);

			ZEPHIR_OBS_VAR(realUri);
			zephir_array_fetch_long(&realUri, urlParts, 0, PH_NOISY);

			if (!(zend_is_true(realUri))) {

				RETURN_CCTOR(realUri);

			}
		}
	}

	RETURN_MM_STRING("/", 1);


}

/**
 * Sets the URI source. One of the URI_SOURCE_* constants
 *
 *<code>
 *	$router->setUriSource(Router::URI_SOURCE_SERVER_REQUEST_URI);
 *</code>
 *
 * @param string uriSource
 * @return Test\Router
 */
PHP_METHOD(Test_Router, setUriSource) {

	zval *uriSource;


	zephir_fetch_params(0, 1, 0, &uriSource);

	zephir_update_property_zval(this_ptr, SL("_uriSource"), uriSource TSRMLS_CC);

	RETURN_THISW();


}

/**
 * Set whether router must remove the extra slashes in the handled routes
 *
 * @param boolean remove
 * @return Test\Router
 */
PHP_METHOD(Test_Router, removeExtraSlashes) {

	zval *remove;


	zephir_fetch_params(0, 1, 0, &remove);

	zephir_update_property_zval(this_ptr, SL("_removeExtraSlashes"), remove TSRMLS_CC);

	RETURN_THISW();


}

/**
 * Sets the name of the default namespace
 *
 * @param string namespaceName
 * @return Test\Router
 */
PHP_METHOD(Test_Router, setDefaultNamespace) {

	zval *namespaceName;


	zephir_fetch_params(0, 1, 0, &namespaceName);

	zephir_update_property_zval(this_ptr, SL("_defaultNamespace"), namespaceName TSRMLS_CC);

	RETURN_THISW();


}

/**
 * Sets the name of the default module
 *
 * @param string moduleName
 * @return Test\Router
 */
PHP_METHOD(Test_Router, setDefaultModule) {

	zval *moduleName;


	zephir_fetch_params(0, 1, 0, &moduleName);

	zephir_update_property_zval(this_ptr, SL("_defaultModule"), moduleName TSRMLS_CC);

	RETURN_THISW();


}

/**
 * Sets the default controller name
 *
 * @param string controllerName
 * @return Test\Router
 */
PHP_METHOD(Test_Router, setDefaultController) {

	zval *controllerName;


	zephir_fetch_params(0, 1, 0, &controllerName);

	zephir_update_property_zval(this_ptr, SL("_defaultController"), controllerName TSRMLS_CC);

	RETURN_THISW();


}

/**
 * Sets the default action name
 *
 * @param string actionName
 * @return Test\Router
 */
PHP_METHOD(Test_Router, setDefaultAction) {

	zval *actionName;


	zephir_fetch_params(0, 1, 0, &actionName);

	zephir_update_property_zval(this_ptr, SL("_defaultAction"), actionName TSRMLS_CC);

	RETURN_THISW();


}

/**
 * Sets an array of default paths. If a route is missing a path the router will use the defined here
 * This method must not be used to set a 404 route
 *
 *<code>
 * $router->setDefaults(array(
 *		'module' => 'common',
 *		'action' => 'index'
 * ));
 *</code>
 *
 * @param array defaults
 * @return Test\Router
 */
PHP_METHOD(Test_Router, setDefaults) {

	zval *defaults, *namespaceName, *module, *controller, *action, *params;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 1, 0, &defaults);

	if (Z_TYPE_P(defaults) == IS_ARRAY) {
		ZEPHIR_THROW_EXCEPTION_STR(test_router_exception, "Defaults must be an array");
	}
	//missing comment
	ZEPHIR_OBS_VAR(namespaceName);
	if (zephir_array_isset_string(defaults, SS("namespace"))) {

		zephir_update_property_zval(this_ptr, SL("_defaultNamespace"), namespaceName TSRMLS_CC);

	}
	//missing comment
	ZEPHIR_OBS_VAR(module);
	if (zephir_array_isset_string(defaults, SS("module"))) {

		zephir_update_property_zval(this_ptr, SL("_defaultModule"), module TSRMLS_CC);

	}
	//missing comment
	ZEPHIR_OBS_VAR(controller);
	if (zephir_array_isset_string(defaults, SS("controller"))) {

		zephir_update_property_zval(this_ptr, SL("_defaultController"), controller TSRMLS_CC);

	}
	//missing comment
	ZEPHIR_OBS_VAR(action);
	if (zephir_array_isset_string(defaults, SS("action"))) {

		zephir_update_property_zval(this_ptr, SL("_defaultAction"), action TSRMLS_CC);

	}
	//missing comment
	ZEPHIR_OBS_VAR(params);
	if (zephir_array_isset_string(defaults, SS("params"))) {

		zephir_update_property_zval(this_ptr, SL("_defaultParams"), params TSRMLS_CC);

	}

	RETURN_THIS();


}

/**
 * Handles routing information received from the rewrite engine
 *
 *<code>
 * //Read the info from the rewrite engine
 * $router->handle();
 *
 * //Manually passing an URL
 * $router->handle('/posts/edit/1');
 *</code>
 *
 * @param string uri
 */
PHP_METHOD(Test_Router, handle) {

	zval *uri, *realUri, *request, *currentHostName, *routeFound = NULL, *parts, *params = NULL, *matches, *routes, *reversedRoutes, *notFoundPaths, *vnamespace, *module, *controller, *action, *paramsStr, *strParams, *paramsMerge, *_0, *_1, *_2, *_3, *_4, *_5, *_6, *_7, *_8, *_9, *_10, *_11, *_12;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 1, 0, &uri);

	if (!(zend_is_true(uri))) {
		//missing comment
		ZEPHIR_INIT_VAR(realUri);
		zephir_call_method(realUri, this_ptr, "getrewriteuri");

	} else {

		ZEPHIR_CPY_WRT(realUri, uri);

	}

	ZEPHIR_INIT_VAR(request);
	ZVAL_NULL(request);

	ZEPHIR_INIT_VAR(currentHostName);
	ZVAL_NULL(currentHostName);

	ZEPHIR_INIT_VAR(routeFound);
	ZVAL_BOOL(routeFound, 0);

	ZEPHIR_INIT_VAR(parts);
	array_init(parts);

	ZEPHIR_INIT_VAR(params);
	array_init(params);

	ZEPHIR_INIT_VAR(matches);
	ZVAL_NULL(matches);

	ZEPHIR_INIT_VAR(_0);
	zephir_update_property_zval(this_ptr, SL("_wasMatched"), _0 TSRMLS_CC);

	ZEPHIR_INIT_VAR(_1);
	zephir_update_property_zval(this_ptr, SL("_matchedRoute"), _1 TSRMLS_CC);

	//missing comment
	ZEPHIR_OBS_VAR(routes);
	zephir_read_property(&routes, this_ptr, SL("_routes"), PH_NOISY_CC);

	ZEPHIR_INIT_VAR(reversedRoutes);
	ZVAL_NULL(reversedRoutes);

	//missing for
	//missing comment
	if (zend_is_true(routeFound)) {

		ZEPHIR_INIT_VAR(_2);
		zephir_update_property_zval(this_ptr, SL("_wasMatched"), _2 TSRMLS_CC);

	} else {

		ZEPHIR_INIT_VAR(_3);
		zephir_update_property_zval(this_ptr, SL("_wasMatched"), _3 TSRMLS_CC);

	}
	//missing comment
	if (!(zend_is_true(routeFound))) {
		ZEPHIR_OBS_VAR(notFoundPaths);
		zephir_read_property(&notFoundPaths, this_ptr, SL("_notFoundPaths"), PH_NOISY_CC);

		if (Z_TYPE_P(notFoundPaths) == IS_NULL) {

			ZEPHIR_CPY_WRT(parts, notFoundPaths);

			ZEPHIR_INIT_NVAR(routeFound);
			ZVAL_BOOL(routeFound, 1);

		}
	}
	if (zend_is_true(routeFound)) {
		//missing comment
		ZEPHIR_OBS_VAR(vnamespace);
		if (zephir_array_isset_string(parts, SS("namespace"))) {
			if (!(0)) {

				zephir_update_property_zval(this_ptr, SL("_namespace"), vnamespace TSRMLS_CC);

			}
			//missing unset
		} else {
			ZEPHIR_OBS_VAR(_4);
			zephir_read_property(&_4, this_ptr, SL("_defaultNamespace"), PH_NOISY_CC);

			zephir_update_property_zval(this_ptr, SL("_namespace"), _4 TSRMLS_CC);

		}
		//missing comment
		ZEPHIR_OBS_VAR(module);
		if (zephir_array_isset_string(parts, SS("module"))) {
			if (!(0)) {

				zephir_update_property_zval(this_ptr, SL("_module"), module TSRMLS_CC);

			}
			//missing unset
		} else {
			ZEPHIR_OBS_VAR(_5);
			zephir_read_property(&_5, this_ptr, SL("_defaultModule"), PH_NOISY_CC);

			zephir_update_property_zval(this_ptr, SL("_module"), _5 TSRMLS_CC);

		}
		//missing comment
		ZEPHIR_OBS_VAR(controller);
		if (zephir_array_isset_string(parts, SS("controller"))) {
			if (!(0)) {

				zephir_update_property_zval(this_ptr, SL("_controller"), controller TSRMLS_CC);

			}
			//missing unset
		} else {
			ZEPHIR_OBS_VAR(_6);
			zephir_read_property(&_6, this_ptr, SL("_defaultController"), PH_NOISY_CC);

			zephir_update_property_zval(this_ptr, SL("_controller"), _6 TSRMLS_CC);

		}
		//missing comment
		ZEPHIR_OBS_VAR(action);
		if (zephir_array_isset_string(parts, SS("action"))) {
			if (!(0)) {

				zephir_update_property_zval(this_ptr, SL("_action"), action TSRMLS_CC);

			}
			//missing unset
		} else {
			ZEPHIR_OBS_VAR(_7);
			zephir_read_property(&_7, this_ptr, SL("_defaultAction"), PH_NOISY_CC);

			zephir_update_property_zval(this_ptr, SL("_action"), _7 TSRMLS_CC);

		}
		//missing comment
		ZEPHIR_OBS_VAR(paramsStr);
		if (zephir_array_isset_string(parts, SS("params"))) {

			ZEPHIR_INIT_VAR(strParams);
			ZVAL_NULL(strParams);

			if (zend_is_true(strParams)) {

				ZEPHIR_INIT_NVAR(params);
				ZVAL_NULL(params);

			}
			//missing unset
		}
		if (zephir_fast_count_ev(params TSRMLS_CC)) {

			ZEPHIR_INIT_VAR(paramsMerge);
			ZVAL_NULL(paramsMerge);

		} else {

			ZEPHIR_CPY_WRT(paramsMerge, parts);

		}

		zephir_update_property_zval(this_ptr, SL("_params"), paramsMerge TSRMLS_CC);

	} else {
		//missing comment
		ZEPHIR_OBS_VAR(_8);
		zephir_read_property(&_8, this_ptr, SL("_defaultNamespace"), PH_NOISY_CC);

		zephir_update_property_zval(this_ptr, SL("_namespace"), _8 TSRMLS_CC);

		ZEPHIR_OBS_VAR(_9);
		zephir_read_property(&_9, this_ptr, SL("_defaultModule"), PH_NOISY_CC);

		zephir_update_property_zval(this_ptr, SL("_module"), _9 TSRMLS_CC);

		ZEPHIR_OBS_VAR(_10);
		zephir_read_property(&_10, this_ptr, SL("_defaultController"), PH_NOISY_CC);

		zephir_update_property_zval(this_ptr, SL("_controller"), _10 TSRMLS_CC);

		ZEPHIR_OBS_VAR(_11);
		zephir_read_property(&_11, this_ptr, SL("_defaultAction"), PH_NOISY_CC);

		zephir_update_property_zval(this_ptr, SL("_action"), _11 TSRMLS_CC);

		ZEPHIR_OBS_VAR(_12);
		zephir_read_property(&_12, this_ptr, SL("_defaultParams"), PH_NOISY_CC);

		zephir_update_property_zval(this_ptr, SL("_params"), _12 TSRMLS_CC);

	}
	ZEPHIR_MM_RESTORE();

}

/**
 * Adds a route to the router without any HTTP constraint
 *
 *<code>
 * $router->add('/about', 'About::index');
 *</code>
 *
 * @param string pattern
 * @param string/array paths
 * @param string httpMethods
 * @return Test\Router\Route
 */
PHP_METHOD(Test_Router, add) {

	zval *pattern, *paths, *httpMethods, *route;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 3, 0, &pattern, &paths, &httpMethods);

	//missing comment
	ZEPHIR_INIT_VAR(route);
	object_init_ex(route, test_router_route_ce);
	zephir_call_method_p3_noret(route, "__construct", pattern, paths, httpMethods);

	//missing object-property-append

	RETURN_CCTOR(route);


}

/**
 * Adds a route to the router that only match if the HTTP method is GET
 *
 * @param string pattern
 * @param string/array paths
 * @return Test\Router\Route
 */
PHP_METHOD(Test_Router, addGet) {

	zval *pattern, *paths, *_0, *_1;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 2, 0, &pattern, &paths);

	ZEPHIR_INIT_VAR(_0);
	ZEPHIR_INIT_VAR(_1);
	ZVAL_STRING(_1, "GET", 1);
	zephir_call_method_p3(_0, this_ptr, "add", pattern, paths, _1);
	RETURN_CCTOR(_0);


}

/**
 * Adds a route to the router that only match if the HTTP method is POST
 *
 * @param string pattern
 * @param string/array paths
 * @return Test\Router\Route
 */
PHP_METHOD(Test_Router, addPost) {

	zval *pattern, *paths, *_0, *_1;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 2, 0, &pattern, &paths);

	ZEPHIR_INIT_VAR(_0);
	ZEPHIR_INIT_VAR(_1);
	ZVAL_STRING(_1, "POST", 1);
	zephir_call_method_p3(_0, this_ptr, "add", pattern, paths, _1);
	RETURN_CCTOR(_0);


}

/**
 * Adds a route to the router that only match if the HTTP method is PUT
 *
 * @param string pattern
 * @param string/array paths
 * @return Test\Router\Route
 */
PHP_METHOD(Test_Router, addPut) {

	zval *pattern, *paths, *_0, *_1;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 2, 0, &pattern, &paths);

	ZEPHIR_INIT_VAR(_0);
	ZEPHIR_INIT_VAR(_1);
	ZVAL_STRING(_1, "PUT", 1);
	zephir_call_method_p3(_0, this_ptr, "add", pattern, paths, _1);
	RETURN_CCTOR(_0);


}

/**
 * Adds a route to the router that only match if the HTTP method is PATCH
 *
 * @param string pattern
 * @param string/array paths
 * @return Test\Router\Route
 */
PHP_METHOD(Test_Router, addPatch) {

	zval *pattern, *paths, *_0, *_1;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 2, 0, &pattern, &paths);

	ZEPHIR_INIT_VAR(_0);
	ZEPHIR_INIT_VAR(_1);
	ZVAL_STRING(_1, "PATCH", 1);
	zephir_call_method_p3(_0, this_ptr, "add", pattern, paths, _1);
	RETURN_CCTOR(_0);


}

/**
 * Adds a route to the router that only match if the HTTP method is DELETE
 *
 * @param string pattern
 * @param string/array paths
 * @return Test\Router\Route
 */
PHP_METHOD(Test_Router, addDelete) {

	zval *pattern, *paths, *_0, *_1;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 2, 0, &pattern, &paths);

	ZEPHIR_INIT_VAR(_0);
	ZEPHIR_INIT_VAR(_1);
	ZVAL_STRING(_1, "DELETE", 1);
	zephir_call_method_p3(_0, this_ptr, "add", pattern, paths, _1);
	RETURN_CCTOR(_0);


}

/**
 * Add a route to the router that only match if the HTTP method is OPTIONS
 *
 * @param string pattern
 * @param string/array paths
 * @return Test\Router\Route
 */
PHP_METHOD(Test_Router, addOptions) {

	zval *pattern, *paths, *_0, *_1;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 2, 0, &pattern, &paths);

	ZEPHIR_INIT_VAR(_0);
	ZEPHIR_INIT_VAR(_1);
	ZVAL_STRING(_1, "OPTIONS", 1);
	zephir_call_method_p3(_0, this_ptr, "add", pattern, paths, _1);
	RETURN_CCTOR(_0);


}

/**
 * Adds a route to the router that only match if the HTTP method is HEAD
 *
 * @param string pattern
 * @param string/array paths
 * @return Test\Router\Route
 */
PHP_METHOD(Test_Router, addHead) {

	zval *pattern, *paths, *_0, *_1;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 2, 0, &pattern, &paths);

	ZEPHIR_INIT_VAR(_0);
	ZEPHIR_INIT_VAR(_1);
	ZVAL_STRING(_1, "HEAD", 1);
	zephir_call_method_p3(_0, this_ptr, "add", pattern, paths, _1);
	RETURN_CCTOR(_0);


}

/**
 * Mounts a group of routes in the router
 *
 * @param Test\Router\Group route
 * @return Test\Router
 */
PHP_METHOD(Test_Router, mount) {

	zval *group, *groupRoutes, *beforeMatch, *hostname, *routes, *_0;

	ZEPHIR_MM_GROW();

	zephir_fetch_params(1, 1, 0, &group);

	if (Z_TYPE_P(group) != IS_OBJECT) {
		ZEPHIR_THROW_EXCEPTION_STR(test_router_exception, "The group of routes is not valid");
	}
	ZEPHIR_INIT_VAR(groupRoutes);
	zephir_call_method(groupRoutes, group, "getroutes");

	if (!(zephir_fast_count_ev(groupRoutes TSRMLS_CC))) {
		ZEPHIR_THROW_EXCEPTION_STR(test_router_exception, "The group of routes does not contain any routes");
	}
	//missing comment
	ZEPHIR_INIT_VAR(beforeMatch);
	zephir_call_method(beforeMatch, group, "getbeforematch");

	if (Z_TYPE_P(beforeMatch) == IS_NULL) {
		//missing for
	}
	//missing comment
	ZEPHIR_INIT_VAR(hostname);
	zephir_call_method(hostname, group, "gethostname");

	if (Z_TYPE_P(hostname) == IS_NULL) {
		//missing for
	}
	ZEPHIR_OBS_VAR(routes);
	zephir_read_property(&routes, this_ptr, SL("_routes"), PH_NOISY_CC);

	if (Z_TYPE_P(routes) == IS_ARRAY) {

		ZEPHIR_INIT_VAR(_0);
		zephir_update_property_zval(this_ptr, SL("_routes"), _0 TSRMLS_CC);

	} else {

		zephir_update_property_zval(this_ptr, SL("_routes"), groupRoutes TSRMLS_CC);

	}

	RETURN_THIS();


}

/**
 * Set a group of paths to be returned when none of the defined routes are matched
 *
 * @param array paths
 * @return Test\Router
 */
PHP_METHOD(Test_Router, notFound) {

	zval *paths;


	zephir_fetch_params(0, 1, 0, &paths);

	if (Z_TYPE_P(paths) != IS_ARRAY) {
		if (Z_TYPE_P(paths) != IS_STRING) {
			ZEPHIR_THROW_EXCEPTION_STRW(test_router_exception, "The not-found paths must be an array or string");
		}
	}

	zephir_update_property_zval(this_ptr, SL("_notFoundPaths"), paths TSRMLS_CC);

	RETURN_THISW();


}

/**
 * Removes all the pre-defined routes
 */
PHP_METHOD(Test_Router, clear) {

	zval *_0;

	ZEPHIR_MM_GROW();
	ZEPHIR_INIT_VAR(_0);
	array_init(_0);
	zephir_update_property_zval(this_ptr, SL("_routes"), _0 TSRMLS_CC);

	ZEPHIR_MM_RESTORE();

}

/**
 * Returns the processed namespace name
 *
 * @return string
 */
PHP_METHOD(Test_Router, getNamespaceName) {

	zval *_0;

	ZEPHIR_MM_GROW();
	ZEPHIR_OBS_VAR(_0);
	zephir_read_property(&_0, this_ptr, SL("_namespace"), PH_NOISY_CC);
	RETURN_CCTOR(_0);


}

/**
 * Returns the processed module name
 *
 * @return string
 */
PHP_METHOD(Test_Router, getModuleName) {

	zval *_0;

	ZEPHIR_MM_GROW();
	ZEPHIR_OBS_VAR(_0);
	zephir_read_property(&_0, this_ptr, SL("_module"), PH_NOISY_CC);
	RETURN_CCTOR(_0);


}

/**
 * Returns the processed controller name
 *
 * @return string
 */
PHP_METHOD(Test_Router, getControllerName) {

	zval *_0;

	ZEPHIR_MM_GROW();
	ZEPHIR_OBS_VAR(_0);
	zephir_read_property(&_0, this_ptr, SL("_controller"), PH_NOISY_CC);
	RETURN_CCTOR(_0);


}

/**
 * Returns the processed action name
 *
 * @return string
 */
PHP_METHOD(Test_Router, getActionName) {

	zval *_0;

	ZEPHIR_MM_GROW();
	ZEPHIR_OBS_VAR(_0);
	zephir_read_property(&_0, this_ptr, SL("_action"), PH_NOISY_CC);
	RETURN_CCTOR(_0);


}

/**
 * Returns the processed parameters
 *
 * @return array
 */
PHP_METHOD(Test_Router, getParams) {

	zval *_0;

	ZEPHIR_MM_GROW();
	ZEPHIR_OBS_VAR(_0);
	zephir_read_property(&_0, this_ptr, SL("_params"), PH_NOISY_CC);
	RETURN_CCTOR(_0);


}

/**
 * Returns the route that matchs the handled URI
 *
 * @return Test\Router\Route
 */
PHP_METHOD(Test_Router, getMatchedRoute) {

	zval *_0;

	ZEPHIR_MM_GROW();
	ZEPHIR_OBS_VAR(_0);
	zephir_read_property(&_0, this_ptr, SL("_matchedRoute"), PH_NOISY_CC);
	RETURN_CCTOR(_0);


}

/**
 * Returns the sub expressions in the regular expression matched
 *
 * @return array
 */
PHP_METHOD(Test_Router, getMatches) {

	zval *_0;

	ZEPHIR_MM_GROW();
	ZEPHIR_OBS_VAR(_0);
	zephir_read_property(&_0, this_ptr, SL("_matches"), PH_NOISY_CC);
	RETURN_CCTOR(_0);


}

/**
 * Checks if the router macthes any of the defined routes
 *
 * @return bool
 */
PHP_METHOD(Test_Router, wasMatched) {

	zval *_0;

	ZEPHIR_MM_GROW();
	ZEPHIR_OBS_VAR(_0);
	zephir_read_property(&_0, this_ptr, SL("_wasMatched"), PH_NOISY_CC);
	RETURN_CCTOR(_0);


}

/**
 * Returns all the routes defined in the router
 *
 * @return Test\Router\Route[]
 */
PHP_METHOD(Test_Router, getRoutes) {

	zval *_0;

	ZEPHIR_MM_GROW();
	ZEPHIR_OBS_VAR(_0);
	zephir_read_property(&_0, this_ptr, SL("_routes"), PH_NOISY_CC);
	RETURN_CCTOR(_0);


}

/**
 * Returns a route object by its id
 *
 * @param string id
 * @return Test\Router\Route
 */
PHP_METHOD(Test_Router, getRouteById) {

	zval *id;


	zephir_fetch_params(0, 1, 0, &id);

	//missing for

	RETURN_BOOL(0);


}

/**
 * Returns a route object by its name
 *
 * @param string name
 * @return Test\Router\Route
 */
PHP_METHOD(Test_Router, getRouteByName) {

	zval *name;


	zephir_fetch_params(0, 1, 0, &name);

	//missing for

	RETURN_BOOL(0);


}

