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

proc_status_t tproc_int1::process() {
  **output_ += 55;
//   std::cout << "  " << processCount_ << " " << **output_ << std::endl;
  if (**output_ < 0)
    return proc_status_t::Error_s;

  if (**output_ > 200)
    return proc_status_t::Interrupt_s;

  return proc_status_t::OK_s;
}

using namespace std;

void TestWorker::setUp() {
  mTestObj_1.enableStats();
  mTestObj_2.enableStats();
  mTestObj_3.enableStats();
}

void TestWorker::tearDown() {}

void TestWorker::Test1Worker() {

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
void TestWorker::Test2WorkersSameData() {

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

void TestWorker::Test2WorkersDiffData() {

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

void TestWorker::TestStop() {
  auto fifo_in_ptr = mTestObj_1.getInput();
  auto fifo_out_ptr = mTestObj_1.getOutput();
  CPPUNIT_ASSERT(fifo_in_ptr->size() == 0);
  fifo_in_ptr->setMaxSize(11);
  for (int i=0;i<10;++i)
    fifo_in_ptr->push(new int(i+143));

  fifo_in_ptr->push(nullptr);
  CPPUNIT_ASSERT(fifo_in_ptr->size() == 11);
  CPPUNIT_ASSERT(fifo_out_ptr->size() == 0);

  mTestObj_1.init("");
  mTestObj_1.start();
  CPPUNIT_ASSERT(fifo_in_ptr->size() == 7);
  CPPUNIT_ASSERT(fifo_out_ptr->size() == 3);

  CPPUNIT_ASSERT(mTestObj_1.status() == proc_status_t::Interrupt_s);
  CPPUNIT_ASSERT(fifo_in_ptr->status() == proc_status_t::OK_s);
  CPPUNIT_ASSERT(fifo_out_ptr->status() == proc_status_t::Interrupt_s);
}

void TestWorker::TestError() {
  auto fifo_in_ptr = mTestObj_1.getInput();
  auto fifo_out_ptr = mTestObj_1.getOutput();
  CPPUNIT_ASSERT(fifo_in_ptr->size() == 0);
  fifo_in_ptr->setMaxSize(11);
  for (int i=0;i<10;++i) {
    if (i==6)
      fifo_in_ptr->push(new int(-200));
    else
      fifo_in_ptr->push(new int(i+13));
  }

  fifo_in_ptr->push(nullptr);
  CPPUNIT_ASSERT(fifo_in_ptr->size() == 11);
  CPPUNIT_ASSERT(fifo_out_ptr->size() == 0);

  mTestObj_1.init("");
  mTestObj_1.start();

  CPPUNIT_ASSERT(fifo_in_ptr->size() == 4);
  CPPUNIT_ASSERT(fifo_out_ptr->size() == 6);

  CPPUNIT_ASSERT(mTestObj_1.status() == proc_status_t::Error_s);
  CPPUNIT_ASSERT(fifo_in_ptr->status() == proc_status_t::Error_s);
  CPPUNIT_ASSERT(fifo_out_ptr->status() == proc_status_t::Error_s);
}
