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

#ifndef INTEGRATION_TEST_HPP_
#define INTEGRATION_TEST_HPP_

// C++
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
extern std::mutex mtx;

// CppUnit
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

// Mic11
#include "src/Worker.h"
#include "src/Macros.h"

class t_input : public InProcessor<int> {
 public:
  t_input(): InProcessor("t_input") {}

  void     init() override {}
  proc_status_t process() override;
  void     close(proc_status_t st) override {}
};

///////////////////////

class t_proc_tr : public InOutProcessor<int, std::string> {
 public:
  t_proc_tr() : InOutProcessor("t_proc_tr") {}
  void     init() override {}
  proc_status_t process() override;
  void     close(proc_status_t st) override {}
};

///////////////////////

class t_output_s : public OutProcessor<std::string> {
 public:
  t_output_s() : OutProcessor("t_output_s") {}
  void     init() override {}
  proc_status_t process() override;
  void     close(proc_status_t st) override {}
};

///////////////////////

class t_proc_up : public InProcessor<int> {
 public:
  t_proc_up() : InProcessor("t_proc_up") {}
  void     init() override {}
  proc_status_t process() override;
  void     close(proc_status_t st) override {}
};

///////////////////////

class t_output_i : public OutProcessor<int> {
 public:
  t_output_i() : OutProcessor("t_output_i") {}

  void     init() override {}
  proc_status_t process() override;
  void     close(proc_status_t st) override {}
};

IMPLEMENT_INPUT(t_input_worker, int, t_input)   // worker name, data type, proce name
IMPLEMENT_UWORKER(t_proc_up_worker, int, t_proc_up)
IMPLEMENT_OUTPUT(t_outputput_i_worker, int, t_output_i)
IMPLEMENT_TWORKER(t_proc_tr_worker, int, std::string, t_proc_tr)
IMPLEMENT_OUTPUT(t_outputput_s_worker, std::string, t_output_s)



class TestIntegration : public CppUnit::TestFixture {
public:
  void setUp();
  void tearDown();

protected:
  void TestUpdateJobs();
  void TestTransformJobs();
  void TestUpWorkerStop();
  void TestTrWorkerError();

private:
  CPPUNIT_TEST_SUITE( TestIntegration );
  CPPUNIT_TEST( TestUpdateJobs );
  CPPUNIT_TEST( TestTransformJobs );
  CPPUNIT_TEST( TestUpWorkerStop );
  CPPUNIT_TEST( TestTrWorkerError );
  CPPUNIT_TEST_SUITE_END();
};

#endif


