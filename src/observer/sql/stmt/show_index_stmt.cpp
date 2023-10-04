#include "sql/stmt/show_index_stmt.h"
#include "event/sql_debug.h"

RC ShowIndexStmt::create(Db *db, const ShowIndexSqlNode & show_index, Stmt *&stmt){
    stmt = new ShowIndexStmt(show_index.relation_name);
    sql_debug("show index statement: table name %s",
            show_index.relation_name.c_str());
  return RC::SUCCESS;
}