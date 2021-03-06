/*
 * This file is part of the Charly Virtual Machine (https://github.com/KCreate/charly-vm)
 *
 * MIT License
 *
 * Copyright (c) 2017 - 2020 Leonard Schütz
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "compiler.h"
#include "codegenerator.h"
#include "lvar-rewrite.h"
#include "normalizer.h"

namespace Charly::Compilation {

CompilerResult Compiler::compile(AST::AbstractNode* tree) {
  CompilerResult result = {.abstract_syntax_tree = tree};

  // We wrap the entire program inside a function
  // This function gets called by the runtime
  // This function receives a single argument called export, which it
  // returns at the end. This object serves as the return value to an
  // import call

  // Append a return export node to the end of the parsed block
  AST::Block* block = result.abstract_syntax_tree->as<AST::Block>();
  AST::Identifier* ret_id = new AST::Identifier("export");
  AST::Return* ret_stmt = new AST::Return(ret_id->at(block));
  ret_stmt->at(block);
  block->statements.push_back(ret_stmt);

  // Wrap the whole program in a function which handles the exporting interface
  // to other programs
  AST::Function* inclusion_function = new AST::Function("main", {"export"}, {}, block, false);
  inclusion_function->at(block);
  inclusion_function->lvarcount = 0;
  result.abstract_syntax_tree = inclusion_function;

  // Push the function onto the stack and wrap it in a block node
  // The PushStack node prevents the optimizer from removing the function literal
  AST::NodeList* arglist = new AST::NodeList(new AST::Hash());
  result.abstract_syntax_tree = new AST::Call(result.abstract_syntax_tree, arglist);
  result.abstract_syntax_tree->at(block);
  result.abstract_syntax_tree = new AST::Return(result.abstract_syntax_tree);
  result.abstract_syntax_tree->at(block);

  try {
    // Clean up the code a little bit and add or remove some nodes
    Normalizer normalizer(result);
    result.abstract_syntax_tree = normalizer.visit_node(result.abstract_syntax_tree);

    if (result.has_errors) {
      return result;
    }

    // Calculate all offsets of all variables, assignments and declarations
    LVarRewriter lvar_rewriter(result);
    lvar_rewriter.push_local_scope();
    result.abstract_syntax_tree = lvar_rewriter.visit_node(result.abstract_syntax_tree);

    if (result.has_errors) {
      return result;
    }

    CodeGenerator codegenerator(result);
    InstructionBlock* compiled_block = codegenerator.compile(result.abstract_syntax_tree);
    result.instructionblock = compiled_block;
  } catch (CompilerMessage msg) {
    result.has_errors = true;
  }

  return result;
}
}  // namespace Charly::Compilation
