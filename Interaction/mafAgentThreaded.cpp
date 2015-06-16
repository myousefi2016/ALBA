/*=========================================================================

 Program: MAF2
 Module: mafAgentThreaded
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafAgentThreaded.h"
#include "mafMutexLock.h"
#include "mmuIdFactory.h"

#include <assert.h>

//------------------------------------------------------------------------------
// Events
//------------------------------------------------------------------------------
MAF_ID_IMP(mafAgentThreaded::AGENT_ASYNC_DISPATCH);

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafAgentThreaded);
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafAgentThreaded::mafAgentThreaded()
//------------------------------------------------------------------------------
{
  m_Threaded      = 1;
  m_ThreadId      = -1;
  m_ThreadData    = NULL;
  m_Threader      = NULL;
  m_ActiveFlag    = 0;

  SetDispatchModeToSelfProcess();

#ifdef _WIN32
  m_MessageSignal = CreateEvent(0, false, false, 0);
#else
  m_Gate = new mafMutexLock;
  m_Gate->Lock();
#endif
}

//------------------------------------------------------------------------------
mafAgentThreaded::~mafAgentThreaded()
//------------------------------------------------------------------------------
{
  Shutdown();
#ifdef _WIN32
  CloseHandle(m_MessageSignal);
#else
  delete m_Gate;
#endif
}

//------------------------------------------------------------------------------
void mafAgentThreaded::InternalShutdown()
//------------------------------------------------------------------------------
{
  StopThread();
}

//------------------------------------------------------------------------------
void mafAgentThreaded::StopThread()
//------------------------------------------------------------------------------
{
  if (m_ThreadId!=-1)
  {
    if (m_ActiveFlag)
    {
      m_ActiveFlag=0;
      this->SignalNewMessage(); //awoke the thread to make it die
    }
    // terminate the thread
    m_Threader->TerminateThread(m_ThreadId);
    m_ThreadId = -1;

    delete m_Threader;
  }
}

//------------------------------------------------------------------------------
int mafAgentThreaded::InternalInitialize()
//------------------------------------------------------------------------------
{
  // Spawns a thread if necessary
  if (m_Threaded)
  {
    if (m_ActiveFlag || m_ThreadId != -1) 
    {
      mafErrorMacro("Dispatcher handler polling thread already started!");
      return -1;
    }

    m_Threader = new mafMultiThreader;
    m_ActiveFlag=1;

    m_ThreadId = m_Threader->SpawnThread(&mafAgentThreaded::UpdateLoop,this);    
  }

  return 0;
}

//------------------------------------------------------------------------------
int mafAgentThreaded::GetActiveFlag()
//------------------------------------------------------------------------------
{
  return m_ActiveFlag;
}

      
//------------------------------------------------------------------------------
void mafAgentThreaded::UpdateLoop(mmuThreadInfoStruct *data)
//------------------------------------------------------------------------------
{
  mafAgentThreaded *self = (mafAgentThreaded *)(data->m_UserData);

  self->m_ThreadData=data;

  // wait for initialization to be completed
  for (;self->GetActiveFlag()&&!self->IsInitialized();)
    mafSleep(100); 

  // This loop is termintated when active flag is set by TerminateThread() or 
  // if InternalUpdate() returns a value !=0
  for (;self->GetActiveFlag()&&!self->InternalUpdate();) ; // active loop

  self->m_ThreadData=NULL;

  self->m_ThreadId=-1;
}

//------------------------------------------------------------------------------
int mafAgentThreaded::InternalUpdate()
//------------------------------------------------------------------------------
{
  this->WaitForNewMessage();

  this->DispatchEvents();
  
  return 0;
}

//------------------------------------------------------------------------------
void mafAgentThreaded::OnEvent(mafEventBase *event)
//------------------------------------------------------------------------------
{
  assert(event);

  if (event->GetId()==AGENT_ASYNC_DISPATCH)
  {
    if (event->GetSender()==this)
    {
      // this is used for the asynchronous InvokeEvent
      this->InvokeEvent((mafEventBase *)event->GetData());
    }
    else
    {
      assert(true); // should not pass from here
      
      // this is used for the asynchronous SendEvent
      // notice we are using the sender field of the AsyncInvokeEvent to store the recipient
      //((mafObserver *)event->GetSender())->OnEvent((mafEventBase *)event->GetData());
    }
  }
  else
  {
    Superclass::OnEvent(event);
  }
}

//------------------------------------------------------------------------------
void mafAgentThreaded::RequestForDispatching()
//------------------------------------------------------------------------------
{
  //if (m_Threaded&&this->Initialized&&this->GetActiveFlag())
  //{
    if (PeekLastEvent()->GetId()==AGENT_ASYNC_DISPATCH)
    {
      Superclass::RequestForDispatching();
    }
    else
    {
      // send my self an event to awoke my thread
      this->SignalNewMessage();
    }
  //}
  //else
  //{
  //  this->DispatchEvents();
  //}

}

//------------------------------------------------------------------------------
void mafAgentThreaded::AsyncInvokeEvent(mafEventBase *event, mafID channel)
//------------------------------------------------------------------------------
{
  mafEventBase *copy_of_event = event->NewInstance();
  copy_of_event->DeepCopy(event);
  copy_of_event->SetChannel(channel);
  PushEvent(AGENT_ASYNC_DISPATCH,this,copy_of_event); // this make a copy of the event
  mafDEL(copy_of_event);
}

//------------------------------------------------------------------------------
void mafAgentThreaded::AsyncSendEvent(mafObserver *target,mafEventBase *event, mafID channel)
//------------------------------------------------------------------------------
{
  if (event&&target&&target!=this)
  {
    // use the sender field to store the recipient.
    mafID old_ch=event->GetChannel();
    event->SetChannel(channel); // set the right channel
    PushEvent(AGENT_ASYNC_DISPATCH,target,event);
    event->SetChannel(old_ch);
  }
}

//----------------------------------------------------------------------------
void mafAgentThreaded::AsyncInvokeEvent(mafID id, mafID channel,void *data)
//----------------------------------------------------------------------------
{
  AsyncInvokeEvent(&mafEventBase(this,id,data,channel),channel);
}

//----------------------------------------------------------------------------
void mafAgentThreaded::WaitForNewMessage()
//------------------------------------------------------------------------------
{
#ifdef _WIN32
  WaitForSingleObject( m_MessageSignal, INFINITE );
#else
  m_Gate->Lock();
#endif
}

//----------------------------------------------------------------------------
void mafAgentThreaded::SignalNewMessage()
//------------------------------------------------------------------------------
{
#ifdef _WIN32
  SetEvent( m_MessageSignal );
#else
  m_Gate->Unlock();
#endif
}
//----------------------------------------------------------------------------
void mafAgentThreaded::AsyncSendEvent(mafObserver *target, void *sender, mafID id, mafID channel,void *data)
//----------------------------------------------------------------------------
{
  AsyncSendEvent(target,&mafEventBase(sender,id,data,channel),channel);
}
