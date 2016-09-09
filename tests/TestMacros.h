/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MACROS_TEST_HPP_
#define MACROS_TEST_HPP_

// C++
#include <string>
#include <sstream>

// CppUnit
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

// Mic11
#include "src/Macros.h"
#include "src/Worker.h"
#include "tests/TestWorker.h"

class TestMacros : public CppUnit::TestFixture {

 public:
  void setUp(void);
  void tearDown(void);

 protected:
  void TestStreams(void);

 private:

  CPPUNIT_TEST_SUITE( TestMacros );
  CPPUNIT_TEST( TestStreams );

  CPPUNIT_TEST_SUITE_END();
};

#endif
