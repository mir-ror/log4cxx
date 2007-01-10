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

#ifndef _LOG4CXX_SPI_LOGGING_EVENT_H
#define _LOG4CXX_SPI_LOGGING_EVENT_H

#include <log4cxx/helpers/objectptr.h>
#include <log4cxx/logstring.h>
#include <time.h>
#include <log4cxx/logger.h>
#include <log4cxx/mdc.h>
#include <set>
#include <log4cxx/spi/location/locationinfo.h>


namespace log4cxx
{
        class Logger;
        typedef helpers::ObjectPtrT<Logger> LoggerPtr;

        class Level;

        namespace helpers
        {
                class SocketOutputStream;
                typedef helpers::ObjectPtrT<SocketOutputStream> SocketOutputStreamPtr;

                class SocketInputStream;
                typedef helpers::ObjectPtrT<SocketInputStream> SocketInputStreamPtr;
        }

        namespace spi
        {
                class LoggingEvent;
                /** smart pointer to a LoggingEvent class */
                typedef helpers::ObjectPtrT<LoggingEvent> LoggingEventPtr;

                /**
                The internal representation of logging events. When an affirmative
                decision is made to log then a <code>LoggingEvent</code> instance
                is created. This instance is passed around to the different log4cxx
                components.

                <p>This class is of concern to those wishing to extend log4cxx.
                */
                class LOG4CXX_EXPORT LoggingEvent :
                        public virtual helpers::ObjectImpl
                {
                public:
                        DECLARE_LOG4CXX_OBJECT(LoggingEvent)
                        BEGIN_LOG4CXX_CAST_MAP()
                                LOG4CXX_CAST_ENTRY(LoggingEvent)
                        END_LOG4CXX_CAST_MAP()

                        /** For serialization only
                        */
                        LoggingEvent();

                        /**
                        Instantiate a LoggingEvent from the supplied parameters.

                        <p>Except #timeStamp all the other fields of
                        <code>LoggingEvent</code> are filled when actually needed.
                        <p>
                        @param logger The logger of this event.
                        @param level The level of this event.
                        @param message  The message of this event.
                        @param file The file where this log statement was written.
                        @param line The line where this log statement was written.
                        */
                        LoggingEvent(const LoggerPtr& logger,
                                const LevelPtr& level,   const LogString& message,
                                const log4cxx::spi::LocationInfo& location);

                        ~LoggingEvent();

                        /** Return the #level of this event. */
                        inline const LevelPtr& getLevel() const
                                { return level; }

                        /**  Return the name of the #logger. */
                        const LogString getLoggerName() const;

                        /** Return the #message for this logging event. */
                        inline const LogString& getMessage() const
                                { return message; }

                        /** Return the #message for this logging event. */
                        inline const LogString& getRenderedMessage() const
                                { return message; }

                        /**Returns the time when the application started,
                        in seconds elapsed since 01.01.1970.
                        */
                        static log4cxx_time_t getStartTime();

                        /** Return the #threadName of this event. */
                        inline const LogString& getThreadName() const {
                             return threadName;
                        }

                        /** Return the #timeStamp of this event. */
                        inline log4cxx_time_t getTimeStamp() const
                                { return timeStamp; }

                        /* Return the file where this log statement was written. */
                        inline const log4cxx::spi::LocationInfo& getLocationInformation() const
                                { return locationInfo; }

                        /**
                        * This method returns the NDC for this event. It will return the
                        * correct content even if the event was generated in a different
                        * thread or even on a different machine. The NDC#get method
                        * should <em>never</em> be called directly.  */
                        const LogString& getNDC() const;

                        /** Write this event to a helpers::SocketOutputStream. */
                        void write(helpers::SocketOutputStreamPtr& os) const;

                        void writeLevel(helpers::SocketOutputStreamPtr& os) const;

                        /** Read this event from a helpers::SocketOutputStream. */
                        void read(const helpers::SocketInputStreamPtr& is);

                        void readLevel(const helpers::SocketInputStreamPtr& is);

                        /**
                        * Returns the the context corresponding to the <code>key</code> parameter.
                        * If there is a local MDC copy, possibly because we are in a logging
                        * server or running inside AsyncAppender, then we search for the key in
                        * MDC copy, if a value is found it is returned. Otherwise, if the search
                        * in MDC copy returns an empty result, then the current thread's
                        * <code>MDC</code> is used.
                        *
                        * <p>
                        * Note that <em>both</em> the local MDC copy and the current thread's MDC
                        * are searched.
                        * </p>
                        */
                        LogString getMDC(const LogString& key) const;

                        /**
                        * Returns the set of of the key values in the MDC for the event.
                        * The returned set is unmodifiable by the caller.
                        *
                        * @return Set an unmodifiable set of the MDC keys.
                        * @since 1.3
                        */
                        std::set<LogString> getMDCKeySet() const;

                        /**
                        Obtain a copy of this thread's MDC prior to serialization
                        or asynchronous logging.
                        */
                        void getMDCCopy() const;

                        /**
                        * Return a previously set property. The return value can be null.
                        */
                        LogString getProperty(const LogString& key) const;
                        /**
                        * Returns the set of of the key values in the properties
                        * for the event. The returned set is unmodifiable by the caller.
                        *
                        * @return Set an unmodifiable set of the property keys.
                        */
                        std::set<LogString> getPropertyKeySet() const;

                        /**
                        * Set a string property using a key and a string value.  since 1.3
                        */
                        void setProperty(const LogString& key, const LogString& value);

                public:
                        /**
                        * Fully qualified name of the calling category class.
                        */
                        LogString fqnOfCategoryClass;

                private:
                        /**
                        * The logger of the logging event. This field is not serialized for
                        * performance reasons.
                        *
                        * <p>
                        * It is set by the LoggingEvent constructor or set by a remote
                        * entity after deserialization.
                        **/
                        LoggerPtr logger;

                        /** level of logging event. */
                        LevelPtr level;

                        /** The nested diagnostic context (NDC) of logging event. */
                        LogString ndc;

                        /** The mapped diagnostic context (MDC) of logging event. */
                        MDC::Map mdcCopy;

                        /**
                        * A map of String keys and String values.
                        */
                        std::map<LogString, LogString> * properties;

                        /** Have we tried to do an NDC lookup? If we did, there is no need
                        *  to do it again.  Note that its value is always false when
                        *  serialized. Thus, a receiving SocketNode will never use it's own
                        *  (incorrect) NDC. See also writeObject method.
                        */
                        bool ndcLookupRequired;

                        /**
                        * Have we tried to do an MDC lookup? If we did, there is no need to do it
                        * again.  Note that its value is always false when serialized. See also
                        * the getMDC and getMDCCopy methods.
                        */
                        bool mdcCopyLookupRequired;

                        /** The application supplied message of logging event. */
                        LogString message;


                        /** The number of milliseconds elapsed from 1/1/1970 until logging event
                         was created. */
                        log4cxx_time_t timeStamp;

                        /** The is the location where this log statement was written. */
                        const log4cxx::spi::LocationInfo locationInfo;


                        /** The identifier of thread in which this logging event
                        was generated.
                        */
                       const LogString threadName;

                       //
                       //   prevent copy and assignment
                       //
                       LoggingEvent(const LoggingEvent&);
                       LoggingEvent& operator=(const LoggingEvent&);
                       static const LogString getCurrentThreadName();
                };
        }
}

#endif //_LOG4CXX_SPI_LOGGING_EVENT_H
