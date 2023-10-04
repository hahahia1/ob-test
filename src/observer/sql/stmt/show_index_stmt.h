#pragma once

#include <string>
#include <vector>

#include "sql/stmt/stmt.h"

class Db;

/**
 * @brief 查询给定表上的所有索引
 * @ingroup Statement
 * @details 
 */
class ShowIndexStmt : public Stmt
{
public:
  ShowIndexStmt(const std::string & table_name):table_name_(table_name) {}
  virtual ~ShowIndexStmt() = default;

  StmtType type() const override { return StmtType::SHOW_INDEX; }
  const std::string &table_name() const { return table_name_; }

  static RC create(Db *db,const ShowIndexSqlNode &show_index, Stmt *&stmt);

  private:
  std::string table_name_;
};