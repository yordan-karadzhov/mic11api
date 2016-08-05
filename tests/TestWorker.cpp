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

#include "TestWorker.h"

bool tproc_int1::process() {
  **output_ += 55;
//   std::cout << "  " << processCount_ << "  " << **input_ << " -> " << **output_ << std::endl;
  return true;
}

using namespace std;

void TestNWorker::setUp(void) {
  mTestObj_1.enableTimeStats();
  mTestObj_2.enableTimeStats();
  mTestObj_3.enableTimeStats();
}

void TestNWorker::tearDown(void) {}

void TestNWorker::Test1Worker(void) {

  auto fifo_ptr = mTestObj_1.getInput();
  fifo_ptr->setMaxSize(11);
  for (int i=0;i<10;++i)
    fifo_ptr->push(new int(i+3));

  fifo_ptr->push(nullptr);
  CPPUNIT_ASSERT(mTestObj_1.getInFifoPtr()->size() == 11);

  mTestObj_1.init("");
  mTestObj_1.start();

  CPPUNIT_ASSERT(mTestObj_1.getCount() == 10);
  CPPUNIT_ASSERT(mTestObj_1.getInFifoPtr()->size() == 0);
  CPPUNIT_ASSERT(mTestObj_1.getOutFifoPtr()->size() == 10);
}

#define NPROC 16
void TestNWorker::Test2WorkersSameData(void) {

  mTestObj_1.getInput()->setMaxSize(NPROC+1);
  mTestObj_1.getOutput()->setMaxSize(NPROC+1);

  mTestObj_2.getInput()->setMaxSize(NPROC+1);
  mTestObj_2.getOutput()->setMaxSize(NPROC+1);

  auto fifo_ptr = mTestObj_1.getInput();

  for (int i=0;i<NPROC;++i)
    fifo_ptr->push(new int(i+33));

  fifo_ptr->push(nullptr);

  mTestObj_2.setInFifoPtr( mTestObj_1.getOutFifoPtr() );

  CPPUNIT_ASSERT(mTestObj_1.getInFifoPtr()->size() == NPROC+1);
  CPPUNIT_ASSERT(mTestObj_1.getOutFifoPtr()->size() == 0);

  mTestObj_1.init("");
  mTestObj_2.init("");
  mTestObj_1.start();

  CPPUNIT_ASSERT(mTestObj_1.getCount() == NPROC);
  CPPUNIT_ASSERT(mTestObj_1.getInFifoPtr()->size() == 0);
  CPPUNIT_ASSERT(mTestObj_1.getOutFifoPtr()->size() == NPROC+1);

  mTestObj_2.start();
  CPPUNIT_ASSERT(mTestObj_2.getCount() == NPROC);

  for (int i=0;i<NPROC;++i) {
    auto obj = mTestObj_2.getOutFifoPtr()->pop();
    CPPUNIT_ASSERT(*obj == i+33+55+55);
    delete obj;
  }

  CPPUNIT_ASSERT(mTestObj_2.getOutFifoPtr()->size() == 0);
//   auto obj = mTestObj_2.getOutFifoPtr()->pop();
//   CPPUNIT_ASSERT(obj == nullptr);
}

void TestNWorker::Test2WorkersDiffData(void) {

  mTestObj_1.getInput()->setMaxSize(NPROC+1);
  mTestObj_3.getInput()->setMaxSize(NPROC+1);
  mTestObj_1.getOutput()->setMaxSize(NPROC+1);
  mTestObj_3.getOutput()->setMaxSize(NPROC+1);

  auto fifo_ptr = mTestObj_1.getInput();
  for (int i=0;i<NPROC;++i)
    fifo_ptr->push(new int(i+16));

  fifo_ptr->push(nullptr);

  mTestObj_3.setInFifoPtr( mTestObj_1.getOutFifoPtr() );

  CPPUNIT_ASSERT(mTestObj_1.getInFifoPtr()->size() == NPROC+1);
  CPPUNIT_ASSERT(mTestObj_1.getOutFifoPtr()->size() == 0);

  mTestObj_1.init("");
  mTestObj_3.init("");

  mTestObj_1.start();

  CPPUNIT_ASSERT(mTestObj_1.getCount() == NPROC);
  CPPUNIT_ASSERT(mTestObj_1.getInFifoPtr()->size() == 0);
  CPPUNIT_ASSERT(mTestObj_1.getOutFifoPtr()->size() == NPROC+1);

  mTestObj_3.start();
  CPPUNIT_ASSERT (mTestObj_3.getCount() == NPROC);

  for (int i=0;i<NPROC;++i) {
    auto obj = mTestObj_3.getOutFifoPtr()->pop();
    std::stringstream ss;
    ss << "output is " << i+16+55-200;
    CPPUNIT_ASSERT(*obj == ss.str());
    delete obj;
  }

  CPPUNIT_ASSERT(mTestObj_3.getOutFifoPtr()->size() == 0);
//   auto obj = mTestObj_3.getOutFifoPtr()->pop();
//   CPPUNIT_ASSERT(obj == nullptr);
}


