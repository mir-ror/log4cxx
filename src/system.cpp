/***************************************************************************
                          system.cpp  -  class System
                             -------------------
    begin                : 2003/07/11
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

#include <log4cxx/helpers/system.h>

#if defined(HAVE_FTIME)
#include <sys/timeb.h>
#endif

#if defined(HAVE_GETTIMEOFDAY)
#include <sys/time.h>
#endif

#include <time.h>
#include <log4cxx/helpers/properties.h>

using namespace log4cxx;
using namespace log4cxx::helpers;

int64_t System::currentTimeMillis()
{
#if defined(HAVE_GETTIMEOFDAY)
    timeval tp;
    ::gettimeofday(&tp, 0);

    return ((int64_t)tp.tv_sec * 1000) + (int64_t)(tp.tv_usec / 1000);
#elif defined(HAVE_FTIME)
    struct timeb tp;
    ::ftime(&tp);

    return ((int64_t)tp.time * 1000) + (int64_t)tp.millitm;
#else
    return (int64_t)::time(0) * 1000;
#endif
}

String System::getProperty(const String& key)
{
	if (key.empty())
	{
		throw IllegalArgumentException(_T("key is empty"));
	}
	
	USES_CONVERSION;
	char * value = ::getenv(T2A(key.c_str()));
	if (value == 0)
	{
		return String();
	}
	else
	{
		return A2T(value);
	}
}

void System::setProperty(const String& key, const String& value)
{
	if (key.empty())
	{
		throw IllegalArgumentException(_T("key is empty"));
	}
	
#ifdef WIN32 || defined(__hpux)
	String strEnv = key + _T("=") + value;
	USES_CONVERSION;
	::putenv((char *)T2A(strEnv.c_str()));
#else
	/* WARNING ! 
	We don't use putenv with glibc, because it doesn't make
	a copy of the string, but try to keep the pointer
	cf. man 3 putenv.
	*/
	USES_CONVERSION;
	std::string name = T2A(key.c_str());
	std::string val = T2A(value.c_str());
	::setenv(name.c_str(), val.c_str(), 1);
#endif
}

void System::setProperties(const Properties& props)
{
	std::vector<String> propertyNames = props.propertyNames();
	
	for (std::vector<String>::iterator it = propertyNames.begin();
	it != propertyNames.end(); it++)
	{
		const String& propertyName = *it;
		setProperty(propertyName, props.getProperty(propertyName));
	}
}
