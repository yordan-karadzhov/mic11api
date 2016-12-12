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

#include <typeinfo>
#include <chrono>

#include "TestMacros.h"

void TestMacros::setUp(void) {}

void TestMacros::tearDown(void) {}

IMPLEMENT_INPUT(I1, int, tproc_int1);

IMPLEMENT_USER_INPUT(I2, int, tproc_int1);
void I2::start(int n) {}

IMPLEMENT_UWORKER(UW1, int, tproc_int1);

IMPLEMENT_USER_UWORKER(UW2, int, tproc_int1);
void UW2::start(int n) {}

IMPLEMENT_TWORKER(TW1, int, std::string, tproc_str);

IMPLEMENT_USER_TWORKER(TW2, int, std::string, tproc_str);
void TW2::start(int n) {}
;

IMPLEMENT_OUTPUT(O1, int, tout_int);

IMPLEMENT_USER_OUTPUT(O2, int, tout_int);
void O2::start(int n) {}

void TestMacros::TestStreams() {

  I1   i1(0);
  I2   ie(1);
  UW1  uw1(2);
  UW2  uw2(3);
  TW1  tw1(4);
  TW2  tw2(5);
  O1   o1(6);
  O2   o2(7);
}

class t_Idle : public Idle {};
class t_Standby : public Standby {};
class t_Active : public Active {};
class t_Failure : public Failure {};
class t_FatalError : public FatalError {};

#define stateMap1(STATE) \
  STATE(DEFAULT_STANDBY) \
  STATE(USER_ACTIVE(t_Active)) \
  STATE(DEFAULT_IDLE) \
  STATE(USER_FAILURE(t_Failure)) \
  STATE(DEFAULT_FATALERROR)

IMPLEMENT_STATE_FACTORY(stateMap1, t1_StateFactory)


#define stateMap2(STATE) \
  STATE(DEFAULT_IDLE) \
  STATE(USER_ACTIVE(t_Active)) \
  STATE(USER_STANDBY(t_Standby)) \
  STATE(DEFAULT_FAILURE) \
  STATE(DEFAULT_FATALERROR)

IMPLEMENT_STATE_FACTORY(stateMap2, t2_StateFactory)


#define stateMap3(STATE) \
  STATE(DEFAULT_ACTIVE) \
  STATE(USER_IDLE(t_Idle)) \
  STATE(DEFAULT_FAILURE) \
  STATE(DEFAULT_STANDBY) \
  STATE(USER_FATALERROR(t_FatalError))

IMPLEMENT_STATE_FACTORY(stateMap3, t3_StateFactory)

void TestMacros::TestStateFactory() {

  t1_StateFactory f1;
  State *st = f1.newState(fsm_state_t::Active_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(t_Active));
  delete st;

  st = f1.newState(fsm_state_t::Idle_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(Idle));
  delete st;

  st = f1.newState(fsm_state_t::Standby_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(Standby));
  delete st;

  st = f1.newState(fsm_state_t::Failure_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(t_Failure));
  delete st;

  st = f1.newState(fsm_state_t::FatalError_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(FatalError));
  delete st;

  t2_StateFactory f2;
  st = f2.newState(fsm_state_t::Active_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(t_Active));
  delete st;

  st = f2.newState(fsm_state_t::Idle_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(Idle));
  delete st;

  st = f2.newState(fsm_state_t::Standby_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(t_Standby));
  delete st;

  st = f2.newState(fsm_state_t::Failure_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(Failure));
  delete st;

  st = f1.newState(fsm_state_t::FatalError_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(FatalError));
  delete st;

  t3_StateFactory f3;
  st = f3.newState(fsm_state_t::Active_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(Active));
  delete st;

  st = f3.newState(fsm_state_t::Idle_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(t_Idle));
  delete st;

  st = f3.newState(fsm_state_t::Standby_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(Standby));
  delete st;

  st = f3.newState(fsm_state_t::Failure_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(Failure));
  delete st;

  st = f3.newState(fsm_state_t::FatalError_s);
  CPPUNIT_ASSERT (typeid(*st) == typeid(t_FatalError));
  delete st;
}