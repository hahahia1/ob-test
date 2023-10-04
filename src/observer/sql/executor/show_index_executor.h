#pragma once
#include <memory>

#include "common/log/log.h"
#include "common/rc.h"
#include "event/session_event.h"
#include "event/sql_event.h"
#include "session/session.h"
#include "sql/operator/string_list_physical_operator.h"
#include "sql/stmt/show_index_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

using namespace std;

class SQLStageEvent;

/**
 * @brief 描述表上的索引的执行器
 * @ingroup Executor
 */
class ShowIndexExecutor {
 public:
  ShowIndexExecutor() = default;
  virtual ~ShowIndexExecutor() = default;

  RC execute(SQLStageEvent *sql_event) {
    RC rc = RC::SUCCESS;
    Stmt *stmt = sql_event->stmt();
    SessionEvent *session_event = sql_event->session_event();
    Session *session = session_event->session();
    ASSERT(stmt->type() == StmtType::SHOW_INDEX,
           "show index executor cannot run this command: %d",
           static_cast<int>(stmt->type()));
    ShowIndexStmt *show_index_stmt = static_cast<ShowIndexStmt *>(stmt);
    SqlResult *sql_result = session_event->sql_result();
    const char *table_name = show_index_stmt->table_name().c_str();

    Db *db = session->get_current_db();
    Table *table = db->find_table(table_name);
    if (table != nullptr) {
      // Table | Non_unique | Key_name | Seq_in_index | Column_name
      TupleSchema tuple_schema;
      tuple_schema.append_cell(TupleCellSpec("", "Table", "Table"));
      tuple_schema.append_cell(TupleCellSpec("", "Non_unique", "Non_unique"));
      tuple_schema.append_cell(TupleCellSpec("", "Key_name", "Key_name"));
      tuple_schema.append_cell(
          TupleCellSpec("", "Seq_in_index", "Seq_in_index"));
      tuple_schema.append_cell(TupleCellSpec("", "Column_name", "Column_name"));

      sql_result->set_tuple_schema(tuple_schema);

      auto oper = new StringListPhysicalOperator;
      const TableMeta &table_meta = table->table_meta();
      std::vector<std::vector<std::string>> tmp_str_vec =
          table->get_index_info();
      for (std::vector<std::string> tmp_vec : tmp_str_vec) {
        oper->append(tmp_vec);
      }

      sql_result->set_operator(unique_ptr<PhysicalOperator>(oper));
    } else {
      sql_result->set_return_code(RC::INDEX_NOT_FOUND);
      sql_result->set_state_string("Index not exists");
    }
    return rc;
  }
};