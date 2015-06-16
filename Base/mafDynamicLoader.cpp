/*=========================================================================

 Program: MAF2
 Module: mafDynamicLoader
 Authors: Based on DynamicLoader (www.itk.org), adapted by Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "mafDynamicLoader.h"

// This file is actually 3 different implementations.
// 1. HP machines which uses shl_load
// 2. Power PC MAC which uses GetSharedLibrary
// 3. Windows which uses LoadLibrary
// 4. Most unix systems which use dlopen (default )
// Each part of the ifdef contains a complete implementation for
// the static methods of mafDynamicLoader.  


//----------------------------------------------------------------------------
// 1. Implementation for HPUX  machines
#ifdef __hpux
#define MAFDYNAMICLOADER_DEFINED 1
#include <dl.h>

//----------------------------------------------------------------------------
LibHandle mafDynamicLoader::OpenLibrary(const char* libname )
//----------------------------------------------------------------------------
{
  return shl_load(libname, BIND_DEFERRED | DYNAMIC_PATH, 0L);
}

//----------------------------------------------------------------------------
int mafDynamicLoader::CloseLibrary(LibHandle lib)
//----------------------------------------------------------------------------
{
  return 0;
}

//----------------------------------------------------------------------------
void* mafDynamicLoader::GetSymbolAddress(LibHandle lib, const char* sym)
//----------------------------------------------------------------------------
{ 
  void* addr;
  int status;
  
  status = shl_findsym (&lib, sym, TYPE_PROCEDURE, &addr);
  return (status < 0) ? (void*)0 : addr;
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LibPrefix()
//----------------------------------------------------------------------------
{ 
  return "lib";
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LibExtension()
//----------------------------------------------------------------------------
{
  return ".sl";
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LastError()
//----------------------------------------------------------------------------
{
  return 0;
}

#endif


//----------------------------------------------------------------------------
// 2. Implementation for the Power PC (MAC)
#ifdef __APPLE__
#define MAFDYNAMICLOADER_DEFINED 
#include <mach-o/dyld.h>

//----------------------------------------------------------------------------
LibHandle mafDynamicLoader::OpenLibrary(const char* libname )
//----------------------------------------------------------------------------
{
  NSObjectFileImageReturnCode rc;
  NSObjectFileImage image;
  
  rc = NSCreateObjectFileImageFromFile(libname, &image);
  return NSLinkModule(image, libname, true);
}

//----------------------------------------------------------------------------
int mafDynamicLoader::CloseLibrary(LibHandle lib)
//----------------------------------------------------------------------------
{
  return 0;
}

//----------------------------------------------------------------------------
void* mafDynamicLoader::GetSymbolAddress(LibHandle lib, const char* sym)
//----------------------------------------------------------------------------
{ 
  void *result=0;
  if(NSIsSymbolNameDefined(sym))
    {
    NSSymbol symbol= NSLookupAndBindSymbol(sym);
    if(symbol)
      {
      result = NSAddressOfSymbol(symbol);
      }
    }
  return result;
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LibPrefix()
//----------------------------------------------------------------------------
{ 
  return "";
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LibExtension()
//----------------------------------------------------------------------------
{
  return ".dylib";
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LastError()
//----------------------------------------------------------------------------
{
  return 0;
}

#endif

//----------------------------------------------------------------------------
// 3. Implementation for Windows win32 code
#ifdef _WIN32
#include "mafIncludeWIN32.h"
#define MAFDYNAMICLOADER_DEFINED 1
  
//----------------------------------------------------------------------------
LibHandle mafDynamicLoader::OpenLibrary(const char* libname )
//----------------------------------------------------------------------------
{
  return LoadLibrary(libname);
}

//----------------------------------------------------------------------------
int mafDynamicLoader::CloseLibrary(LibHandle lib)
//----------------------------------------------------------------------------
{
  return (int)FreeLibrary(lib);
}

//----------------------------------------------------------------------------
void* mafDynamicLoader::GetSymbolAddress(LibHandle lib, const char* sym)
//----------------------------------------------------------------------------
{ 
  return (void *)GetProcAddress(lib, sym);
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LibPrefix()
//----------------------------------------------------------------------------
{ 
  return "";
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LibExtension()
//----------------------------------------------------------------------------
{
  return ".dll";
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LastError()
//----------------------------------------------------------------------------
{
  LPVOID lpMsgBuf;

  FormatMessage( 
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL,
    GetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR) &lpMsgBuf,
    0,
    NULL 
    );
  
  static char* str = 0;
  delete [] str;
  str = strcpy(new char[strlen((char*)lpMsgBuf)+1], (char*)lpMsgBuf);
  // Free the buffer.
  LocalFree( lpMsgBuf );
  return str;
}

#endif

// ---------------------------------------------------------------
// 4. Implementation for default UNIX machines.
// if nothing has been defined then use this
#ifndef MAFDYNAMICLOADER_DEFINED
#define MAFDYNAMICLOADER_DEFINED
// Setup for most unix machines
#include <dlfcn.h>
  
//----------------------------------------------------------------------------
LibHandle mafDynamicLoader::OpenLibrary(const char* libname )
//----------------------------------------------------------------------------
{
  return dlopen(libname, RTLD_LAZY);
}

//----------------------------------------------------------------------------
int mafDynamicLoader::CloseLibrary(LibHandle lib)
//----------------------------------------------------------------------------
{
  if (lib)
  {
    return (int)dlclose(lib);
  }
  else
  {
    return 0;
  }
}

//----------------------------------------------------------------------------
void* mafDynamicLoader::GetSymbolAddress(LibHandle lib, const char* sym)
//----------------------------------------------------------------------------
{ 
  return dlsym(lib, sym);
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LibPrefix()
//----------------------------------------------------------------------------
{ 
  return "lib";
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LibExtension()
//----------------------------------------------------------------------------
{
  return ".so";
}

//----------------------------------------------------------------------------
const char* mafDynamicLoader::LastError()
//----------------------------------------------------------------------------
{
  return dlerror(); 
}

#endif
