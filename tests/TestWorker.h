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

#ifndef NWORKER_TEST_HPP_
#define NWORKER_TEST_HPP_

// C++
#include <string>
#include <sstream>

// CppUnit
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

// Mic11
#include "src/Worker.h"
#include "tests/TestProcessor.h"

class tproc_int1 : public InProcessor<int> {
 public:
  tproc_int1() : InProcessor<int> ("testProcInt") {}
  ~tproc_int1() {}

  void init(std::string s, void *arg) {}
  proc_status_t process();
  void close(proc_status_t) {}
};

class twork_int : public UpdateWorker<int> {
 public:
  twork_int() : UpdateWorker("TestWorkerInt", new tproc_int1) {}
};

class twork_str : public TransformWorker<int,std::string> {
 public:
  twork_str() : TransformWorker("TestWorkerStr", new tproc_str) {}
};

class TestWorker : public CppUnit::TestFixture {

 public:
  void setUp();
  void tearDown();

 protected:
  void Test1Worker();
  void Test2WorkersSameData();
  void Test2WorkersDiffData();
  void TestStop();
  void TestError();

 private:
  twork_int mTestObj_1;
  twork_int mTestObj_2;
  twork_str mTestObj_3;
  int *inPtr, *outPtr;

  CPPUNIT_TEST_SUITE( TestWorker );
  CPPUNIT_TEST( Test1Worker );
  CPPUNIT_TEST( TestStop );
  CPPUNIT_TEST( TestError );
  CPPUNIT_TEST( Test2WorkersSameData );
  CPPUNIT_TEST( Test2WorkersDiffData );
  CPPUNIT_TEST_SUITE_END();
};

#endif
