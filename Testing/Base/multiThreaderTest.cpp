/*=========================================================================

 Program: MAF2
 Module: multiThreaderTest
 Authors: Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "multiThreaderTest.h"
#include "mafMultiThreader.h"
#include "mafMutexLock.h"

#include <iostream>
using namespace std;

class test_data
{
public:
  int thread_id;
  int flag;
  mafMutexLock lock;
};

//----------------------------------------------------------------------------
static void ThreadFunction(mmuThreadInfoStruct *data)
//----------------------------------------------------------------------------
{
  assert(data);
  test_data *mydata=(test_data *)(data->m_UserData);
  mydata->flag=1;
  std::cerr<<"Thread "<<mydata->thread_id<<" (ID="<<data->m_ThreadID<<") waiting for gate\n";
  mydata->lock.Lock(); // wait for gate
  mydata->flag=2;
  for (;mydata->flag;) mafSleep(10); // wait for flag reset
  std::cerr<<"Thread "<<mydata->thread_id<<" (ID="<<data->m_ThreadID<<") is dying\n";
}

//----------------------------------------------------------------------------
void multiThreaderTest::MultiThreaderMainTest()
//----------------------------------------------------------------------------
{  
  mafMultiThreader threader;
  test_data data[8];
  int i;
  for (i=0;i<8;i++)
  {
    data[i].thread_id=i;
    data[i].flag=0;
    data[i].lock.Lock();
    threader.SpawnThread(ThreadFunction,&(data[i]));
  }
  mafSleep(2000); // wait 2secs for all threads to complete their work
  std::cerr<<"Gate1\n";
  
  for (i=0;i<8;i++)
  {
    CPPUNIT_ASSERT(data[i].flag==1);
    data[i].lock.Unlock();
  }
  mafSleep(2000); // wait 2secs for all threads to complete their work
  
  for (i=0;i<8;i++)
  {
    CPPUNIT_ASSERT(data[i].flag==2);
  }
  std::cerr<<"Gate2\n";
  for (i=0;i<8;i++)
  {
   data[i].flag=0;
  }
  mafSleep(2000); // wait 2secs for all threads to complete their work

  std::cerr<<"Test completed successfully!"<<std::endl;
}
