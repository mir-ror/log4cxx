/***************************************************************************
                          relativetimedateformat.h  -  description
                             -------------------
    begin                : dim avr 20 2003
    copyright            : (C) 2003 by Michael CATANZARITI
    email                : mcatan@free.fr
 ***************************************************************************/

/***************************************************************************
 * Copyright (C) The Apache Software Foundation. All rights reserved.      *
 *                                                                         *
 * This software is published under the terms of the Apache Software       *
 * License version 1.1, a copy of which has been included with this        *
 * distribution in the LICENSE.txt file.                                   *
 ***************************************************************************/

#ifndef _LOG4CXX_HELPERS_RELATIVE_TIME_DATE_FORMAT_H
#define _LOG4CXX_HELPERS_RELATIVE_TIME_DATE_FORMAT_H

#include <log4cxx/helpers/dateformat.h>
#include <log4cxx/helpers/system.h>

namespace log4cxx
{
	namespace helpers
	{
		/**
		Formats a date by printing the number of seconds
		elapsed since the start of the application. This is the fastest
		printing DateFormat in the package.
		*/
		class LOG4CXX_EXPORT RelativeTimeDateFormat : public DateFormat
		{
		protected:
			int64_t startTime;
			
		public:
			RelativeTimeDateFormat()
			 : DateFormat(_T(""), _T("")), startTime(System::currentTimeMillis())
			{
			}
			
			virtual void format(ostream& os, int64_t time) const
			{
				os << (time - startTime);
			}
		};
	}; // namespace helpers
}; // namespace log4cxx

#endif // _LOG4CXX_HELPERS_RELATIVE_TIME_DATE_FORMAT_H
