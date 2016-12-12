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

#ifndef NFIFO_TEST_HPP_
#define NFIFO_TEST_HPP_

// CppUnit
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

// Mic11
#include "src/Fifo.h"

class TestFifo  : public CppUnit::TestFixture {

 public:
  void setUp();
  void tearDown();

 protected:
  void testPushPop();
  void testSetGetMaxSize();
  void testSize();
  void testSetGetNProducers();
  void testAddRmProducer();
  void testSetGetNConsummers();
  void testAddRmNConsummers();
  void testNode();
  void testStopWork();
  void testError();

 private:
  Fifo<int*> *mTestObj;

  CPPUNIT_TEST_SUITE( TestFifo );
  CPPUNIT_TEST( testPushPop );
  CPPUNIT_TEST( testSetGetMaxSize );
  CPPUNIT_TEST( testSetGetNProducers );
  CPPUNIT_TEST( testAddRmProducer );
  CPPUNIT_TEST( testSetGetNConsummers );
  CPPUNIT_TEST( testAddRmNConsummers );
  CPPUNIT_TEST( testNode );
  CPPUNIT_TEST( testStopWork );
  CPPUNIT_TEST( testError );
  CPPUNIT_TEST_SUITE_END();
};

#endif
