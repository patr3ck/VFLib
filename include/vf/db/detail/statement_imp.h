// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_DETAIL_STATEMENT_IMP_VFHEADER__
#define __VF_DB_DETAIL_STATEMENT_IMP_VFHEADER__

#include "vf/db/detail/into_type.h"
#include "vf/db/detail/use_type.h"

namespace detail {

// statement implementation
class statement_imp : vf::NonCopyable
{
public:
  explicit statement_imp (session& s);
  explicit statement_imp (prepare_temp_type const& prep);
  ~statement_imp ();

  void erase_bindings ();
  void exchange (detail::into_type_ptr const& i);
  void exchange (detail::use_type_ptr const& u);
  void clean_up ();

  void prepare (std::string const& query, bool bRepeatable = true);
  Error execute ();
  bool fetch (Error& error);
  bool got_data () const;

public:
  void do_intos ();
  void pre_use ();
  void do_uses ();
  void post_use ();
  void release_resources ();
  rowid last_insert_rowid ();

public:
  session& m_session;
  sqlite3_stmt* m_stmt;
  std::string m_query;
  bool m_bReady;
  bool m_bGotData;
  bool m_bFirstTime;
  rowid m_last_insert_rowid;

  std::vector<detail::into_type_base*> m_intos;
  std::vector<detail::use_type_base*> m_uses;
};

}

#endif