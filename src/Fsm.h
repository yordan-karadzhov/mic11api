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

// typedef int(*inputFunc_t)(void);

class Fsm {
public:
  Fsm();
  virtual ~Fsm();

  template<typename factoryType>
  void setUserFactory();

  template<typename T>
  void start(T input_func, int n=0);
//   void start1(Fifo<int> *inputs, int n=0);

  template<typename workerType>
  workerType* addWorker();

  template<typename workerType>
  workerType* getWorker(int i);

  int  addWorker(WInterface *w);

  void dismissWorkers();

  int  addNode(std::shared_ptr<Node> n);
  void dismissNodes();

  void changeState( fsm_state_t state_id );

  void handleInput(fsm_input_t input); // delegate input handling to 'state'.
  void handleMonitor();

  fsm_state_t stateId();
  std::vector<WInterface*>                getWorkers() { return workers_;}
  std::vector< std::shared_ptr<Node> >    getNodes()   { return nodes_;}
  std::vector<WStats>&                    getStats()   { return stats_;}
  size_t getNThreads()  {return threads_.size();}

  void startWork();
  void stopWork();
  void close();

  void joinWorkers();
  void detachWorkrs();

  proc_status_t getNodesStatus();
  proc_status_t getWorkersStatus();

  void enableStats(bool e=true);
  void updateStats();

private:
  StateFactory *factory_;
  std::shared_ptr<State>    current_state_;
//   std::unique_ptr<State>    current_state_;
  std::vector<WInterface*>  workers_;
  std::vector<bool>         dynamic_workers_;

  std::vector< std::shared_ptr<Node> >  nodes_;
  std::vector<std::thread>  threads_;

  std::future<int>          user_input_;
  int n_iterations_;
  bool time_stats_enable_;
  std::vector<WStats> stats_;
//   std::mutex fsm_mtx_;
};

template<typename factoryType>
void Fsm::setUserFactory() {
  delete factory_;
  current_state_ = std::shared_ptr<State>(nullptr);
  factory_ = new factoryType;
  this->changeState(fsm_state_t::Idle_s);
}

template<typename T>
void Fsm::start(T input_func, int n) {
  n_iterations_ = n;
  std::future_status status;
  user_input_ = std::async(std::launch::async, input_func);
  while (1) {
    status = user_input_.wait_for(std::chrono::milliseconds(250));
    if (status == std::future_status::ready) {
      int choice = user_input_.get();

      if (choice < 1 || choice > 4) {
        this->close();
        return;
      }

      const fsm_input_t input = static_cast<fsm_input_t>(choice);
      this->handleInput(input);
      user_input_ = std::async(std::launch::async, input_func);
    }

    this->handleMonitor();
  }
}

template<typename workerType>
workerType* Fsm::addWorker() {
  unsigned int nWorkers = workers_.size();
  workerType *newWorker = new workerType(nWorkers);
  workers_.push_back(newWorker);
  dynamic_workers_.push_back(true);

  return newWorker;
}

template<typename workerType>
workerType* Fsm::getWorker(int i) {
  return dynamic_cast<workerType*>(workers_[i]);
}

#endif


