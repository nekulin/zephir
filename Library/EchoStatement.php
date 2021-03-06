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
 * EchoStatement
 *
 * Produce output according to the type
 */
class EchoStatement
{
	protected $_statement;

	public function __construct($statement)
	{
		$this->_statement = $statement;
	}

	public function compile(CompilationContext $compilationContext)
	{
		$expr = $this->_statement['expr'];
		switch ($expr['type']) {
			case 'variable':

				$variable = $compilationContext->symbolTable->getVariableForRead($expr['value']);

				switch ($variable->getType()) {
					case 'int':
						$compilationContext->codePrinter->output('fprintf(stdout, "%d", ' . $expr['value'] . ');');
						break;
					case 'variable':
						$compilationContext->codePrinter->output('zval_print(' . $expr['value'] . ');');
						break;
					default:
						throw new CompilerException("Unknown type: " . $variable->getType(), $expr);
				}
			default:
				throw new CompilerException("Unknown type: " . $expr['type'], $expr);
		}
	}

}