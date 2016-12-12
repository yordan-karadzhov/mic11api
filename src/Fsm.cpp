
#include <chrono>
#include <iostream>

#include "Fsm.h"
#include "State.h"

using namespace std;
using namespace std::chrono;

ostream& operator<<(ostream& s, fsm_state_t state) {
  switch (state) {
    case fsm_state_t::Idle_s:
      s << "Idle";
      break;

    case fsm_state_t::Active_s:
      s << "Active";
      break;

    case fsm_state_t::Standby_s:
      s << "Standby";
      break;

    case fsm_state_t::Failure_s:
      s << "Failure";
      break;

    case fsm_state_t::FatalError_s:
      s << "FatalError";
      break;

    case fsm_state_t::Undefined_s:
      s << "Undefined";
      break;
  }
  return s;
}

ostream& operator<<(ostream& s, fsm_input_t i) {
  switch (i) {
    case fsm_input_t::Error_i:
      s << "Error";
      break;

    case fsm_input_t::On_i:
      s << "On";
      break;

    case fsm_input_t::Start_i:
      s << "Start";
      break;

    case fsm_input_t::Stop_i:
      s << "Stop";
      break;

    case fsm_input_t::Off_i:
      s << "Off";
      break;
  }
  return s;
}

State* StateFactory::newState(fsm_state_t s) {
  State* statePtr(nullptr);
  switch (s) {
    case fsm_state_t::Idle_s :
      statePtr = new Idle;
      break;

    case fsm_state_t::Active_s :
      statePtr = new Active;
      break;

    case fsm_state_t::Standby_s :
      statePtr = new Standby;
      break;

    case fsm_state_t::Failure_s :
      statePtr = new Failure;
      break;

    case fsm_state_t::FatalError_s :
      statePtr = new FatalError;
      break;

    case fsm_state_t::Undefined_s :
      break;
  }

  return statePtr;
}

Fsm::Fsm()
: factory_(new StateFactory),
  current_state_(factory_->newState(fsm_state_t::Idle_s)) {
    current_state_->whenEnteringStateDo(this, fsm_state_t::Idle_s);
  }

Fsm::~Fsm() {
  delete factory_;
}

void Fsm::start(userInputFunc input_func, int n) {
  n_iterations_ = n;
  future_status status;
  user_input_ = async(launch::async, input_func);
  while (1) {
    status = user_input_.wait_for(milliseconds(250));
    if (status == future_status::ready) {
      int choice = user_input_.get();

      if (choice < 1 || choice > 4) {
        this->close();
        exit(EXIT_SUCCESS);
      }

      const fsm_input_t input = static_cast<fsm_input_t>(choice);
      this->handleInput(input);
      user_input_ = async(launch::async, input_func);
    }

    this->handleMonitor();
  }
}

void Fsm::close() {
  current_state_->exit(this);
}

void Fsm::handleInput(fsm_input_t input) {
  current_state_->handleInput(this, input);
}

void Fsm::changeState(fsm_state_t state_id) {
//   lock_guard<mutex> lock(fsm_mtx_);
  cout << "   going from " << current_state_->id() << " to " << state_id << endl;

  current_state_->whenLeavingStateDo(this, state_id);
  auto old_state_id = current_state_->id();
  auto new_state = shared_ptr<State>( this->factory_->newState(state_id) );
//   auto new_state = unique_ptr<State>( this->factory_->newState(state_id) );
  current_state_ = move(new_state);
  current_state_->whenEnteringStateDo(this, old_state_id);
}

fsm_state_t Fsm::stateId() {
  return current_state_->id();
}

void Fsm::startWork() {
  if (threads_.size() != 0) {
    cerr << "FSM Error: unmanaged threads found when starting a new job!\n";
    this->changeState(fsm_state_t::Failure_s);
  }

  for (auto &w: workers_)
    threads_.push_back(thread(ref(*w), n_iterations_));
}

void Fsm::stopWork() {
  for (auto &w: workers_)
    w->closeInput();

  for (auto &n: nodes_)
    n->clear();

  this->joinWorkers();

  for (auto &n: nodes_)
    n->clear();
}

void Fsm::joinWorkers() {
  if (workers_.size() == 0)
    return;

  if (threads_.size() == 0) {
    cerr << "FSM Error: no active workers. Can not join.!\n";
    this->changeState(fsm_state_t::Failure_s);
  }

  for (auto &t: threads_)
    if (t.joinable())
      t.join();

  threads_.resize(0);
}

void Fsm::detachWorkrs() {
  if (workers_.size() == 0)
    return;

  if (threads_.size() == 0) {
    cerr << "FSM Error: no active workers. Can not detach.!\n";
    this->changeState(fsm_state_t::Failure_s);
  }

  for (auto &t: threads_)
    if (t.joinable())
      t.detach();

  threads_.resize(0);
}

void Fsm::handleMonitor() {
  if (this->stateId() != fsm_state_t::Active_s)
    return;

  proc_status_t workers_st = this->getWorkersStatus();
//   cerr << "handleMonitor " << workers_st << endl;
  if (workers_st != proc_status_t::OK_s) {
    for (auto &w: workers_) {
      if (w->status() != proc_status_t::OK_s)
        std::cerr << std::endl << w->status() << " from  " << w->name()
                  << " (id " << w->id() <<  ")\n";

    }

    switch (workers_st) {
      case proc_status_t::OK_s :
        break;

      case proc_status_t::Interrupt_s :
        this->changeState(fsm_state_t::Standby_s);
        break;

      case proc_status_t::Error_s :
        this->changeState(fsm_state_t::Failure_s);
        break;

      case proc_status_t::FatalError_s :
        this->changeState(fsm_state_t::FatalError_s);
        break;
    }
  }
}

proc_status_t Fsm::getNodesStatus() {
  proc_status_t st = proc_status_t::OK_s;
  for(auto &n: nodes_)
    st &= n->status();

  return st;
}

proc_status_t Fsm::getWorkersStatus() {
  proc_status_t st = proc_status_t::OK_s;
  for(auto &w: workers_)
    st &= w->status();

  return st;
}


