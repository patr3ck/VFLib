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

#ifndef VF_LOCKFREEQUEUE_VFHEADER
#define VF_LOCKFREEQUEUE_VFHEADER

#include "../memory/vf_CacheLine.h"
#include "../memory/vf_AtomicPointer.h"
#include "../threads/vf_SpinDelay.h"

struct LockFreeQueueDefaultTag;

/*============================================================================*/
/** 
    Multiple Producer, Single Consumer (MPSC) intrusive FIFO.

    This container uses the same intrusive interface as List. It is wait-free
    for producers and lock-free for consumers. The caller is responsible for
    preventing the ABA problem (http://en.wikipedia.org/wiki/ABA_problem)

    Invariants:

    - Any thread may call push_back() at any time (Multiple Producer).

    - Only one thread may call try_pop_front() at a time (Single Consumer)

    - The queue is signaled if there are one or more elements.

    @param Tag  A type name used to distinguish lists and nodes, for
                putting objects in multiple lists. If this parameter is
                omitted, the default tag is used.

    @ingroup vf_core intrusive
*/

template <class Element, class Tag = LockFreeQueueDefaultTag>
class LockFreeQueue
{
public:
  class Node : Uncopyable
  {
  public:
    Node ()
	{
	}

	explicit Node (Node* next) : m_next (next)
	{
	}

    AtomicPointer <Node> m_next;
  };

public:
  /** Create an empty list. */
  LockFreeQueue ()
    : m_head (&m_null)
    , m_tail (&m_null)
    , m_null (nullptr)
  {
  }

  /** Determine if the list is empty.
  
      This is not thread safe, the caller must synchronize.

      @return true if the list is empty.
  */
  bool empty () const
  {
    return (m_head.get () == m_tail);
  }

  /** Put an element into the list.
  
      This operation is wait-free.

      @return true if the list was previously empty.

  */
  bool push_back (Node* node)
  {
    node->m_next.set (0);

    Node* prev = m_head.exchange (node);

    // (*) If a try_pop_front() happens at this point, it might not see the
    //     element we are pushing. This only happens when the list is empty,
    //     and furthermore it is detectable.

    prev->m_next.set (node);

    return prev == &m_null;
  }

  /** Retrieve an element from the list.

      This operation is lock-free.

      @return The element, or nullptr if the list was empty.
  */
  Element* pop_front ()
  {
    Element* elem;

    // Avoid the SpinDelay ctor if possible
    if (!try_pop_front (&elem))
    {
      SpinDelay delay;
      do
      {
        delay.pause ();
      }
      while (!try_pop_front (&elem));
    }

    return elem;
  }

  /** Attempt to retrieve an element.

      This attempts to pop an element from the front of the list. The return
      value indicates if the operation was successful. An operation is
      successful if there is no contention for the list. On a successful
      operation, an element is returned if the list was non empty, else nullptr
      is returned. On failure, the returned element is undefined.

      This operation is wait-free.

      @param[out] pElem The element that was retrieved, or nullptr if the
                        list was empty.

      @return           true if the list was uncontended.
  */
  bool try_pop_front (Element** pElem)
  {
    Node* tail = m_tail;
    Node* next = tail->m_next.get ();

    if (tail == &m_null)
    {
      if (next == 0)
      {
        // (*) If a push_back() happens at this point,
        //     we might not see the element.

        if (m_head.get() == tail)
        {
          *pElem = nullptr;
          return true; // success, but queue empty
        }
        else
        {
          return false; // failure: a push_back() caused contention
        }
      }

      m_tail = next;
      tail = next;
      next = next->m_next.get ();
    }

    if (next)
    {
      m_tail = next;
      *pElem = static_cast <Element*> (tail);
      return true;
    }

    Node* head = m_head.get ();

    if (tail == head)
    {
      push_back (&m_null);
      next = tail->m_next.get();
      if (next)
      {
        m_tail = next;
        *pElem = static_cast <Element*> (tail);
        return true;
      }
    }

    // (*) If a push_back() happens at this point,
    //     we might not see the element.

    if (head == m_tail)
    {
      *pElem = nullptr;
      return true; // success, but queue empty
    }
    else
    {
      return false; // failure: a push_back() caused contention
    }
  }

private:
  // Elements are pushed on to the head and popped from the tail.
  AtomicPointer <Node> m_head;
  Node* m_tail;
  Node m_null;
};

/*============================================================================*/
/** Default tag for LockFreeQueue

    @ingroup vf_core intrusive
*/
struct LockFreeQueueDefaultTag { };

#endif
