#if defined(HAVE_CONFIG_H)
#include "resiprocate/config.hxx"
#endif

#include "resiprocate/RequestLine.hxx"
#include "resiprocate/ParseUtil.hxx"
#include "resiprocate/UnknownParameter.hxx"
#include "resiprocate/os/Data.hxx"
#include "resiprocate/os/Logger.hxx"
#include "resiprocate/os/ParseBuffer.hxx"

using namespace resip;
using namespace std;

#define RESIPROCATE_SUBSYSTEM Subsystem::SIP

//====================
// RequestLine:
//====================
RequestLine::RequestLine(MethodTypes method, 
                         const Data& sipVersion)
   : mMethod(method),
     mUnknownMethodName(),
     mSipVersion(sipVersion)
{}

RequestLine::RequestLine(HeaderFieldValue* hfv, Headers::Type type) 
   : ParserCategory(hfv, type),
     mMethod(UNKNOWN),
     mUnknownMethodName(MethodNames[UNKNOWN]),
     mSipVersion(Symbols::DefaultSipVersion)
{}
      
RequestLine::RequestLine(const RequestLine& rhs)
   : ParserCategory(rhs),
     mUri(rhs.mUri),
     mMethod(rhs.mMethod),
     mUnknownMethodName(rhs.mUnknownMethodName),
     mSipVersion(rhs.mSipVersion)
{}

RequestLine&
RequestLine::operator=(const RequestLine& rhs)
{
   if (this != &rhs)
   {
      ParserCategory::operator=(rhs);
      mUri = rhs.mUri;
      mMethod = rhs.mMethod;
      mUnknownMethodName = rhs.mUnknownMethodName;
      mSipVersion = rhs.mSipVersion;
   }
   return *this;
}

RequestLine::~RequestLine()
{}

ParserCategory *
RequestLine::clone() const
{
   return new RequestLine(*this);
}

const Uri&
RequestLine::uri() const 
{
   checkParsed(); 
   return mUri;
}

Uri&
RequestLine::uri()
{
   checkParsed(); 
   return mUri;
}

MethodTypes
RequestLine::getMethod() const 
{
   checkParsed(); 
   return mMethod;
}

MethodTypes& 
RequestLine::method() 
{
   checkParsed(); 
   return mMethod;
}

MethodTypes 
RequestLine::method() const 
{
   checkParsed(); 
   return mMethod;
}

Data& 
RequestLine::unknownMethodName()
{
   checkParsed(); 
   return mUnknownMethodName;
}

const Data& 
RequestLine::unknownMethodName() const 
{
   checkParsed(); 
   return mUnknownMethodName;
}

const Data& 
RequestLine::getSipVersion() const 
{
   checkParsed(); 
   return mSipVersion;
}

void 
RequestLine::parse(ParseBuffer& pb)
{
   const char* start;
   start = pb.skipWhitespace();
   pb.skipNonWhitespace();
   mMethod = getMethodType(start, pb.position() - start);
   // for backward compatibility, set the method name even if the method is known
   pb.data(mUnknownMethodName, start);
   pb.skipWhitespace();
   mUri.parse(pb);
   start = pb.skipWhitespace();
   pb.skipNonWhitespace();
   pb.data(mSipVersion, start);
}

ostream&
RequestLine::encodeParsed(ostream& str) const
{
   str << (mMethod != UNKNOWN ? MethodNames[mMethod] : mUnknownMethodName) << Symbols::SPACE;
   mUri.encodeParsed(str);
   str << Symbols::SPACE << mSipVersion;
   return str;
}


/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */