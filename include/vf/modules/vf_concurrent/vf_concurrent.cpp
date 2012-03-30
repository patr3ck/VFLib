// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#include "vf/vf_StandardHeader.h"

#include "vf_concurrent.h"

namespace vf
{
#include "memory/vf_Allocator.cpp"
#include "memory/vf_AllocatorWithoutTls.cpp"
#include "memory/vf_PageAllocator.cpp"

#include "queue/vf_GuiWorker.cpp"
#include "queue/vf_Listeners.cpp"
#include "queue/vf_ThreadWorker.cpp"
#include "queue/vf_Worker.cpp"

#include "threads/vf_ReadWriteMutex.cpp"
}