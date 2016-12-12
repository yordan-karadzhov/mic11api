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

#include "TestProcessor.h"

proc_status_t tproc_int::process() {
  **output_ = **input_ + 55;
//   std::cout << "  " << processCount_ << "  " << **input_ << " -> " << **output_ << std::endl;

  return proc_status_t::OK_s;
}


proc_status_t tproc_str::process() {
  std::stringstream ss;
  ss << "output is " << **input_ - 200;
  **output_ = ss.str();
//   std::cout << "  " << processCount_ << "  " << **input_ << " -> " << **output_ << std::endl;
  return proc_status_t::OK_s;
}

void TestProcessor::setUp() {
  inPtr  = new int;
  outPtr_int = new int;
  outPtr_str = new std::string;

  mTestObj_1.setInputObj(&inPtr);
  mTestObj_2.setInputObj(&inPtr);

  mTestObj_1.setOutputObj(&outPtr_int);
  mTestObj_2.setOutputObj(&outPtr_str);

  *inPtr = 3;
}

void TestProcessor::tearDown() {
  delete inPtr;
  delete outPtr_int;
}

void TestProcessor::TestStatusOperators() {
  proc_status_t a, b, c, st;

  a = proc_status_t::OK_s;
  b = proc_status_t::Interrupt_s;
  st = a & b;
  CPPUNIT_ASSERT (st == proc_status_t::Interrupt_s);

  a &= b;
  CPPUNIT_ASSERT (a == proc_status_t::Interrupt_s);

  b = proc_status_t::Error_s;
  st = a & b;
  CPPUNIT_ASSERT (st == proc_status_t::Error_s);
  a &= b;
  CPPUNIT_ASSERT (a == proc_status_t::Error_s);

  b = proc_status_t::FatalError_s;
  st = a & b;
  CPPUNIT_ASSERT (st == proc_status_t::FatalError_s);
  a &= b;
  CPPUNIT_ASSERT (a == proc_status_t::FatalError_s);

  a = proc_status_t::Interrupt_s;
  b = proc_status_t::Error_s;
  c = proc_status_t::OK_s;
  st = a & b & c;
  CPPUNIT_ASSERT (st == proc_status_t::Error_s);
}

void TestProcessor::TestProcessSameData() {
  mTestObj_1.process();
  int testOut = *(mTestObj_1.getOutputPtr());
  CPPUNIT_ASSERT (testOut == 58);
}

void TestProcessor::TestProcessDiffData() {
  mTestObj_2.process();
  std::string testOut = *(mTestObj_2.getOutputPtr());
  CPPUNIT_ASSERT (testOut == "output is -197");
}

void TestProcessor::TestSetInOut() {
  mTestObj_1.setInputObj(&inPtr);
  mTestObj_1.setOutputObj(&inPtr);

  *inPtr = 17;
  int testOut = *(mTestObj_1.getOutputPtr());
  CPPUNIT_ASSERT (testOut == 17);

  *inPtr = 21;
  testOut = **(mTestObj_1.getOutputObj());
  CPPUNIT_ASSERT (testOut == 21);
}