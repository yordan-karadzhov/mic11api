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

bool tproc_int::process() {
  **output_ = **input_ + 55;
//   std::cout << "  " << processCount_ << "  " << **input_ << " -> " << **output_ << std::endl;
  return true;
}


bool tproc_str::process() {
  std::stringstream ss;
  ss << "output is " << **input_ - 200;
  **output_ = ss.str();
//   std::cout << "  " << processCount_ << "  " << **input_ << " -> " << **output_ << std::endl;
  return true;
}

void TestNProcessor::setUp(void) {
  inPtr  = new int;
  outPtr_int = new int;
  outPtr_str = new std::string;

  mTestObj_1.setInputObj(&inPtr);
  mTestObj_2.setInputObj(&inPtr);

  mTestObj_1.setOutputObj(&outPtr_int);
  mTestObj_2.setOutputObj(&outPtr_str);

  *inPtr = 3;
}

void TestNProcessor::tearDown(void) {
  delete inPtr;
  delete outPtr_int;
}

void TestNProcessor::TestProcessSameData(void) {
  mTestObj_1.process();
  int testOut = *(mTestObj_1.getOutputPtr());
  CPPUNIT_ASSERT (testOut == 58);
}

void TestNProcessor::TestProcessDiffData(void) {
  mTestObj_2.process();
  std::string testOut = *(mTestObj_2.getOutputPtr());
  CPPUNIT_ASSERT (testOut == "output is -197");
}

void TestNProcessor::TestSetInOut(void) {
  mTestObj_1.setInputObj(&inPtr);
  mTestObj_1.setOutputObj(&inPtr);

  *inPtr = 17;
  int testOut = *(mTestObj_1.getOutputPtr());
  CPPUNIT_ASSERT (testOut == 17);

  *inPtr = 21;
  testOut = **(mTestObj_1.getOutputObj());
  CPPUNIT_ASSERT (testOut == 21);
}