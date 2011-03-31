// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#if VF_HAVE_JUCE

#include "vf/vf_CatchAny.h"
#include "vf/vf_JuceThread.h"

JuceThread::InterruptionModel::InterruptionModel ()
  : m_state (stateReset)
{
}

bool JuceThread::InterruptionModel::do_wait ()
{
  bool interrupted;

  for (;;)
  {
    const int state = m_state.get ();

    vfassert (state != stateWait);

    if (state == stateInterrupt)
    {
      interrupted = true;

      m_state.set (stateReset);

      break;
    }
    else if (m_state.compareAndSetBool (stateWait, stateReset))
    {
      interrupted = false;

      break;
    }
  }

  return interrupted;
}

bool JuceThread::InterruptionModel::do_timeout ()
{
  bool interrupted;

  if (m_state.compareAndSetBool (stateReset, stateWait))
  {
    interrupted = false;
  }
  else
  {
    vfassert (m_state.get () == stateInterrupt);

    interrupted = true;
  }

  return interrupted;
}

void JuceThread::InterruptionModel::interrupt (JuceThread& thread)
{
  for (;;)
  {
    const int state = m_state.get ();

    // If we are already in the interrupt state, or if
    // we successfully transition from reset to interrupt,
    // then there is no need to signal because the thread
    // will see the new state.
    //
    if (state == stateInterrupt ||
        m_state.compareAndSetBool (stateInterrupt, stateReset))
    {
      break;
    }
    // If we are in the waiting state, then try to change to
    // reset. Whoever is successful will wake the thread up.
    //
    else if (m_state.compareAndSetBool (stateReset, stateWait))
    {
      thread.notify ();
      break;
    }
  }
}

bool JuceThread::InterruptionModel::do_interruptionPoint ()
{
  // How could we possibly be in the wait state and get called?
  vfassert (m_state.get() != stateWait);

  // If we are in the signaled state, switch to reset, and interrupt.
  // Only one thread will "win" this contest.
  const bool interrupted = m_state.compareAndSetBool (stateReset, stateInterrupt);

  return interrupted;
}

//------------------------------------------------------------------------------

bool JuceThread::ExceptionBased::wait (int milliseconds, JuceThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  bool interrupted = do_wait ();

  if (!interrupted)
  {
    interrupted = thread.VF_JUCE::Thread::wait (milliseconds);

    if (!interrupted)
      interrupted = do_timeout ();
  }

  if (interrupted)
    throw Interruption();

  return interrupted;
}

ThreadBase::Interrupted JuceThread::ExceptionBased::interruptionPoint (JuceThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  const bool interrupted = do_interruptionPoint ();

  if (interrupted)
    throw Interruption();

  return ThreadBase::Interrupted (false);
}

//------------------------------------------------------------------------------

bool JuceThread::PollingBased::wait (int milliseconds, JuceThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  bool interrupted = do_wait ();

  if (!interrupted)
  {
    interrupted = thread.VF_JUCE::Thread::wait (milliseconds);

    if (!interrupted)
      interrupted = do_timeout ();
  }
  else
  {
    return true;
  }

  return interrupted;
}

ThreadBase::Interrupted JuceThread::PollingBased::interruptionPoint (JuceThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  const bool interrupted = do_interruptionPoint ();

  return ThreadBase::Interrupted (interrupted);
}

//------------------------------------------------------------------------------

JuceThread::JuceThread (const VF_NAMESPACE::String& name)
  : JuceThreadWrapper (name, this)
{
}

JuceThread::~JuceThread ()
{
  join ();
}

void JuceThread::start (const Function <void (void)>& f)
{
  m_function = f;
  
  VF_JUCE::Thread::startThread ();
}

void JuceThread::join ()
{
  VF_JUCE::Thread::stopThread (-1);
}

JuceThread::id JuceThread::getId () const
{
  return VF_JUCE::Thread::getThreadId ();
}

bool JuceThread::isTheCurrentThread () const
{
  return VF_JUCE::Thread::getCurrentThreadId () ==
         VF_JUCE::Thread::getThreadId ();
}

void JuceThread::setPriority (int priority)
{
  VF_JUCE::Thread::setPriority (priority);
}

void JuceThread::run ()
{
  CatchAny (m_function);
}

//------------------------------------------------------------------------------

namespace CurrentJuceThread {

ThreadBase::Interrupted interruptionPoint ()
{
  bool interrupted;

  VF_JUCE::Thread* thread = VF_JUCE::Thread::getCurrentThread();

  // Can't use interruption points on the message thread
  vfassert (thread != 0);
  
  if (thread)
  {
#if 0
    detail::ThreadBase* threadBase = dynamic_cast <detail::ThreadBase*> (thread);

    // Can only use interruption points from one of our threads
    vfassert (threadBase != 0);

    if (threadBase)
      interrupted = threadBase->interruptionPoint ();
    else
      interrupted = false;
#else
    detail::JuceThreadWrapper* threadWrapper = dynamic_cast <detail::JuceThreadWrapper*> (thread);

    vfassert (threadWrapper != 0);

    if (threadWrapper)
      interrupted = threadWrapper->getThreadBase()->interruptionPoint ();
    else
      interrupted = false;
#endif
  }
  else
  {
    interrupted = false;
  }

  return ThreadBase::Interrupted (interrupted);
}

}

#endif

END_VF_NAMESPACE
