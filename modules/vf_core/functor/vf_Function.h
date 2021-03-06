/*============================================================================*/
/*
  Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
  See the file GNU_GPL_v2.txt for full licensing terms.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 51
  Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/*============================================================================*/

#ifndef VF_FUNCTION_VFHEADER
#define VF_FUNCTION_VFHEADER

//
// Strong replacement for boost::function:
//
// #1 Bounded memory requirement, avoids the free store.
//
// #2 Always refers to a functor (i.e. is never invalid)
//
// #3 Default value (None) is a function that
//    returns a default object (the result type
//    constructed with a default constructor).
//    

template <typename Signature, int Bytes = 128>
class Function;

//
// nullary function
//

template <typename R, int Bytes>
class Function <R (void), Bytes>
{
public:
  typedef R result_type;
  typedef Function self_type;

  struct None
  {
    typedef R result_type;
    result_type operator() () const
    {
      return result_type();
    } 
  };

  Function()
  {
    constructCopyOf (None ());
  }

  Function (Function const& f)
  {
    f.getCall().constructCopyInto (m_storage);
  }

  template <class Functor>
  Function (Functor const& f)
  {
    constructCopyOf (f);
  }

  ~Function ()
  {
    getCall().~Call();
  }

  Function& operator= (Function const& f)
  {
    getCall().~Call();
    f.getCall().constructCopyInto (m_storage);
    return *this;
  }

  template <class Functor>
  Function& operator= (Functor const& f)
  {
    getCall().~Call();
    constructCopyOf (f);
    return *this;
  }

  result_type operator()()
  {
    return getCall().operator()();
  }

private:
  template <class Functor>
  void constructCopyOf (Functor const& f)
  {
    // If this generates a compile error it means that
    // the functor is too large for the static buffer.
    // Increase the storage template parameter until
    // the error message goes away. This might cause
    // changes throughout the application with other
    // template classes that depend on the size.
    static_jassert (sizeof (StoredCall <Functor>) <= Bytes);
    new (m_storage) StoredCall <Functor> (f);
  }

private:
  struct Call
  {
    virtual ~Call () {}
    virtual void constructCopyInto (void* p) const = 0;
    virtual result_type operator()() = 0;
  };

  template <class Functor>
  struct StoredCall : Call
  {
    explicit StoredCall (Functor const& f) : m_f (f) { }
    StoredCall (const StoredCall& c) : m_f (c.m_f) { }
    void constructCopyInto (void* p) const { new (p) StoredCall (m_f); }
    result_type operator() () { return m_f (); }
  private:
    Functor m_f;
  };

  Call& getCall ()
  {
    return *reinterpret_cast <Call*> (&m_storage[0]);
  }

  Call const& getCall () const
  {
    return *reinterpret_cast <Call const*> (&m_storage[0]);
  }

  char m_storage [Bytes]; // should be enough
};

//------------------------------------------------------------------------------

//
// unary function
//

template <typename R, typename T1, int Bytes>
class Function <R (T1 t1), Bytes>
{
public:
  typedef R result_type;
  typedef Function self_type;

  struct None
  {
    typedef R result_type;
    result_type operator() (T1) const
    {
      return result_type();
    } 
  };

  Function()
  {
    constructCopyOf (None ());
  }

  Function (const Function& f)
  {
    f.getCall().constructCopyInto (m_storage);
  }

  template <class Functor>
  Function (Functor const& f)
  {
    constructCopyOf (f);
  }

  ~Function ()
  {
    getCall().~Call();
  }

  Function& operator= (const Function& f)
  {
    getCall().~Call();
    f.getCall().constructCopyInto (m_storage);
    return *this;
  }

  template <class Functor>
  Function& operator= (Functor const& f)
  {
    getCall().~Call();
    constructCopyOf (f);
    return *this;
  }

  result_type operator() (T1 t1)
  {
    return getCall().operator() (t1);
  }

private:
  template <class Functor>
  void constructCopyOf (Functor const& f)
  {
    // If this generates a compile error it means that
    // the functor is too large for the static buffer.
    // Increase the storage template parameter until
    // the error message goes away. This might cause
    // changes throughout the application with other
    // template classes that depend on the size.
    static_jassert (sizeof (StoredCall <Functor>) <= Bytes);
    new (m_storage) StoredCall <Functor> (f);
  }

private:
  struct Call
  {
    virtual ~Call () {}
    virtual void constructCopyInto (void* p) const = 0;
    virtual result_type operator() (T1 t1) = 0;
  };

  template <class Functor>
  struct StoredCall : Call
  {
    explicit StoredCall (Functor const& f) : m_f (f) { }
    StoredCall (const StoredCall& c) : m_f (c.m_f) { }
    void constructCopyInto (void* p) const { new (p) StoredCall (m_f); }
    result_type operator() (T1 t1) { return m_f (t1); }
  private:
    Functor m_f;
  };

  Call& getCall ()
  {
    return *reinterpret_cast <Call*> (&m_storage[0]);
  }

  Call const& getCall () const
  {
    return *reinterpret_cast <Call const*> (&m_storage[0]);
  }

  char m_storage [Bytes]; // should be enough
};

#endif
