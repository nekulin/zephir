<?php

/*
 +----------------------------------------------------------------------+
 | Zephir Language                                                      |
 +----------------------------------------------------------------------+
 | Copyright (c) 2013 Zephir Team                                       |
 +----------------------------------------------------------------------+
 | This source file is subject to version 1.0 of the Zephir license,    |
 | that is bundled with this package in the file LICENSE, and is        |
 | available through the world-wide-web at the following url:           |
 | http://www.zephir-lang.com/license                                   |
 |                                                                      |
 | If you did not receive a copy of the Zephir license and are unable   |
 | to obtain it through the world-wide-web, please send a note to       |
 | license@zephir-lang.com so we can mail you a copy immediately.       |
 +----------------------------------------------------------------------+
*/

/**
 * CompilerFile
 *
 * This class represents every file compiled in a project compilation
 */
class CompilerFile
{

	protected $_namespace;

	protected $_className;

	protected $_filePath;

	protected $_ir;

	protected $_compiledFile;

	/**
	 *
	 */
	public function __construct($className, $filePath)
	{
		$this->_className = $className;
		$this->_filePath = $filePath;
		$this->_compiledFilePath = preg_replace('/\.zep$/', '', $className);
		$this->_filesCompiled = array();
	}

	public function getClassDefinition()
	{
		return $this->_classDefinition;
	}

	/**
	 * Compiles the file generating a JSON
	 */
	public function genIR()
	{
		$compilePath = str_replace(DIRECTORY_SEPARATOR, '.', realpath($this->_filePath)) . ".js";
		system("./xx " . realpath($this->_filePath) . " > .temp/" . $compilePath);
		return json_decode(file_get_contents(".temp/" . $compilePath), true);
	}

	public function compileClass(CompilationContext $compilationContext, $namespace, $topStatement)
	{
		$classDefinition = $this->_classDefinition;

		/**
		 * Do the compilation
		 */
		$classDefinition->compile($compilationContext);

		$code  = '' . PHP_EOL;
		$code .= '#ifdef HAVE_CONFIG_H' . PHP_EOL;
		$code .= '#include "config.h"' . PHP_EOL;
		$code .= '#endif' . PHP_EOL;
		$code .= '' . PHP_EOL;

		$code .= '#include "php.h"' . PHP_EOL;
		$code .= '#include "php_test.h"' . PHP_EOL;
		$code .= '#include "test.h"' . PHP_EOL;
		$code .= '' . PHP_EOL;

		$code .= '#include "Zend/zend_operators.h"' . PHP_EOL;
		$code .= '#include "Zend/zend_exceptions.h"' . PHP_EOL;
		$code .= '#include "Zend/zend_interfaces.h"' . PHP_EOL;
		$code .= '' . PHP_EOL;

		$code .= '#include "kernel/main.h"' . PHP_EOL;

		foreach ($compilationContext->headersManager->get() as $header => $one) {
			$code .= '#include "' . $header . '.h"' . PHP_EOL;
		}

		/**
		 * Prepend the required files to the header
		 */
		$compilationContext->codePrinter->preOutput($code);
	}

	/**
	 * Compiles a comment as a top-level statement
	 */
	public function compileComment(CompilationContext $compilationContext, $topStatement)
	{
		$compilationContext->codePrinter->output('/' . $topStatement['value'] . '/');
	}

	/**
	 * Creates the definition
	 */
	public function preCompileClass($namespace, $topStatement)
	{
		$classDefinition = new ClassDefinition($namespace, $topStatement['name']);

		if (isset($topStatement['definition'])) {

			$definition = $topStatement['definition'];

			if (isset($definition['properties'])) {
				foreach ($definition['properties'] as $property) {
					$classDefinition->addProperty(new ClassProperty(
						$property['visibility'],
						$property['name'],
						isset($property['default']) ? $property['default'] : null
					));
				}
			}

			if (isset($definition['methods'])) {
				foreach ($definition['methods'] as $method) {
					$classDefinition->addMethod(new ClassMethod(
						$method['visibility'],
						$method['name'],
						isset($method['parameters']) ? new ClassMethodParameters($method['parameters']) : null,
						isset($method['statements']) ? new StatementsBlock($method['statements']) : null,
						isset($method['docblock']) ? $method['docblock'] : null
					), $method);
				}
			}
		}

		$this->_classDefinition = $classDefinition;
	}

	/**
	 *
	 */
	public function preCompile()
	{


		$ir = $this->genIR();
		if (!is_array($ir)) {
			throw new Exception("Cannot parse file");
		}

		if (isset($ir['type']) && $ir['type'] == 'error') {
			throw new ParseException($ir['message'], $ir);
		}

		/**
		 * Traverse the top level statements looking for the namespace
		 */
		$namespace = null;
		foreach ($ir as $topStatement) {
			switch ($topStatement['type']) {
				case 'namespace':
					if ($namespace !== null) {
						throw new CompilerException("The namespace must be defined just one time", $topStatement);
					}
					$namespace = $topStatement['name'];
					$this->_namespace = $namespace;
					break;
			}
		}

		if (!$namespace) {
			throw new CompilerException("Every file need a namespace", $topStatement);
		}

		$class = false;
		foreach ($ir as $topStatement) {
			switch ($topStatement['type']) {
				case 'class':
					if ($class) {
						throw new CompilerException("More than one class defined in the same file", $topStatement);
					}
					$class = true;
					$this->preCompileClass($namespace, $topStatement);
					break;
			}
		}

		$this->_ir = $ir;
	}

	public function  getCompiledFile()
	{
		return $this->_compiledFile;
	}

	public function compile(Compiler $compiler)
	{

		/**
		 * Compilation context stores common objects required by compilation entities
		 */
		$compilationContext = new CompilationContext;

		/**
		 * Set global compiler in the compilation context
		 */
		$compilationContext->compiler = $compiler;

		/**
		 * Headers manager
		 */
		$headersManager = new HeadersManager();
		$compilationContext->headersManager = $headersManager;

		/**
		 * Main code-printer for the file
		 */
		$codePrinter = new codePrinter();
		$compilationContext->codePrinter = $codePrinter;

		$codePrinter->outputBlankLine();

		$class = false;
		foreach ($this->_ir as $topStatement)
		{

			switch ($topStatement['type']) {
				case 'class':
					if ($class) {
						throw new Exception("More than one class defined in the same file");
					}
					$class = true;
					$this->compileClass($compilationContext, $this->_namespace, $topStatement);
					break;
				case 'comment':
					$this->compileComment($compilationContext, $topStatement);
					break;
			}
		}

		$path = str_replace('\\', DIRECTORY_SEPARATOR, strtolower(preg_replace('#^test\\\\#i', '', $this->_compiledFilePath)));

		if (!is_dir('ext/' . $path)) {
			mkdir('ext/' . $path);
		}

		file_put_contents('ext/' . $path . '.c', $codePrinter->getOutput());
		file_put_contents('ext/' . $path . '.h', $compilationContext->headerPrinter->getOutput());

		//add to file compiled
		$this->_compiledFile = $path . '.c';
	}

}
