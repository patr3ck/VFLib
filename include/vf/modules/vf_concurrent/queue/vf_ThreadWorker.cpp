// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/modules/vf_concurrent/queue/vf_ThreadWorker.h"

ThreadWorker::ThreadWorker (String name)
  : Worker (name)
{
}

END_VF_NAMESPACE