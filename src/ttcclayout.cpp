/*
 * Copyright 2003,2004 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <log4cxx/ttcclayout.h>
#include <log4cxx/spi/loggingevent.h>
#include <log4cxx/level.h>
#include <apr-1/apr_pools.h>

using namespace log4cxx;
using namespace log4cxx::spi;

IMPLEMENT_LOG4CXX_OBJECT(TTCCLayout)

TTCCLayout::TTCCLayout()
: DateLayout("RELATIVE"), threadPrinting(true), categoryPrefixing(true),
contextPrinting(true), filePrinting(false)
{
	activateOptions();
}

TTCCLayout::TTCCLayout(const String& dateFormatType)
: DateLayout(dateFormatType), threadPrinting(true), categoryPrefixing(true),
contextPrinting(true), filePrinting(false)
{
	activateOptions();
}

void TTCCLayout::format(ostream& output, const spi::LoggingEventPtr& event) const
{
        std::string date;
        apr_pool_t* p;
        apr_pool_create(&p, NULL);
        formatDate(date, event, p);
        apr_pool_destroy(p);

        output << date;

	if(threadPrinting)
	{
		output << _T("[") << event->getThreadId() << _T("] ");
	}

	output << event->getLevel()->toString() << _T(" ");

	if(categoryPrefixing)
	{
		output << event->getLoggerName() << _T(" ");
	}

	if(contextPrinting)
	{
		String ndc = event->getNDC();

		if(!ndc.empty())
		{
			output << ndc << _T(" ");
		}
	}

	output << _T("- ") << event->getRenderedMessage() << std::endl;
}
