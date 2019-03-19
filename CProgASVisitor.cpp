#include "CProgASVisitor.h"
#include <iostream>
#include <string>

antlrcpp::Any CProgASVisitor::visitProgram(CProgParser::ProgramContext *ctx)
{
  std::cout << ".text" << std::endl;
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitFuncdef(CProgParser::FuncdefContext *ctx)
{
  std::cout << ".global " << ctx->IDENTIFIER()->getText() << std::endl;
  std::cout << ctx->IDENTIFIER()->getText() << ":" << std::endl;
  std::cout << "    pushq %rbp" << std::endl;
  std::cout << "    movq %rsp, %rbp" << std::endl;
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitType_name(CProgParser::Type_nameContext *ctx)
{
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitBlock(CProgParser::BlockContext *ctx)
{
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitStatement(CProgParser::StatementContext *ctx)
{
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitReturn_statement(CProgParser::Return_statementContext *ctx)
{
  std::string tmp_var = visit(ctx->expression());
  std::cout << "    movl " << tos[tmp_var].index << "(%rbp) , %eax" << std::endl;
  std::cout << "    popq %rbp" << std::endl;
  std::cout << "    ret" << std::endl;
}

antlrcpp::Any CProgASVisitor::visitDeclaration(CProgParser::DeclarationContext *ctx)
{
  for(auto id : ctx->IDENTIFIER())
  {
      if(tos.find(id->getText()) == tos.end())
      {
          tos_index -= 4;
          tos[id->getText()] = {SymbolType::INTEGER, tos_index, false, false};
      }
  }
  for(CProgParser::AssignmentContext *as : ctx->assignment())
  {
      std::string lhs_name = as->IDENTIFIER()->getText();
      if(tos.find(lhs_name) == tos.end())
      {
          tos_index -= 4;
          tos[lhs_name] = {SymbolType::INTEGER, tos_index, false, false};
      }
      visit(as);
  }
}

antlrcpp::Any CProgASVisitor::visitAssignment(CProgParser::AssignmentContext *ctx)
{
  std::string lhs_name = ctx->IDENTIFIER()->getText();
  if(tos.find(lhs_name) == tos.end())
  {
      std::cerr << "error: use of undeclared identifier '" << lhs_name << "'" << std::endl;
  }
  std::string rhs_name = visit(ctx->expression());
  if(tos.find(rhs_name) == tos.end())
  {
      std::cerr << "error: use of undeclared identifier '" << rhs_name << "'" << std::endl;
  }
  tos[lhs_name].initialized = tos[rhs_name].initialized;
  if(!tos[rhs_name].initialized)
  {
      std::cerr << "warning: use of uninitialized variable '" << rhs_name << "'" << std::endl;
  }
  std::cout << "    movl " << tos[rhs_name].index << "(%rbp)" << ", %eax" << std::endl;
  std::cout << "    movl " << "%eax, " << tos[lhs_name].index << "(%rbp)" << std::endl;
}

antlrcpp::Any CProgASVisitor::visitExpr_sub(CProgParser::Expr_subContext *ctx)
{
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitExpr_ass(CProgParser::Expr_assContext *ctx)
{
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitExpr_div(CProgParser::Expr_divContext *ctx)
{
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitExpr_add(CProgParser::Expr_addContext *ctx)
{
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitExpr_brk(CProgParser::Expr_brkContext *ctx)
{
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitExpr_lit(CProgParser::Expr_litContext *ctx)
{
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitExpr_mod(CProgParser::Expr_modContext *ctx)
{
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitExpr_mul(CProgParser::Expr_mulContext *ctx)
{
  return visitChildren(ctx);
}

antlrcpp::Any CProgASVisitor::visitExpr_id(CProgParser::Expr_idContext *ctx)
{
  return visitChildren(ctx);
}
