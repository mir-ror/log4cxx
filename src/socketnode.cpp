/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <log4cxx/logstring.h>
#include <log4cxx/net/socketnode.h>
#include <log4cxx/spi/loggingevent.h>
#include <log4cxx/spi/loggerrepository.h>
#include <log4cxx/helpers/socket.h>
#include <log4cxx/helpers/socketinputstream.h>
#include <log4cxx/level.h>
#include <log4cxx/helpers/loglog.h>

using namespace log4cxx;
using namespace log4cxx::net;
using namespace log4cxx::helpers;
using namespace log4cxx::spi;

IMPLEMENT_LOG4CXX_OBJECT(SocketNode)

SocketNode::SocketNode(helpers::SocketPtr& socket,
   spi::LoggerRepositoryPtr& hierarchy1)
 : is(socket->getInputStream()),
   hierarchy(hierarchy1)
{
}

void SocketNode::run()
{
   LoggingEventPtr event = new LoggingEvent();
   LoggerPtr remoteLogger;

   try
   {
      while(true)
      {
         // read an event from the wire
         event->read(is);

         // get a logger from the hierarchy.
         // The name of the logger is taken to be the
         // name contained in the event.
         if (event->getLoggerName() == LOG4CXX_STR("root"))
         {
            remoteLogger = hierarchy->getRootLogger();
         }
         else
         {
            remoteLogger =
               hierarchy->getLogger(event->getLoggerName());
         }

         // apply the logger-level filter
         if(event->getLevel()->isGreaterOrEqual(
            remoteLogger->getEffectiveLevel()))
         {
                                Pool p;
            // finally log the event as if was generated locally
            remoteLogger->callAppenders(event, p);
         }
      }
   }
   catch(EOFException&)
   {
      LogLog::debug(LOG4CXX_STR("Caught EOFException. Closing connection."));
   }
   catch(SocketException&)
   {
      LogLog::debug(LOG4CXX_STR("Caught SocketException. Closing connection"));
   }
    catch(IOException& e)
    {
      LogLog::debug(LOG4CXX_STR("Caught IOException."), e);
      LogLog::debug(LOG4CXX_STR("Closing connection."));
    }
    catch(Exception& e)
    {
      LogLog::error(LOG4CXX_STR("Unexpected exception. Closing connection."), e);
    }

   try
   {
      is->close();
   }
   catch(SocketException& e)
   {
      LogLog::debug(LOG4CXX_STR("Could not close SocketNode connection: "), e);
   }
}
