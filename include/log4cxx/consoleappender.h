/***************************************************************************
                          consoleappender.h  -  class ConsoleAppender
                             -------------------
    begin                : mar avr 15 2003
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

#ifndef _LOG4CXX_CONSOLE_APPENDER_H
#define _LOG4CXX_CONSOLE_APPENDER_H

#include <log4cxx/writerappender.h>

namespace log4cxx
{
	class ConsoleAppender;
	typedef helpers::ObjectPtrT<ConsoleAppender> ConsoleAppenderPtr;

	/**
	* ConsoleAppender appends log events to <code>stdout</code> or
	* <code>stderr</code> using a layout specified by the user. The
	* default target is <code>stdout</code>.
	*/
	class LOG4CXX_EXPORT ConsoleAppender : public WriterAppender
	{
	public:
		DECLARE_LOG4CXX_OBJECT(ConsoleAppender)
		BEGIN_LOG4CXX_CAST_MAP()
			LOG4CXX_CAST_ENTRY(ConsoleAppender)
			LOG4CXX_CAST_ENTRY_CHAIN(AppenderSkeleton)
		END_LOG4CXX_CAST_MAP()

		ConsoleAppender();
		ConsoleAppender(const LayoutPtr& layout);
		ConsoleAppender(const LayoutPtr& layout, const String& target);
		~ConsoleAppender();

	/**
	*  This method overrides the parent
	*  WriterAppender#closeWriter implementation to do nothing because
	*  the console stream is not ours to close.
	* */
	protected:
		virtual void closeWriter() {}

	/**
	*  Sets the value of the <b>#target</b> property. Recognized values
	*  are "System.out" and "System.err". Any other value will be
	*  ignored.
	* */
	public:
		void setTarget(const String& value);

	/**
	* Returns the current value of the <b>#target</b> property. The
	* default value of the option is "System.out".
	*
	* See also #setTarget.
	* */
	public:
		const String& getTarget() const;

	protected:
		void targetWarn(const String& val);

	public:
		void activateOptions();
		void setOption(const String& option, const String& value);

	public:
		static String SYSTEM_OUT;
		static String SYSTEM_ERR;

	protected:
		String target;
	};
}; //namespace log4cxx

#endif //_LOG4CXX_CONSOLE_APPENDER_H

