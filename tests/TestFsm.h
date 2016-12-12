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

#ifndef FSM_TEST_HPP_
#define FSM_TEST_HPP_

// C++
#include <string>
#include <sstream>

// CppUnit
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

// Mic11
#include "src/Fsm.h"
#include "src/State.h"

class MyIdle : public Idle {
public:
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) override {}
  void whenSameStateDo(Fsm *sm) override {}
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) override {}
};

class MyStandby : public Standby {
public:
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) override {}
  void whenSameStateDo(Fsm *sm) override {}
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) override {}
};

class MyActive : public Active {
public:
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) override {}
  void whenSameStateDo(Fsm *sm) override {}
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) override {}
};

class MyFailure : public Failure {
public:
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) override {}
  void whenSameStateDo(Fsm *sm) override {}
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) override {}
};


class TestFsm : public CppUnit::TestFixture {

 public:
  void setUp();
  void tearDown();

 protected:
  void TestGetWorkers();
  void TestGetNodes();
  void TestGetStatus();
  void TestHandleInput();
  void TestUserStates();
  void TestStop();
  void TestError();

 private:
  Fsm testObj_;

  CPPUNIT_TEST_SUITE( TestFsm );
  CPPUNIT_TEST( TestGetWorkers );
  CPPUNIT_TEST( TestGetNodes );
  CPPUNIT_TEST( TestGetStatus );
  CPPUNIT_TEST( TestHandleInput );
  CPPUNIT_TEST( TestUserStates );
  CPPUNIT_TEST( TestStop );
  CPPUNIT_TEST( TestError );
  CPPUNIT_TEST_SUITE_END();
};

#endif
