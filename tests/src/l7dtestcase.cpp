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

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

#include <log4cxx/logger.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/propertyresourcebundle.h>

#include "util/compare.h"

#include <vector>
#include <sstream>

#define _T(str) L ## str
typedef std::basic_ostringstream<wchar_t> StringBuffer;

using namespace log4cxx;
using namespace log4cxx::helpers;

class L7dTestCase : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(L7dTestCase);
		CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();

	LoggerPtr root;
	ResourceBundlePtr bundles[3];

public:
	void setUp()
	{
		bundles[0] =
			ResourceBundle::getBundle(LOG4CXX_STR("L7D"), Locale(LOG4CXX_STR("en"), LOG4CXX_STR("US")));
		CPPUNIT_ASSERT(bundles[0] != 0);

		bundles[1] =
			ResourceBundle::getBundle(LOG4CXX_STR("L7D"), Locale(LOG4CXX_STR("fr"), LOG4CXX_STR("FR")));
		CPPUNIT_ASSERT(bundles[1] != 0);

		bundles[2] =
			ResourceBundle::getBundle(LOG4CXX_STR("L7D"), Locale(LOG4CXX_STR("fr"), LOG4CXX_STR("CH")));
		CPPUNIT_ASSERT(bundles[2] != 0);

		root = Logger::getRootLogger();
	}

	void tearDown()
	{
		root->getLoggerRepository()->resetConfiguration();
	}

	void test1()
	{
		PropertyConfigurator::configure(LOG4CXX_FILE("input/l7d1.properties"));

        log4cxx::helpers::Pool pool;
 
		for (int i = 0; i < 3; i++)
		{
			root->setResourceBundle(bundles[i]);

			LOG4CXX_L7DLOG(root, Level::DEBUG, _T("bogus1"));
			LOG4CXX_L7DLOG(root, Level::INFO, _T("test"));
			LOG4CXX_L7DLOG(root, Level::WARN, _T("hello_world"));


            StringBuffer os;
            os << i + 1;
			LOG4CXX_L7DLOG2(root, Level::DEBUG, _T("msg1"), os.str(),
				 _T("log4j"));
			LOG4CXX_L7DLOG2(root, Level::getError(), _T("bogusMsg"), os.str(),
				 _T("log4j"));
			LOG4CXX_L7DLOG2(root, Level::getError(), _T("msg1"), os.str(),
				 _T("log4j"));
			LOG4CXX_L7DLOG(root, Level::INFO, _T("bogus2"));
		}

 		CPPUNIT_ASSERT(Compare::compare(LOG4CXX_FILE("output/temp"),
        LOG4CXX_FILE("witness/l7d.1")));
	}

};

CPPUNIT_TEST_SUITE_REGISTRATION(L7dTestCase);
