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

#include "TestIntegration.h"

using namespace std;
using namespace std::chrono;

#define N 10000

int init_v(0);
proc_status_t t_input::process() {
  **output_ = ++init_v + 220;
  std::this_thread::sleep_for( microseconds(18) );
  if (**output_ == N + 240)
    return proc_status_t::Interrupt_s;

  return proc_status_t::OK_s;
}

proc_status_t t_proc_tr::process() {
  std::stringstream ss;
  ss << **input_ - 120;
  **output_ = ss.str();
  std::this_thread::sleep_for( microseconds(40) );
  if (**input_ == N + 340)
    return proc_status_t::Error_s;
  return proc_status_t::OK_s;
}

proc_status_t t_output_s::process() {
  std::this_thread::sleep_for( microseconds(21) );
  return proc_status_t::OK_s;
}

proc_status_t t_proc_up::process() {
  std::stringstream ss;
  **output_ -= 120;
  std::this_thread::sleep_for( microseconds(30) );
  if (**output_ == N + 220) {
//     std::cerr << "t_proc_up: " << **output_ << std::endl;
    return proc_status_t::Interrupt_s;
  }

  return proc_status_t::OK_s;
}

proc_status_t t_output_i::process() {
  std::this_thread::sleep_for( microseconds(15) );
  if (**input_ == N + 300)
    return proc_status_t::Interrupt_s;

  return proc_status_t::OK_s;
}

void TestIntegration::setUp() {}

void TestIntegration::tearDown() {}

void TestIntegration::TestUpdateJobs() {
  t_input_worker  w0(0);
  t_proc_up_worker   w1(1), w2(2);
  t_outputput_i_worker w3(3);

  w0.enableStats();
  w1.enableStats();
  w2.enableStats();
  w3.enableStats();

  w0 >> w1.getInput();
  w0 >> w2.getInput();
  w1 << w3.getInput();
  w2 << w3.getInput();

  vector<thread> threads;
  threads.push_back( thread(std::ref(w0), N) ); // Start the work. 100 iterations.
  threads.push_back( thread(std::ref(w1)) );
  threads.push_back( thread(std::ref(w2)) );
  threads.push_back( thread(std::ref(w3)) );

  for (auto &t:threads)
    t.join();

  CPPUNIT_ASSERT( w0.getCount() == N );
  CPPUNIT_ASSERT( (w1.getCount() + w2.getCount()) == N );
  CPPUNIT_ASSERT( w3.getCount() == N );
}

void TestIntegration::TestTransformJobs() {
  init_v = 0;
  t_input_worker  w0(0);
  t_proc_tr_worker   w1(1), w2(2);
  t_outputput_s_worker w3(3);

  w0.enableStats();
  w1.enableStats();
  w2.enableStats();
  w3.enableStats();

  w0 >> w1.getInput();
  w0 >> w2.getInput();
  w1 << w3.getInput();
  w2 << w3.getInput();

  vector<thread> threads;
  threads.push_back( thread(std::ref(w0), N) ); // Start the work. 100 iterations.
  threads.push_back( thread(std::ref(w1)) );
  threads.push_back( thread(std::ref(w2)) );
  threads.push_back( thread(std::ref(w3)) );

  for (auto &t:threads)
    t.join();

//   std::cout << " " << w0.getCount() << " " << w1.getCount() << " "
//             << w2.getCount() << " " << w3.getCount() << std::endl;

  CPPUNIT_ASSERT( w0.getCount() == N );
  CPPUNIT_ASSERT( (w1.getCount() + w2.getCount()) == N );
  CPPUNIT_ASSERT( w3.getCount() == N );
}

void TestIntegration::TestUpWorkerStop() {
  init_v = N;
  t_input_worker w0(0);
  t_proc_up_worker w1(1), w2(2);
  t_outputput_i_worker w3(3);

  std::vector<WInterface*> workers;
  workers.push_back(&w0);
  workers.push_back(&w1);
  workers.push_back(&w2);
  workers.push_back(&w3);

  for (auto &w: workers) {
    w->enableStats();
    w->init();
  }

  w0 >> w1.getInput();
  w0 >> w2.getInput();
  w1 << w3.getInput();
  w2 << w3.getInput();

  Node *n0 = w0.getOutput().get();
  Node *n1 = w3.getInput().get();
  n0->setMaxSize(50);
  n1->setMaxSize(50);

  vector<thread> threads;
  threads.push_back( thread(std::ref(w0), 50) ); // Start the work. 100 iterations.
  threads.push_back( thread(std::ref(w1)) );
  threads.push_back( thread(std::ref(w2)) );
  threads.push_back( thread(std::ref(w3)) );

  for (auto &t:threads)
    t.join();

//   std::cout << "1 " << w0.getCount() << " " << w1.getCount() << " "
//             << w2.getCount() << " " << w3.getCount() << std::endl;
//   std::cout << "fifos: " << n0->size() << " " << n1->size() << std::endl;

  CPPUNIT_ASSERT(  w0.getCount() == 20 );
  CPPUNIT_ASSERT( (w1.getCount() + w2.getCount()) == 19 );
  CPPUNIT_ASSERT(  w3.getCount() == 19 );

  CPPUNIT_ASSERT( w0.status() == proc_status_t::Interrupt_s);
  CPPUNIT_ASSERT( w1.status() == proc_status_t::OK_s);
  CPPUNIT_ASSERT( w2.status() == proc_status_t::OK_s);
  CPPUNIT_ASSERT( w3.status() == proc_status_t::OK_s);

  CPPUNIT_ASSERT( n0->status() == proc_status_t::Interrupt_s);
  CPPUNIT_ASSERT( n1->status() == proc_status_t::OK_s);
  CPPUNIT_ASSERT( n0->size() == 0);
  CPPUNIT_ASSERT( n1->size() == 0);

  init_v = N + 100;
  for (auto &w: workers)
    w->init();

  n0->reset();
  n1->reset();

  threads.resize(0);
  threads.push_back( thread(std::ref(w0), 50) );
  threads.push_back( thread(std::ref(w1)) );
  threads.push_back( thread(std::ref(w2)) );
  threads.push_back( thread(std::ref(w3)) );

  for (auto &t:threads)
    t.join();

//   std::cout << "2 " << w0.getCount() << " " << w1.getCount() << " "
//             << w2.getCount() << " " << w3.getCount() << std::endl;
//   std::cout << "fifos: " << n0->size() << " " << n1->size() << std::endl;

  CPPUNIT_ASSERT( w0.status() == proc_status_t::Interrupt_s);
  CPPUNIT_ASSERT( (w1.status() == proc_status_t::OK_s && w2.status() == proc_status_t::Interrupt_s) ||
                  (w2.status() == proc_status_t::OK_s && w1.status() == proc_status_t::Interrupt_s ) );
  CPPUNIT_ASSERT( w3.status() == proc_status_t::OK_s);

  CPPUNIT_ASSERT( n0->status() == proc_status_t::Interrupt_s);
  CPPUNIT_ASSERT( n1->status() == proc_status_t::Interrupt_s);
  CPPUNIT_ASSERT( n0->size() == 1);
  CPPUNIT_ASSERT( n1->size() == 0);

  auto obj = w0.getOutFifoPtr()->pop();
  CPPUNIT_ASSERT(obj == nullptr);

  init_v = N + 175;
  for (auto &w: workers)
    w->init();

  n0->reset();
  n1->reset();

  threads.resize(0);
  threads.push_back( thread(std::ref(w0), 50) );
  threads.push_back( thread(std::ref(w1)) );
  threads.push_back( thread(std::ref(w2)) );
  threads.push_back( thread(std::ref(w3)) );

  for (auto &t:threads)
    t.join();

//   std::cout << "3 " << w0.getCount() << " " << w1.getCount() << " "
//             << w2.getCount() << " " << w3.getCount() << std::endl;
//   std::cout << "fifos: " << n0->size() << " " << n1->size() << std::endl;

  CPPUNIT_ASSERT( w0.status() == proc_status_t::Interrupt_s);
  CPPUNIT_ASSERT( w1.status() == proc_status_t::OK_s);
  CPPUNIT_ASSERT( w2.status() == proc_status_t::OK_s);
  CPPUNIT_ASSERT( w3.status() == proc_status_t::Interrupt_s);

  CPPUNIT_ASSERT( n0->status() == proc_status_t::Interrupt_s);
  CPPUNIT_ASSERT( n1->status() == proc_status_t::OK_s);

  CPPUNIT_ASSERT( n0->size() == 0);
  CPPUNIT_ASSERT( n1->size() == 26);
  n1->clear();
  CPPUNIT_ASSERT( n1->size() == 0);
}

void TestIntegration::TestTrWorkerError() {
  init_v = N;
  t_input_worker w0(0);
  t_proc_tr_worker w1(1), w2(2);
  t_outputput_s_worker w3(3);

  std::vector<WInterface*> workers;
  workers.push_back(&w0);
  workers.push_back(&w1);
  workers.push_back(&w2);
  workers.push_back(&w3);

  for (auto &w: workers) {
    w->enableStats();
    w->init();
  }

  w0 >> w1.getInput();
  w0 >> w2.getInput();
  w1 << w3.getInput();
  w2 << w3.getInput();

  Node *n0 = w0.getOutput().get();
  Node *n1 = w3.getInput().get();
  n0->setMaxSize(50);
  n1->setMaxSize(50);

  vector<thread> threads;
  threads.push_back( thread(std::ref(w0), 50) ); // Start the work. 100 iterations.
  threads.push_back( thread(std::ref(w1)) );
  threads.push_back( thread(std::ref(w2)) );
  threads.push_back( thread(std::ref(w3)) );

  for (auto &t:threads)
    t.join();

//   std::cout << " " << w0.getCount() << " " << w1.getCount() << " "
//             << w2.getCount() << " " << w3.getCount() << std::endl;
//   std::cout << "fifos: " << n0->size() << " " << n1->size() << std::endl;

  CPPUNIT_ASSERT(  w0.getCount() == 20 );
  CPPUNIT_ASSERT( (w1.getCount() + w2.getCount()) == 19 );
  CPPUNIT_ASSERT(  w3.getCount() == 19 );

  CPPUNIT_ASSERT( w0.status() == proc_status_t::Interrupt_s);
  CPPUNIT_ASSERT( w1.status() == proc_status_t::OK_s);
  CPPUNIT_ASSERT( w2.status() == proc_status_t::OK_s);
  CPPUNIT_ASSERT( w3.status() == proc_status_t::OK_s);

  CPPUNIT_ASSERT( n0->status() == proc_status_t::Interrupt_s);
  CPPUNIT_ASSERT( n1->status() == proc_status_t::OK_s);
  CPPUNIT_ASSERT( n0->size() == 0);
  CPPUNIT_ASSERT( n1->size() == 0);

  init_v = N + 100;
  for (auto &w: workers)
    w->init();

  n0->reset();
  n1->reset();

  threads.resize(0);
  threads.push_back( thread(std::ref(w0), 50) ); // Start the work. 100 iterations.
  threads.push_back( thread(std::ref(w1)) );
  threads.push_back( thread(std::ref(w2)) );
  threads.push_back( thread(std::ref(w3)) );

  for (auto &t:threads)
    t.join();

//   std::cout << " " << w0.getCount() << " " << w1.getCount() << " "
//             << w2.getCount() << " " << w3.getCount() << std::endl;
//   std::cout << "fifos: " << n0->size() << " " << n1->size() << std::endl;

  CPPUNIT_ASSERT( w0.status() == proc_status_t::Interrupt_s);
  CPPUNIT_ASSERT( (w1.status() == proc_status_t::OK_s && w2.status() == proc_status_t::Error_s) ||
                  (w2.status() == proc_status_t::OK_s && w1.status() == proc_status_t::Error_s ) );
  CPPUNIT_ASSERT( w3.status() == proc_status_t::OK_s);

  CPPUNIT_ASSERT( n0->status() == proc_status_t::Error_s);
  CPPUNIT_ASSERT( n1->status() == proc_status_t::Error_s);
  CPPUNIT_ASSERT( n0->size() == 1);
  CPPUNIT_ASSERT( n1->size() == 0);

  auto obj = w0.getOutFifoPtr()->pop();
  CPPUNIT_ASSERT(obj == nullptr);


  init_v = N + 100;
  for (auto &w: workers)
    w->init();

  n0->reset();
  n1->reset();

  threads.resize(0);
  threads.push_back( thread(std::ref(w0), 150) );
  threads.push_back( thread(std::ref(w1)) );
  threads.push_back( thread(std::ref(w2)) );
  threads.push_back( thread(std::ref(w3)) );

  std::this_thread::sleep_for( milliseconds(300) );

  for (auto &t: threads)
    if (t.joinable())
      t.join();

//   std::cout << " " << w0.getCount() << " " << w1.getCount() << " "
//             << w2.getCount() << " " << w3.getCount() << std::endl;
//   std::cout << "fifos: " << n0->size() << " " << n1->size() << std::endl;
//   std::cout << "status: " << n0->status() << " " << n1->status() << std::endl;
}


