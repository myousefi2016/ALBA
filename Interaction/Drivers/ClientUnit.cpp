/*=========================================================================

 Program: MAF2
 Module: ClientUnit
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h"

#include "ClientUnit.h"
#include "mmuIdFactory.h"
#include "mafEvent.h"


MAF_ID_IMP(ClientUnit::Socket_ID);
MAF_ID_IMP(ClientUnit::RemoteMessage_ID);

BEGIN_EVENT_TABLE(ClientUnit, wxFrame)
  EVT_SOCKET(Socket_ID, ClientUnit::OnSocketEvent)
END_EVENT_TABLE()

//----------------------------------------------------------------------------
ClientUnit::ClientUnit()
//----------------------------------------------------------------------------
{
  // Create the socket
  m_sock = new wxSocketClient();

  // Setup the event handler and subscribe to most events
  m_sock->SetEventHandler(*this, Socket_ID);
  m_sock->SetNotify(wxSOCKET_CONNECTION_FLAG |
                    wxSOCKET_INPUT_FLAG |
                    wxSOCKET_LOST_FLAG);
  m_sock->Notify(true);

  m_busy = false;
}
//----------------------------------------------------------------------------
ClientUnit::~ClientUnit() 
//----------------------------------------------------------------------------
{
  if(m_sock)
  {
    delete m_sock;
    m_sock = NULL;
  }
}
//----------------------------------------------------------------------------
bool ClientUnit::ConnectClient(wxIPV4address &addr)
//----------------------------------------------------------------------------
{
  mafLogMessage(_("\nTrying to connect (timeout = 10 sec) ...\n"));
  m_sock->Connect(addr, false);
  m_sock->WaitOnConnect(10);

  bool status = m_sock->IsConnected();

  if(status)
    mafLogMessage(_("Succeeded ! Connection established\n"));
  else
  {
    m_sock->Close();
    mafLogMessage(_("Failed ! Unable to connect\n"));
    mafLogMessage(_("Can't connect to the specified host"));
  }

  return status;
}
//----------------------------------------------------------------------------
bool ClientUnit::DisconnectClient()
//----------------------------------------------------------------------------
{
  return m_sock->Close();
}
//----------------------------------------------------------------------------
void ClientUnit::SendMessageToServer(mafString &cmd)
//----------------------------------------------------------------------------
{
  char *msg1;
  size_t len;

  m_busy = true;

  // Here we use ReadMsg and WriteMsg to send messages with
  // a header with size information. Also, the reception is
  // event triggered, so we test input events as well.
  //
  // We need to set no flags here (ReadMsg and WriteMsg are
  // not affected by flags)

  //m_sock->SetFlags(wxSOCKET_WAITALL);
  msg1 = (char *)cmd.GetCStr();
  len  = cmd.Length() + 1;

  m_sock->Write(msg1, len);
  m_sock->Discard();

  m_busy = false;
}
//----------------------------------------------------------------------------
void ClientUnit::OnSocketEvent(wxSocketEvent& event)
//----------------------------------------------------------------------------
{
  wxString s = _("OnSocketEvent: ");

  wxSocketBase *sock = event.GetSocket();

  switch(event.GetSocketEvent())
  {
    case wxSOCKET_INPUT: 
      s.Append(_("wxSOCKET_INPUT\n")); 
      ReadMessageFromServer(sock);
    break;
    case wxSOCKET_LOST       : s.Append(_("wxSOCKET_LOST\n")); break;
    case wxSOCKET_CONNECTION : s.Append(_("wxSOCKET_CONNECTION\n")); break;
    default                  : s.Append(_("Unexpected event !\n")); break;
  }

  //mafLogMessage(s.c_str());
}
//----------------------------------------------------------------------------
void ClientUnit::ReadMessageFromServer(wxSocketBase *sock)
//----------------------------------------------------------------------------
{
#define MAX_MSG_SIZE 256

  m_busy = true;
  char *msg = new char[MAX_MSG_SIZE];
  wxUint32 len;

  // We don't need to set flags because ReadMsg and WriteMsg
  // are not affected by them anyway.

  // Read the message
  len = sock->Read(msg, MAX_MSG_SIZE).LastCount();
  
  if (len > 0) 
  {
    mafString s = msg;
    mafEvent e(this,RemoteMessage_ID,&s);
    InvokeEvent(e);
  }

  delete[] msg;
  m_busy = false;

#undef MAX_MSG_SIZE
}
