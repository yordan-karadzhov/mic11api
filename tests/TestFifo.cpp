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

#include "Processor.h"
#include "TestFifo.h"

void TestFifo::setUp() {
  mTestObj = new Fifo<int*>(11);
}

void TestFifo::tearDown() {
  delete mTestObj;
}

void TestFifo::testPushPop() {
  for (int i=0; i<10; ++i)
    mTestObj->push(new int(100+i));

  CPPUNIT_ASSERT(mTestObj->size() == 10);

  for (int i=0; i<10; ++i) {
    int* x = mTestObj->pop();
    CPPUNIT_ASSERT (*x == 100+i);
    delete x;
  }

  CPPUNIT_ASSERT(mTestObj->size() == 0);
}

void TestFifo::testSetGetMaxSize() {
  mTestObj->setMaxSize(77);
  int maxSize = mTestObj->getMaxSize();
  CPPUNIT_ASSERT(maxSize == 77);
}

void TestFifo::testSetGetNProducers() {
  mTestObj->setNProducers(22);
  int nPr = mTestObj->getNProducers();
  CPPUNIT_ASSERT(nPr == 22);
}

void TestFifo::testAddRmProducer() {
  mTestObj->setNProducers(0);
  for (int i=0; i<44; ++i)
    mTestObj->addProducer();

  int nPr = mTestObj->getNProducers();
  CPPUNIT_ASSERT(nPr == 44);

  for (int i=0; i<44; ++i)
    mTestObj->rmProducer();

  nPr = mTestObj->getNProducers();
  CPPUNIT_ASSERT(nPr == 0);
}

void TestFifo::testSetGetNConsummers() {
  mTestObj->setNConsummers(22);
  int nPr = mTestObj->getNConsummers();
  CPPUNIT_ASSERT(nPr == 22);
}

void TestFifo::testAddRmNConsummers() {
  mTestObj->setNConsummers(0);
  for (int i=0; i<44; ++i)
    mTestObj->addConsummer();

  int nPr = mTestObj->getNConsummers();
  CPPUNIT_ASSERT(nPr == 44);

  for (int i=0; i<44; ++i)
    mTestObj->rmConsummer();

  nPr = mTestObj->getNConsummers();
  CPPUNIT_ASSERT(nPr == 0);
}

void TestFifo::testNode() {
  Node *n1 = mTestObj;
  n1->setNConsummers(5);
  n1->setMaxSize(30);
  for (int i=0; i<30; ++i)
    mTestObj->push(new int(100+i));

  CPPUNIT_ASSERT(n1->size() == 30);
}

void TestFifo::testStopWork() {
  mTestObj->setMaxSize(10);
  CPPUNIT_ASSERT(mTestObj->size() == 0);

  mTestObj->setNConsummers(5);
  mTestObj->setNProducers(2);
  mTestObj->rmActiveProducer(proc_status_t::OK_s);
  CPPUNIT_ASSERT(mTestObj->size() == 0);
  mTestObj->rmActiveProducer(proc_status_t::OK_s);
  CPPUNIT_ASSERT(mTestObj->size() == 5);
  for (int i=0; i<5; ++i) {
    auto x = mTestObj->pop();
    CPPUNIT_ASSERT (x == nullptr);
    delete x;
  }
}

void TestFifo::testError() {
  CPPUNIT_ASSERT(mTestObj->size() == 0);
  mTestObj->setNConsummers(4);
  mTestObj->setNProducers(9);

  mTestObj->consummerError(proc_status_t::Error_s);
  CPPUNIT_ASSERT(mTestObj->status() == proc_status_t::Error_s);
  CPPUNIT_ASSERT(mTestObj->getNConsummers() == 4);
  mTestObj->resetError();
  CPPUNIT_ASSERT(mTestObj->status() == proc_status_t::OK_s);

  mTestObj->rmActiveProducer(proc_status_t::FatalError_s);
  CPPUNIT_ASSERT(mTestObj->status() == proc_status_t::FatalError_s);
  CPPUNIT_ASSERT(mTestObj->getNActiveProducers() == 8);
  mTestObj->resetError();
  CPPUNIT_ASSERT(mTestObj->status() == proc_status_t::OK_s);

  CPPUNIT_ASSERT(mTestObj->size() == 0);
  mTestObj->setNProducers(1);
  CPPUNIT_ASSERT(mTestObj->getNActiveProducers() == 1);
  mTestObj->rmActiveProducer(proc_status_t::Error_s);
  CPPUNIT_ASSERT(mTestObj->size() == 4);

  for (int i=0; i<4; ++i) {
    int* x = mTestObj->pop();
    CPPUNIT_ASSERT (x == nullptr);
  }
}

