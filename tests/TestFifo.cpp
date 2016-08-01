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

#include "TestFifo.h"

void TestNFifo::setUp(void) {
  mTestObj = new Fifo<int*>(11);
}

void TestNFifo::tearDown(void) {
  delete mTestObj;
}

void TestNFifo::testPushPop(void) {
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

void TestNFifo::testSetGetMaxSize(void) {
  mTestObj->setMaxSize(77);
  int maxSize = mTestObj->getMaxSize();
  CPPUNIT_ASSERT(maxSize == 77);
}

void TestNFifo::testSetGetNProducers(void) {
  mTestObj->setNProducers(22);
  int nPr = mTestObj->getNProducers();
  CPPUNIT_ASSERT(nPr == 22);
}

void TestNFifo::testAddRmProducer(void) {
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

void TestNFifo::testSetGetNConsummers(void) {
  mTestObj->setNConsummers(22);
  int nPr = mTestObj->getNConsummers();
  CPPUNIT_ASSERT(nPr == 22);
}

void TestNFifo::testAddRmNConsummers(void) {
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

void TestNFifo::testStopWork(void) {
  mTestObj->setMaxSize(10);
  CPPUNIT_ASSERT(mTestObj->size() == 0);

  mTestObj->setNConsummers(5);
  mTestObj->setNProducers(2);
  mTestObj->rmProducer();
  CPPUNIT_ASSERT(mTestObj->size() == 0);
  mTestObj->rmProducer();
  CPPUNIT_ASSERT(mTestObj->size() == 5);
  for (int i=0; i<5; ++i) {
    auto x = mTestObj->pop();
    CPPUNIT_ASSERT (x == nullptr);
    delete x;
  }
}

