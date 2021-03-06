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

#define LOG_GC 0

namespace {

// This is the upper limit on the amount of physical memory an instance of the
// allocator will allow. Going over this limit means that consumers cannot keep
// up with producers, and application logic should be re-examined.
//
// TODO: ENFORCE THIS GLOBALLY? MEASURE IN KILOBYTES AND FORCE KILOBYTE PAGE SIZES
#define HARD_LIMIT 1
const size_t hardLimitMegaBytes = 256;

}

/*

Implementation notes

- There are two pools, the 'hot' pool and the 'cold' pool.

- When a new page is needed we pop from the 'fresh' stack of the hot pool.

- When a page is deallocated it is pushed to the 'garbage' stack of the hot pool.

- Every so often, a garbage collection is performed on a separate thread.
  During collection, fresh and garbage are swapped in the cold pool.
  Then, the hot and cold pools are atomically swapped.

*/
//------------------------------------------------------------------------------

struct PagedFreeStore::Page : Pages::Node, LeakChecked <Page>
{
  explicit Page (PagedFreeStore* const allocator)
    : m_allocator (*allocator)
  {
  }

  PagedFreeStore& getAllocator () const
  {
    return m_allocator;
  }

private:
  PagedFreeStore& m_allocator;
};

inline void* PagedFreeStore::fromPage (Page* const p)
{
  return reinterpret_cast <char*> (p) +
    Memory::sizeAdjustedForAlignment (sizeof (Page));
}

inline PagedFreeStore::Page* PagedFreeStore::toPage (void* const p)
{
  return reinterpret_cast <Page*> (
    (reinterpret_cast <char *> (p) -
      Memory::sizeAdjustedForAlignment (sizeof (Page))));
}

//------------------------------------------------------------------------------

PagedFreeStore::PagedFreeStore (const size_t pageBytes)
  : m_pageBytes (pageBytes)
  , m_pageBytesAvailable (pageBytes - Memory::sizeAdjustedForAlignment (sizeof (Page)))
  , m_newPagesLeft ((hardLimitMegaBytes * 1024 * 1024) / m_pageBytes)
#if LOG_GC
  , m_swaps (0)
#endif
{
  m_hot  = m_pool1;
  m_cold = m_pool2;

  startOncePerSecond ();
}

PagedFreeStore::~PagedFreeStore ()
{
  endOncePerSecond ();

#if LOG_GC
  vfassert (!m_used.isSignaled ());
#endif

  dispose (m_pool1);
  dispose (m_pool2);

#if LOG_GC
  vfassert (!m_total.isSignaled ());
#endif
}

//------------------------------------------------------------------------------

void* PagedFreeStore::allocate ()
{
  Page* page = m_hot->fresh->pop_front ();

  if (!page)
  {
#if HARD_LIMIT
    const bool exhausted = m_newPagesLeft.release ();
    if (exhausted)
      Throw (Error().fail (__FILE__, __LINE__,
        TRANS("the limit of memory allocations was reached")));
#endif

    void* storage = ::malloc (m_pageBytes);
    if (!storage)
      Throw (Error().fail (__FILE__, __LINE__,
        TRANS("a memory allocation failed")));

    page = new (storage) Page (this);

#if LOG_GC
    m_total.addref ();
#endif
  }

#if LOG_GC
  m_used.addref ();
#endif

  return fromPage (page);
}

void PagedFreeStore::deallocate (void* const p)
{
  Page* const page = toPage (p);
  PagedFreeStore& allocator = page->getAllocator ();

  allocator.m_hot->garbage->push_front (page);

#if LOG_GC
  allocator.m_used.release ();
#endif
}

//
// Perform garbage collection.
//
void PagedFreeStore::doOncePerSecond ()
{
  // Physically free one page.
  // This will reduce the working set over time after a spike.
  {
    Page* page = m_cold->garbage->pop_front ();
    if (page)
    {
      page->~Page ();
      ::free (page);
      m_newPagesLeft.addref ();
#ifdef LOG_GC
      m_total.release ();
#endif
    }
  }

  m_cold->fresh->swap (m_cold->garbage);

  // Swap atomically with respect to m_hot
  Pool* temp = m_hot;
  m_hot = m_cold; // atomic
  m_cold = temp;

#if LOG_GC
  String s;
  s << "swap " << String (++m_swaps);
  s << " (" << String (m_used.get ()) << "/"
    << String (m_total.get ()) << " of "
    << String (m_newPagesLeft.get ()) << ")";
  Logger::outputDebugString (s);
#endif
}

void PagedFreeStore::dispose (Pages& pages)
{
  for (;;)
  {
    Page* const page = pages.pop_front ();

    if (page)
    {
      page->~Page ();
      ::free (page);

#if LOG_GC
      m_total.release ();
#endif
    }
    else
    {
      break;
    }
  }
}

void PagedFreeStore::dispose (Pool& pool)
{
  dispose (pool.fresh);
  dispose (pool.garbage);
}
