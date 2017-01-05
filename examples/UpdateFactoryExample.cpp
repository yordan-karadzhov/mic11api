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

// C++
#include <sstream>
#include <thread>
#include <chrono>
#include <mutex>
std::mutex mtx;

// Mic11
#include "Worker.h"
#include "Macros.h"

using namespace std;
using namespace std::chrono;

/** Define 3 dummy processors.
 *  The following interface must be implemented:
 *
 *   void init(string s) override;
 *   bool process() override;
 *   void close() override;
 */


/** This processor class is a producer.
 *  The output data type is int.
 */
class t_input : public InProcessor<int> {
 public:
  t_input(): InProcessor("t_input") {}

  void     init() override {}
  proc_status_t process() override;
  void     close(proc_status_t st) override {}
};

std::atomic_uint i_count(10);

proc_status_t t_input::process() {

  /// Simulate some work here.
  **output_ = ++i_count + 220;
  std::this_thread::sleep_for( milliseconds(5) );

  return proc_status_t::OK_s;
}


/** This processor class is a consumer and producer.
 *  The input data type is int and the output type is int.
 */
class t_proc : public InProcessor<int> {
 public:
  t_proc() : InProcessor("t_proc") {}

  void     init() override {}
  proc_status_t process() override;
  void     close(proc_status_t st) override {}
};

proc_status_t t_proc::process() {

  /// Simulate some work here.
  std::stringstream ss;
  **output_ -= 120;
  std::this_thread::sleep_for( milliseconds(3) );

  return proc_status_t::OK_s;
}


/** This processor is a consumer.
 *  The input data type is string.
 */
class t_output : public OutProcessor<int> {
 public:
  t_output() : OutProcessor("t_output") {}

  void     init() override {}
  proc_status_t process() override;
  void     close(proc_status_t st) override {}
};

proc_status_t t_output::process() {

  /// Simulate some work here.
  std::unique_lock<std::mutex> lck (mtx, std::defer_lock);
  lck.lock();
  std::cout << "output: " << **input_ << std::endl;
  lck.unlock();

  std::this_thread::sleep_for( milliseconds(6) );

  return proc_status_t::OK_s;
}

/// For each processor, define a worker.
IMPLEMENT_INPUT(t_input_worker, int, t_input)   // worker name, data type, proce name

IMPLEMENT_UWORKER(t_proc_worker, int, t_proc)   // worker name, data type, processor name

IMPLEMENT_OUTPUT(t_outputput_worker, int, t_output)   // worker name, data type, processor name

int main(int argc, char* argv[]) {

  /// Hire 4 workers.
  t_input_worker  w0(0);
  t_proc_worker   w1(1), w2(2);
  t_outputput_worker w3(3);


  /**Position the 4 workers.
  *
  *       |--> w1 -->|
  *       |          |
  *  w0-->|          |--> w3
  *       |          |
  *       |--> w2 -->|
  */

  /** The inputs of w1 and w2 are consumers of the output of w0.
   *  In this case the data buffer is originally owned by w0.
   */
  w0 >> w1.getInput();
  w0 >> w2.getInput();

  /** The input of w3 is a consumer of the outputs of w1 and w2.
   *  In this case the data buffer is originally owned by w3.
   */
  w1 << w3.getInput();
  w2 << w3.getInput();
//   w1 << ( w2 << w3.getInput() );

  vector<thread> threads;
  threads.push_back( thread(std::ref(w0), 100) ); // Start the work. 100 iterations.
//   threads.push_back( thread(std::ref(w0)) ); // Start the work. Run forever.
  threads.push_back( thread(std::ref(w1)) );
  threads.push_back( thread(std::ref(w2)) );
  threads.push_back( thread(std::ref(w3)) );

  for (auto &t:threads)
    t.join();

  return 0;
}

