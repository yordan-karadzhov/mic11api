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

#include "tests/TestFsm.h"
#include "tests/TestWorker.h"

void TestFsm::setUp() {}

void TestFsm::tearDown() {}

void TestFsm::TestGetWorkers() {
  twork_int w0, w1, w2, w3;

  testObj_.getWorkers().push_back(&w0);
  testObj_.getWorkers().push_back(&w1);
  testObj_.getWorkers().push_back(&w2);
  testObj_.getWorkers().push_back(&w3);

  CPPUNIT_ASSERT(testObj_.getWorkers().size() == 4);
}

void TestFsm::TestGetNodes() {
  Fifo<int> n0, n1, n2, n3;

  testObj_.getNodes().push_back(&n0);
  testObj_.getNodes().push_back(&n1);
  testObj_.getNodes().push_back(&n2);
  testObj_.getNodes().push_back(&n3);

  CPPUNIT_ASSERT(testObj_.getNodes().size() == 4);
}

void TestFsm::TestGetStatus() {
  twork_int w0, w1, w2, w3;
  testObj_.getWorkers().push_back(&w0);
  testObj_.getWorkers().push_back(&w1);
  testObj_.getWorkers().push_back(&w2);
  testObj_.getWorkers().push_back(&w3);

  w0.setStatus(proc_status_t::OK_s);
  w1.setStatus(proc_status_t::Interrupt_s);
  w2.setStatus(proc_status_t::OK_s);
  w3.setStatus(proc_status_t::OK_s);

  CPPUNIT_ASSERT(testObj_.getWorkersStatus() == proc_status_t::Interrupt_s);

  w2.setStatus(proc_status_t::Error_s);
  CPPUNIT_ASSERT(testObj_.getWorkersStatus() == proc_status_t::Error_s);

  w0.setStatus(proc_status_t::FatalError_s);
  CPPUNIT_ASSERT(testObj_.getWorkersStatus() == proc_status_t::FatalError_s);

  Fifo<int> n0, n1, n2, n3;

  testObj_.getNodes().push_back(&n0);
  testObj_.getNodes().push_back(&n1);
  testObj_.getNodes().push_back(&n2);
  testObj_.getNodes().push_back(&n3);

  n0.setStatus(proc_status_t::OK_s);
  n1.setStatus(proc_status_t::Interrupt_s);
  n2.setStatus(proc_status_t::OK_s);
  n3.setStatus(proc_status_t::OK_s);

  CPPUNIT_ASSERT(testObj_.getNodesStatus() == proc_status_t::Interrupt_s);

  n2.setStatus(proc_status_t::Error_s);
  CPPUNIT_ASSERT(testObj_.getNodesStatus() == proc_status_t::Error_s);

  n0.setStatus(proc_status_t::FatalError_s);
  CPPUNIT_ASSERT(testObj_.getNodesStatus() == proc_status_t::FatalError_s);
}

void TestFsm::TestHandleInput() {
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Idle_s);

  testObj_.handleInput(fsm_input_t::On_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Standby_s);

  testObj_.handleInput(fsm_input_t::Start_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Active_s);

  testObj_.handleInput(fsm_input_t::Stop_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Standby_s);

  testObj_.handleInput(fsm_input_t::Off_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Idle_s);

  testObj_.handleInput(fsm_input_t::Start_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Active_s);

  testObj_.handleInput(fsm_input_t::Off_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Idle_s);

  testObj_.handleInput(fsm_input_t::Error_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Failure_s);

  testObj_.handleInput(fsm_input_t::On_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Failure_s);

  testObj_.handleInput(fsm_input_t::Start_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Failure_s);

  testObj_.handleInput(fsm_input_t::Off_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Idle_s);

  testObj_.handleInput(fsm_input_t::On_i);
  testObj_.handleInput(fsm_input_t::Error_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Failure_s);

  testObj_.handleInput(fsm_input_t::Off_i);
  testObj_.handleInput(fsm_input_t::Start_i);
  testObj_.handleInput(fsm_input_t::Error_i);
  CPPUNIT_ASSERT(testObj_.stateId() == fsm_state_t::Failure_s);
}

unsigned int idle_entering(0), idle_same(0), idle_leaving(0);
unsigned int standby_entering(0), standby_same(0), standby_leaving(0);
unsigned int active_entering(0), active_same(0), active_leaving(0);
unsigned int failure_entering(0), failure_same(0), failure_leaving(0);
unsigned int fatal_entering(0);

class t1_Idle : public Idle {
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) override {idle_entering++;}
  void whenSameStateDo(Fsm *sm) override {idle_same++;}
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) override {idle_leaving++;}
};

class t1_Standby : public Standby {
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) override {standby_entering++;}
  void whenSameStateDo(Fsm *sm) override {standby_same++;}
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) override {standby_leaving++;}
};

class t1_Active : public Active {
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) override {active_entering++;}
  void whenSameStateDo(Fsm *sm) override {active_same++;}
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) override {active_leaving++;}
};

class t1_Failure : public Failure {
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) override {failure_entering++;}
  void whenSameStateDo(Fsm *sm) override {failure_same++;}
  void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) override {failure_leaving++;}
};

class t1_FatalError : public FatalError {
  void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) override {fatal_entering++;}
};

#define stateMap1(STATE) \
  STATE(USER_IDLE(t1_Idle)) \
  STATE(USER_STANDBY(t1_Standby)) \
  STATE(USER_ACTIVE(t1_Active)) \
  STATE(USER_FAILURE(t1_Failure)) \
  STATE(USER_FATALERROR(t1_FatalError))

IMPLEMENT_STATE_FACTORY(stateMap1, t4_StateFactory)


void TestFsm::TestUserStates() {
//   t4_StateFactory factory;
  testObj_.setUserFactory<t4_StateFactory>();

  CPPUNIT_ASSERT(idle_entering == 1);

  testObj_.handleInput(fsm_input_t::Off_i);
  CPPUNIT_ASSERT(idle_same == 1);

  testObj_.handleInput(fsm_input_t::On_i);
  CPPUNIT_ASSERT(idle_leaving == 1);
  CPPUNIT_ASSERT(standby_entering == 1);

  testObj_.handleInput(fsm_input_t::On_i);
  CPPUNIT_ASSERT(standby_same == 1);

  testObj_.handleInput(fsm_input_t::Start_i);
  CPPUNIT_ASSERT(standby_leaving == 1);
  CPPUNIT_ASSERT(active_entering == 1);

  testObj_.handleInput(fsm_input_t::Start_i);
  CPPUNIT_ASSERT(active_same == 1);

  testObj_.handleInput(fsm_input_t::Stop_i);
  CPPUNIT_ASSERT(active_leaving == 1);
  CPPUNIT_ASSERT(standby_entering == 2);

  testObj_.handleInput(fsm_input_t::Stop_i);
  CPPUNIT_ASSERT(standby_same == 2);

  testObj_.handleInput(fsm_input_t::Off_i);
  CPPUNIT_ASSERT(standby_leaving == 2);
  CPPUNIT_ASSERT(idle_entering == 2);

  testObj_.handleInput(fsm_input_t::Start_i);
  CPPUNIT_ASSERT(idle_leaving == 2);
  CPPUNIT_ASSERT(standby_entering == 3);
  CPPUNIT_ASSERT(standby_leaving == 3);
  CPPUNIT_ASSERT(active_entering == 2);

  testObj_.handleInput(fsm_input_t::Off_i);
  CPPUNIT_ASSERT(idle_entering == 3);
  CPPUNIT_ASSERT(standby_entering == 4);
  CPPUNIT_ASSERT(standby_leaving == 4);
  CPPUNIT_ASSERT(active_leaving == 2);

  testObj_.handleInput(fsm_input_t::Error_i);
  CPPUNIT_ASSERT(failure_entering == 1);

  testObj_.handleInput(fsm_input_t::Start_i);
  CPPUNIT_ASSERT(failure_same == 1);

  testObj_.handleInput(fsm_input_t::Stop_i);
  CPPUNIT_ASSERT(failure_same == 2);

  testObj_.handleInput(fsm_input_t::Off_i);
  CPPUNIT_ASSERT(failure_leaving == 1);
  CPPUNIT_ASSERT(idle_entering == 4);

  testObj_.changeState(fsm_state_t::FatalError_s);
  CPPUNIT_ASSERT(fatal_entering == 1);
}

#include <chrono>
#include <thread>
#include <mutex>
using namespace std::chrono;
std::mutex mtx;

class t5_input : public InProcessor<int> {
 public:
  t5_input(): InProcessor("t5_input") {}
  void init(std::string s, void*) override {}
  proc_status_t process() override;
  void close(proc_status_t st) override {}
};

std::atomic_uint i_count(10);

proc_status_t t5_input::process() {
  **output_ = ++i_count + 220;
  std::this_thread::sleep_for( milliseconds(5) );
//   if (**output_ == 1255)
//     return Interrupt_s;

  return proc_status_t::OK_s;
}


class t5_proc : public InProcessor<int> {
 public:
  t5_proc() : InProcessor("t5_proc") {}
  void init(std::string s, void*) override {}
  proc_status_t process() override;
  void close(proc_status_t st) override {}
};

proc_status_t t5_proc::process() {
  std::stringstream ss;
  **output_ -= 120;
  std::this_thread::sleep_for( milliseconds(3) );

  return proc_status_t::OK_s;
}

class t5_output : public OutProcessor<int> {
 public:
  t5_output() : OutProcessor("t5_output") {}
  void init(std::string s, void*) override {}
  proc_status_t process() override;
  void close(proc_status_t st) override {}
};

proc_status_t t5_output::process() {
//   std::unique_lock<std::mutex> lck (mtx, std::defer_lock);
//   lck.lock();
//   std::cout << "output: " << **input_ << std::endl;
//   lck.unlock();

  std::this_thread::sleep_for( milliseconds(6) );
  if (**input_ == 1750)
    return proc_status_t::Interrupt_s;

  if (**input_ == 2750)
    return proc_status_t::Error_s;

  if (**input_ == 4000)
    return proc_status_t::FatalError_s;

  return proc_status_t::OK_s;
}

IMPLEMENT_INPUT(t5_input_worker, int, t5_input)   // worker name, data type, proce name
IMPLEMENT_UWORKER(t5_proc_worker, int, t5_proc)
IMPLEMENT_OUTPUT(t5_output_worker, int, t5_output)

int input_count=0;
Fsm fsm;
t5_input_worker  w0(0);
t5_proc_worker   w1(1), w2(2);
t5_output_worker w3(3);

int getTestInput() {
//   cout << "Enter 1/2/3/4/>4 (On / Start / Stop / Off / Exit): \n";
  
  switch (input_count) {
    case 0:
      std::this_thread::sleep_for(milliseconds(10));
      input_count++;
      std::cout << "\ninput: " << 2 << std::endl;
      return 2;

    case 1:
      std::this_thread::sleep_for(seconds(3));
      std::cout << fsm.stateId() << " " << w0.getCount() << std::endl;
      w0.printStats();
      w1.printStats();
      w2.printStats();
      w3.printStats();
      input_count++;
      std::cout << "\ninput: " << 6 << std::endl;
      return 6;

    default:
      std::this_thread::sleep_for(seconds(2));
      std::cout << fsm.stateId() << " " << w0.getCount() << std::endl;
      input_count++;
      std::cout << "\ninput: " << 6 << std::endl;
      return 6;
  }
}

void TestFsm::TestStop() {
  w0 >> w1.getInput();
  w0 >> w2.getInput();
  w1 << w3.getInput();
  w2 << w3.getInput();

  w0.enableStats();
  w1.enableStats();
  w2.enableStats();
  w3.enableStats();

  fsm.getWorkers().push_back(&w0);
  fsm.getWorkers().push_back(&w1);
  fsm.getWorkers().push_back(&w2);
  fsm.getWorkers().push_back(&w3);

  fsm.getNodes().push_back(w0.getOutput().get());
  fsm.getNodes().push_back(w3.getInput().get());

//   fsm.start(&getTestInput, 1000);


}

void TestFsm::TestError() {
  
}
