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

void TestFsm::setUp() {
  w0 = new t5_input_worker(0);
  w1 = new t5_proc_worker(1);
  w2 = new t5_proc_worker(2);
  w3 = new t5_output_worker(3);
}

void TestFsm::tearDown() {
  delete w0;
  delete w1;
  delete w2;
  delete w3;
}

void TestFsm::TestGetWorkers() {
  twork_int w0, w1, w2, w3;

  testObj_.addWorker(&w0);
  testObj_.addWorker(&w1);
  testObj_.addWorker(&w2);
  testObj_.addWorker(&w3);

  CPPUNIT_ASSERT(testObj_.getWorkers().size() == 4);
}

void TestFsm::TestGetNodes() {
  std::shared_ptr< Fifo<int> > n0(new Fifo<int>), n1(new Fifo<int>), n2(new Fifo<int>), n3(new Fifo<int>);

  testObj_.addNode(n0);
  testObj_.addNode(n1);
  testObj_.addNode(n2);
  testObj_.addNode(n3);

  CPPUNIT_ASSERT(testObj_.getNodes().size() == 4);
}

void TestFsm::TestGetStatus() {
  testObj_.addWorker(w0);
  testObj_.addWorker(w1);
  testObj_.addWorker(w2);
  testObj_.addWorker(w3);

  w0->setStatus(proc_status_t::OK_s);
  w1->setStatus(proc_status_t::Interrupt_s);
  w2->setStatus(proc_status_t::OK_s);
  w3->setStatus(proc_status_t::OK_s);

  CPPUNIT_ASSERT(testObj_.getWorkersStatus() == proc_status_t::Interrupt_s);

  w2->setStatus(proc_status_t::Error_s);
  CPPUNIT_ASSERT(testObj_.getWorkersStatus() == proc_status_t::Error_s);

  w0->setStatus(proc_status_t::FatalError_s);
  CPPUNIT_ASSERT(testObj_.getWorkersStatus() == proc_status_t::FatalError_s);

  std::shared_ptr< Fifo<int> > n0(new Fifo<int>), n1(new Fifo<int>), n2(new Fifo<int>), n3(new Fifo<int>);;

  testObj_.addNode(n0);
  testObj_.addNode(n1);
  testObj_.addNode(n2);
  testObj_.addNode(n3);

  n0->setStatus(proc_status_t::OK_s);
  n1->setStatus(proc_status_t::Interrupt_s);
  n2->setStatus(proc_status_t::OK_s);
  n3->setStatus(proc_status_t::OK_s);

  CPPUNIT_ASSERT(testObj_.getNodesStatus() == proc_status_t::Interrupt_s);

  n2->setStatus(proc_status_t::Error_s);
  CPPUNIT_ASSERT(testObj_.getNodesStatus() == proc_status_t::Error_s);

  n0->setStatus(proc_status_t::FatalError_s);
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
int i_count = 0;

int output_interrupt_count = 0;
int proc_interrupt_count = 0;

int error_count = 0;
int fatal_count = 0;

proc_status_t t5_input::process() {
  **output_ = ++i_count + 220;
  std::this_thread::sleep_for( microseconds(45) );
//   if (**output_ == 1255)
//     return Interrupt_s;

  return proc_status_t::OK_s;
}


proc_status_t t5_proc::process() {
  std::stringstream ss;
  **output_ -= 120;
  std::this_thread::sleep_for( microseconds(27) );
  if (**output_ == proc_interrupt_count)
    return proc_status_t::Interrupt_s;

  if (**output_ == error_count)
    return proc_status_t::Error_s;

  return proc_status_t::OK_s;
}


proc_status_t t5_output::process() {
//   std::unique_lock<std::mutex> lck (mtx, std::defer_lock);
//   lck.lock();
//   std::cout << "output: " << **input_ << std::endl;
//   lck.unlock();

  std::this_thread::sleep_for( microseconds(61) );
  if (**input_ == output_interrupt_count)
    return proc_status_t::Interrupt_s;


  if (**input_ == fatal_count)
    return proc_status_t::FatalError_s;

  return proc_status_t::OK_s;
}


int input_count=0;

void TestFsm::TestStop() {

  testObj_.changeState(fsm_state_t::Idle_s);

  input_count=0;
  i_count = 0;

  output_interrupt_count = 0;
  proc_interrupt_count = 0;

  error_count = 0;
  fatal_count = 0;

  *w0 >> w1->getInput();
  *w0 >> w2->getInput();
  *w1 << w3->getInput();
  *w2 << w3->getInput();

  testObj_.enableStats(true);
  w0->enableStats();
  w1->enableStats();
  w2->enableStats();
  w3->enableStats();

  testObj_.addWorker(w0);
  testObj_.addWorker(w1);
  testObj_.addWorker(w2);
  testObj_.addWorker(w3);

  testObj_.addNode(w0->getOutput());
  testObj_.addNode(w3->getInput());

  auto input = [&] {
    switch (input_count) {
      case 0:
        std::this_thread::sleep_for(milliseconds(100));
        CPPUNIT_ASSERT( testObj_.stateId() == fsm_state_t::Idle_s);
        input_count++;
        output_interrupt_count = 0;
        std::cout << "\ninput (10ms): " << 2 << std::endl;
        return 2;

      case 1:
        std::this_thread::sleep_for(milliseconds(2000));
        CPPUNIT_ASSERT( testObj_.stateId() == fsm_state_t::Standby_s);
        CPPUNIT_ASSERT( testObj_.getNThreads() == 0);
        CPPUNIT_ASSERT( w0->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w1->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w2->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w3->status() == proc_status_t::OK_s);

        CPPUNIT_ASSERT( w0->getOutput()->getNActiveProducers() == 1);
        CPPUNIT_ASSERT( w1->getOutput()->getNActiveProducers() == 2);

        CPPUNIT_ASSERT( w0->getOutput()->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w3->getInput()->status() == proc_status_t::OK_s);

        CPPUNIT_ASSERT( testObj_.getStats()[0].process_count_ == 10000);
        CPPUNIT_ASSERT( testObj_.getStats()[3].process_count_ == 10000);

        input_count++;
        i_count = 0;
        proc_interrupt_count = 400;
        std::cout << "\ninput: " << 2 << std::endl;
        return 2;

      case 2:
        std::this_thread::sleep_for(milliseconds(400));
        CPPUNIT_ASSERT( testObj_.stateId() == fsm_state_t::Standby_s);
        CPPUNIT_ASSERT( testObj_.getNThreads() == 0);
        CPPUNIT_ASSERT( w0->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w1->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w2->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w3->status() == proc_status_t::OK_s);

        CPPUNIT_ASSERT( w0->getOutput()->getNActiveProducers() == 1);
        CPPUNIT_ASSERT( w1->getOutput()->getNActiveProducers() == 2);

        CPPUNIT_ASSERT( w0->getOutput()->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w3->getInput()->status() == proc_status_t::OK_s);

//         std::cout << "w0 " << testObj_.getStats()[0].process_count_
//                   << "\nw1 " << testObj_.getStats()[1].process_count_
//                   << "\nw2 " << testObj_.getStats()[2].process_count_
//                   << "\nw3 " << testObj_.getStats()[3].process_count_ << std::endl;

        {
          int in_c = testObj_.getStats()[0].process_count_;
          int p1_c = testObj_.getStats()[1].process_count_;
          int p2_c = testObj_.getStats()[2].process_count_;
          int out_c = testObj_.getStats()[3].process_count_;
          CPPUNIT_ASSERT ( (in_c - p1_c - p2_c) >= 0);
          CPPUNIT_ASSERT ( (p1_c + p2_c - out_c) >= 0);
        }

        input_count++;
        i_count = 0;
        output_interrupt_count = 800;
                proc_interrupt_count = 0;
        std::cout << "\ninput: " << 2 << std::endl;
        return 2;

      case 3:
        std::this_thread::sleep_for(milliseconds(800));
        CPPUNIT_ASSERT( testObj_.stateId() == fsm_state_t::Standby_s);
        CPPUNIT_ASSERT( testObj_.getNThreads() == 0);
        CPPUNIT_ASSERT( w0->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w1->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w2->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w3->status() == proc_status_t::OK_s);

        CPPUNIT_ASSERT( w0->getOutput()->getNActiveProducers() == 1);
        CPPUNIT_ASSERT( w1->getOutput()->getNActiveProducers() == 2);

        CPPUNIT_ASSERT( w0->getOutput()->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w3->getInput()->status() == proc_status_t::OK_s);

//         std::cout << i_count << " &&& " << o_count << std::endl;
//         CPPUNIT_ASSERT( i_count == 1000);
        CPPUNIT_ASSERT( testObj_.getStats()[3].process_count_ == 700);

        input_count++;

        std::cout << "\ninput: " << 2 << std::endl;
        return 2;

      case 4 :
        std::this_thread::sleep_for(milliseconds(300));
        input_count++;

        std::cout << "\ninput: " << 3 << std::endl;
        return 3;

      default:
        CPPUNIT_ASSERT( testObj_.stateId() == fsm_state_t::Standby_s);
        CPPUNIT_ASSERT( testObj_.getNThreads() == 0);
        CPPUNIT_ASSERT( w0->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w1->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w2->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w3->status() == proc_status_t::OK_s);

        CPPUNIT_ASSERT( w0->getOutput()->getNActiveProducers() == 1);
        CPPUNIT_ASSERT( w1->getOutput()->getNActiveProducers() == 2);

        CPPUNIT_ASSERT( w0->getOutput()->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w3->getInput()->status() == proc_status_t::OK_s);

        std::cout << "\ninput " << 6 << std::endl;
        return 6;
    }
  };

  testObj_.start(input, 10000);
  CPPUNIT_ASSERT( testObj_.stateId() == fsm_state_t::Idle_s);
  testObj_.dismissWorkers();
  testObj_.dismissNodes();
}

void TestFsm::TestError() {
  testObj_.changeState(fsm_state_t::Idle_s);

  input_count=0;
  i_count = 0;

  output_interrupt_count = 0;
  proc_interrupt_count = 0;

  error_count = 0;
  fatal_count = 0;

  *w0 >> w1->getInput();
  *w0 >> w2->getInput();
  *w1 << w3->getInput();
  *w2 << w3->getInput();

  testObj_.enableStats(true);
  w0->enableStats();
  w1->enableStats();
  w2->enableStats();
  w3->enableStats();

  testObj_.addWorker(w0);
  testObj_.addWorker(w1);
  testObj_.addWorker(w2);
  testObj_.addWorker(w3);

  testObj_.addNode(w0->getOutput());
  testObj_.addNode(w3->getInput());

  auto input = [&] {
    switch (input_count) {
      case 0:
        std::this_thread::sleep_for(milliseconds(10));
        CPPUNIT_ASSERT( testObj_.stateId() == fsm_state_t::Idle_s);
        input_count++;
        error_count = 500;
        std::cout << "\ninput (10ms): " << 2 << std::endl;
        return 2;

      case 1:
        std::this_thread::sleep_for(milliseconds(500));
        CPPUNIT_ASSERT( testObj_.stateId() == fsm_state_t::Failure_s);
        CPPUNIT_ASSERT( testObj_.getNThreads() == 0);
        CPPUNIT_ASSERT( w0->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w3->status() == proc_status_t::OK_s);

        CPPUNIT_ASSERT( w0->getOutput()->getNActiveProducers() == 0);
        CPPUNIT_ASSERT( w3->getInput()->getNActiveProducers() == 0);

        CPPUNIT_ASSERT( (w1->status() == proc_status_t::Error_s && w2->status() == proc_status_t::OK_s) ||
                        (w2->status() == proc_status_t::Error_s && w1->status() == proc_status_t::OK_s) );

        CPPUNIT_ASSERT( w0->getOutput()->status() == proc_status_t::Error_s);
        CPPUNIT_ASSERT( w1->getOutput()->status() == proc_status_t::Error_s);

//         CPPUNIT_ASSERT( testObj_.getStats()[3].process_count_ == 400);

        input_count++;
        i_count = 0;
        std::cout << "\ninput (8s): " << 4 << std::endl;
        return 4;

      case 2:
        std::this_thread::sleep_for(milliseconds(600));
        input_count++;
        error_count = 0;
        fatal_count = 600;
        std::cout << "\ninput (10ms): " << 2 << std::endl;
        return 2;

      default:
        std::this_thread::sleep_for(milliseconds(500));
        CPPUNIT_ASSERT( testObj_.stateId() == fsm_state_t::FatalError_s);
        CPPUNIT_ASSERT( testObj_.getNThreads() == 0);
        CPPUNIT_ASSERT( w0->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w1->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w2->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w3->status() == proc_status_t::FatalError_s);

        CPPUNIT_ASSERT( w0->getOutput()->getNActiveProducers() == 0);
        CPPUNIT_ASSERT( w1->getOutput()->getNActiveProducers() == 0);

        CPPUNIT_ASSERT( w0->getOutput()->status() == proc_status_t::OK_s);
        CPPUNIT_ASSERT( w3->getInput()->status() == proc_status_t::FatalError_s);

        CPPUNIT_ASSERT( testObj_.getStats()[3].process_count_ == 500);
        return 7;
    }
  };

  testObj_.start(input);
  CPPUNIT_ASSERT( testObj_.stateId() == fsm_state_t::FatalError_s);

  testObj_.dismissWorkers();
  testObj_.dismissNodes();
}
