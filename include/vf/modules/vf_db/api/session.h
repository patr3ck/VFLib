// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

#ifndef __VF_DB_SESSION_VFHEADER__
#define __VF_DB_SESSION_VFHEADER__

namespace db {

class session
{
public:
  session ();
  ~session ();

  // DEPRECATED!
  // opens the deferred clone
  explicit session (session const& deferredClone);
  Error clone ();

  //! Connect string parameters
  //!
  //!  "timeout={#,infinite}" |
  //!  "mode={read,write,create}" |
  //!  "cache={shared,private}" |
  //!  "threads={single,multi}"      default is serialized
  //!

  Error open (String fileName,
              std::string options = "timeout=infinite|mode=create|threads=multi");
  
  void close ();

  void begin ();
  Error commit ();
  void rollback ();

  bool in_transaction ()
  {
    return m_bInTransaction;
  }

  detail::once_type once (Error& error);
  detail::prepare_type prepare;

  rowid last_insert_rowid();

  std::ostringstream& get_query_stream();

  void log_query(std::string const& query);

  void set_got_data(bool bGotData);
  bool got_data() const;

  sqlite3* get_connection()
  {
    return m_connection;
  }

private:
  Error hard_exec (std::string const& query);

private:
  class Sqlite3;
  ReferenceCountedObjectPtr <Sqlite3> m_instance;

  sqlite3* m_connection;
  String m_fileName;
  std::string m_connectString;
  bool m_bInTransaction;
  std::ostringstream m_query_stream;
  bool m_bGotData;
};

}

#endif
