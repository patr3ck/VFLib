// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THROW_VFHEADER__
#define __VF_THROW_VFHEADER__

#include "vf/vf_Debug.h"

//
// Throw an exception, with the opportunity to a breakpoint
// with the call stack before the throw.
//

template <class Exception>
inline void Throw (const Exception& e)
{
  Debug::breakPoint ();

#if VF_HAVE_BOOST
  boost::throw_exception (e);
#else
  throw e;
#endif
}

#endif
