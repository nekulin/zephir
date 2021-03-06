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
 * CompilationContext
 *
 * This class encapsulates important entities requiring during compilation
 */
class CompilationContext
{

	/**
	 * Compiler
	 *
	 * @var Compiler
	 */
	public $compiler;

	/**
	 * Current code printer
	 *
	 * @var CodePrinter
	 */
	public $codePrinter;

	/**
	 * Code printer for the header
	 *
	 * @var CodePrinter
	 */
	public $headerPrinter;

	/**
	 * Current symbol table
	 *
	 * @var SymbolTable
	 */
	public $symbolTable;

	/**
	 * Represents the class currently being compiled
	 *
	 * @var ClassDefinition
	 */
	public $classDefinition;

	/**
	 * Represents the c-headers added to the file
	 *
	 * @var HeadersManager
	 */
	public $headersManager;

	/**
	 * Tells if the the compilation is being made inside a cycle
	 *
	 * @var int
	 */
	public $insideCycle = 0;

}
