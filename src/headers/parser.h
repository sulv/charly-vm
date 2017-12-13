/*
 * This file is part of the Charly Virtual Machine (https://github.com/KCreate/charly-vm)
 *
 * MIT License
 *
 * Copyright (c) 2017 Leonard Schütz
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

#include <functional>

#include "ast.h"
#include "lexer.h"
#include "location.h"
#include "token.h"

#pragma once

namespace Charly::Compiler {

// Contains the result of the parsing step
struct ParseResult {
  std::string filename;
  std::vector<Token> tokens;
  AST::AbstractNode* parse_tree;

  ~ParseResult() {
    delete parse_tree;
  }

  ParseResult(const std::string& f, const std::vector<Token> t, AST::AbstractNode* tree)
      : filename(f), tokens(t), parse_tree(tree) {
  }
};

typedef std::function<void()> ParseFunc;

struct KeywordContext {
  bool break_allowed = true;
  bool continue_allowed = true;
  bool return_allowed = true;
};

class Parser : public Lexer {
public:
  KeywordContext keyword_context;
  ParseResult* parse();

  Parser(SourceFile& file) : Lexer(file) {
  }

public:
  // Utility methods
  Token& advance();
  void advance_to_token(TokenType type);
  void unexpected_token();
  void unexpected_token(TokenType expected);
  void unexpected_token(const std::string& expected_value);
  void illegal_token();
  void illegal_node(AST::AbstractNode* node, const std::string& message);
  void assert_token(TokenType type);
  void expect_token(TokenType type);
  void expect_token(TokenType type, ParseFunc func);
  void skip_token(TokenType type);
  void if_token(TokenType type, ParseFunc func);

  // Parse methods
  //
  // TODO: Unindent methods as they are being implemented
  //       This is just so I can remember which methods still need to be
  //       implemented
  AST::AbstractNode* parse_program();
  AST::AbstractNode* parse_block();
  AST::AbstractNode* parse_statement();
  AST::AbstractNode* parse_class_statement();
  AST::AbstractNode* parse_if_statement();
  AST::AbstractNode* parse_unless_statement();
  AST::AbstractNode* parse_guard_statement();
  AST::AbstractNode* parse_switch_statement();
  AST::AbstractNode* parse_switch_node();
  AST::AbstractNode* parse_while_statement();
  AST::AbstractNode* parse_until_statement();
  AST::AbstractNode* parse_loop_statement();
  AST::AbstractNode* parse_try_statement();
  AST::AbstractNode* parse_expression();
  AST::AbstractNode* parse_assignment();
  AST::AbstractNode* parse_ternary_if();
  AST::AbstractNode* parse_or();
  AST::AbstractNode* parse_and();
  AST::AbstractNode* parse_bitwise_or();
  AST::AbstractNode* parse_bitwise_xor();
  AST::AbstractNode* parse_bitwise_and();
  AST::AbstractNode* parse_equal_not();
  AST::AbstractNode* parse_less_greater();
  AST::AbstractNode* parse_bitwise_shift();
  AST::AbstractNode* parse_add_sub();
  AST::AbstractNode* parse_mul_div();
  AST::AbstractNode* parse_mod();
  AST::AbstractNode* parse_unary();
  AST::AbstractNode* parse_pow();
  AST::AbstractNode* parse_typeof();
  AST::AbstractNode* parse_member_call();
  AST::AbstractNode* parse_literal();
  AST::AbstractNode* parse_array_literal();
  AST::AbstractNode* parse_hash_literal();
  AST::AbstractNode* parse_func_literal();
  AST::AbstractNode* parse_arrowfunc_literal();
  AST::AbstractNode* parse_class_literal();
};

// Thrown on unexpected tokens
struct SyntaxError {
  Location location;
  std::string message;

  SyntaxError(Location l, const std::string& str) : location(l), message(str) {
  }
  SyntaxError(Location l, std::string&& str) : location(l), message(std::move(str)) {
  }
};
};  // namespace Charly::Compiler