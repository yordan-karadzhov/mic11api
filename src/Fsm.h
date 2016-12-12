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
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef _FSM_H
#define _FSM_H 1

#include <vector>
#include <thread>
#include <mutex>
#include <future>

#include "Worker.h"

class State;

enum class fsm_state_t {
  Idle_s=0,
  Active_s=1,
  Standby_s=2,
  Failure_s=3,
  FatalError_s=4,
  Undefined_s=5
};
std::ostream& operator<<(std::ostream& s, fsm_state_t state);

enum class fsm_input_t {
  Error_i=0,
  On_i=1,
  Start_i=2,
  Stop_i=3,
  Off_i=4
};
std::ostream& operator<<(std::ostream& s, fsm_input_t i);

class StateFactory {
public:
  StateFactory() {}
  virtual ~StateFactory() {}
  virtual State* newState(fsm_state_t s);
};

typedef int(*userInputFunc)(void);

class Fsm {
public:
  Fsm();
  virtual ~Fsm();

  template<typename F>
  void setUserFactory() {
    delete factory_;
    factory_ = new F;
    this->changeState(fsm_state_t::Idle_s);
  }

  void start(userInputFunc input_func, int n=0);
  void changeState( fsm_state_t state_id );

  void handleInput(fsm_input_t input); // delegate input handling to 'state'.
  void handleMonitor();

  fsm_state_t stateId();
  std::vector<WInterface*>&   getWorkers() { return workers_;}
  std::vector<Node*>&         getNodes()   { return nodes_;}

  void startWork();
  void stopWork();
  void close();

  void joinWorkers();
  void detachWorkrs();

  proc_status_t getNodesStatus();
  proc_status_t getWorkersStatus();

private:
  StateFactory *factory_;
  std::shared_ptr<State>    current_state_;
//   std::unique_ptr<State>    current_state_;
  std::vector<WInterface*>  workers_;
  std::vector<Node*>        nodes_;
  std::vector<std::thread>  threads_;

  std::future<int>          user_input_;
  int n_iterations_;
//   std::mutex fsm_mtx_;
};


#endif


