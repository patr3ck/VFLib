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

#ifndef VF_CONCURRENTSTATE_VFHEADER
#define VF_CONCURRENTSTATE_VFHEADER

#include "vf_ReadWriteMutex.h"

/*============================================================================*/
/** 
    Structured access to a shared state.

    This template wraps an object containing members representing state
    information shared between multiple threads of execution, where any thread
    may need to read or write as needed. Synchronized access to the concurrent
    state is enforced at compile time through strongly typed accessor classes.
    This interface design facilitates source code pattern matching to find all
    areas where a concurrent state is accessed.

    There are three types of access:

    - ReadAccess

      Allows read access to the underlying object as const. ReadAccess may be
      granted to one or more threads simultaneously. If one or more threads have 
      ReadAccess, requests to obtain WriteAccess are blocked.

    - WriteAccess

      Allows exclusive read/write access the underlying object. A WriteAccess
      request blocks until all existing ReadAccess and WriteAccess requests are
      released. While a WriteAccess exists, requests for ReadAccess will block.

    - UnlockedAccess

      Allows read access to the underlying object without using the lock. This
      can be helpful when designing concurrent structures through composition.
      It also makes it easier to search for places in code which use unlocked
      access.

    @code

    struct SharedData
    {
      int value1;
      String value2;
    };

    typedef ConcurrentState <SharedData> SharedState;

    SharedState sharedState;

    void readExample ()
    {
      SharedState::ReadAccess state (sharedState);

      print (state->value1);   // read access
      print (state->value2);   // read access

      state->value1 = 42;      // write disallowed: compile error
    }

    void writeExample ()
    {
      SharedState::WriteAccess state (sharedState);

      state->value2 = "Label"; // write access
    }

    @endcode

    Forwarding constructors with up to eight parameters are provided. This lets
    you write constructors into the underlying data object. For example:

    @code

    struct SharedData
    {
      explicit SharedData (int numSlots)
      {
        m_array.reserve (numSlots);
      }

      std::vector <AudioSampleBuffer*> m_array;
    };

    // Construct SharedData with one parameter
    ConcurrentState <SharedData> sharedState (16);

    @endcode

    @param Object The type of object to encapsulate.

    @warning Recursive calls are not supported. It is generally not possible for
             a thread of execution to acquire write access while it already has
    read access. Such an attempt will result in undefined behavior. Calling into
    unknown code while holding a lock can cause deadlock. See
    @ref CallQueue::queue().

    @ingroup vf_concurrent
*/
template <class Object>
class ConcurrentState : Uncopyable
{
public:
  class ReadAccess;
  class WriteAccess;
  class UnlockedAccess;

  /** Create a concurrent state.

      Up to 8 parameters can be specified in the constructor. These parameters
      are forwarded to the corresponding constructor in Object. If no
      constructor in Object matches the parameter list, a compile error is
      generated.
  */
  ConcurrentState () { }

  template <class T1>
  explicit ConcurrentState (T1 t1) : m_obj (t1) { }

  template <class T1, class T2>
  ConcurrentState (T1 t1, T2 t2) : m_obj (t1, t2) { }

  template <class T1, class T2, class T3>
  ConcurrentState (T1 t1, T2 t2, T3 t3) : m_obj (t1, t2, t3) { }

  template <class T1, class T2, class T3, class T4>
  ConcurrentState (T1 t1, T2 t2, T3 t3, T4 t4) : m_obj (t1, t2, t3, t4) { }

  template <class T1, class T2, class T3, class T4, class T5>
  ConcurrentState (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
               : m_obj (t1, t2, t3, t4, t5) { }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  ConcurrentState (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
               : m_obj (t1, t2, t3, t4, t5, t6) { }

  template <class T1, class T2, class T3, class T4,
            class T5, class T6, class T7>
  ConcurrentState (T1 t1, T2 t2, T3 t3, T4 t4,
               T5 t5, T6 t6, T7 t7) : m_obj (t1, t2, t3, t4, t5, t6, t7) { }

  template <class T1, class T2, class T3, class T4,
            class T5, class T6, class T7, class T8>
  ConcurrentState (T1 t1, T2 t2, T3 t3, T4 t4,
               T5 t5, T6 t6, T7 t7, T8 t8)
               : m_obj (t1, t2, t3, t4, t5, t6, t7, t8) { }

private:
  typedef ReadWriteMutex ReadWriteMutexType;

  Object m_obj;
  ReadWriteMutexType m_mutex;
};

/** Unlocked access to a ConcurrentState.

    Use sparingly.
*/
template <class Object>
class ConcurrentState <Object>::UnlockedAccess : Uncopyable
{
public:
  explicit UnlockedAccess (ConcurrentState const& state)
    : m_state (state)
  {
  }

  Object const& getObject () const { return m_state.m_obj; }
  Object const& operator* () const { return getObject(); }
  Object const* operator->() const { return &getObject(); }

private:
  ConcurrentState const& m_state;
};

/** Read only access to a ConcurrentState */
template <class Object>
class ConcurrentState <Object>::ReadAccess : Uncopyable
{
public:
  /** Create a ReadAccess from the specified ConcurrentState */
  explicit ReadAccess (ConcurrentState const volatile& state)
    : m_state (const_cast <ConcurrentState const&> (state))
    , m_lock (m_state.m_mutex)
  {
  }

  /** Obtain a read only reference to Object */
  Object const& getObject () const { return m_state.m_obj; }

  /** Obtain a read only reference to Object */
  Object const& operator* () const { return getObject(); }

  /** Obtain a read only smart pointer to Object */
  Object const* operator->() const { return &getObject(); }

private:
  ConcurrentState const& m_state;
  ReadWriteMutexType::ScopedReadLockType m_lock;
};

/** Read/write access to a ConcurrentState */
template <class Object>
class ConcurrentState <Object>::WriteAccess : Uncopyable
{
public:
  explicit WriteAccess (ConcurrentState& state)
    : m_state (state)
    , m_lock (m_state.m_mutex)
  {
  }

  /** Obtain a read only reference to Object */
  Object const* getObject () const { return m_state.m_obj; }

  /** Obtain a read only reference to Object */
  Object const& operator* () const { return getObject(); }

  /** Obtain a read only smart pointer to Object */
  Object const* operator->() const { return &getObject(); }

  /** Obtain a read/write pointer to Object */
  Object& getObject () { return m_state.m_obj; }

  /** Obtain a read/write reference to Object */
  Object& operator* () { return getObject(); }

  /** Obtain a read/write smart pointer to Object */
  Object* operator->() { return &getObject(); }

private:
  ConcurrentState& m_state;
  ReadWriteMutexType::ScopedWriteLockType m_lock;
};

#endif
