
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
  current_state_(factory_->newState(fsm_state_t::Idle_s)),
  time_stats_enable_(false) {
    current_state_->whenEnteringStateDo(this, fsm_state_t::Idle_s);
  }

Fsm::~Fsm() {
  delete factory_;
  this->dismissWorkers();
}

int Fsm::addWorker(WInterface* w) {
  workers_.push_back(w);
  dynamic_workers_.push_back(false);
  return workers_.size();
}

void Fsm::dismissWorkers() {
  for (unsigned int i=0; i< workers_.size(); ++i)
    if(dynamic_workers_[i])
      delete workers_[i];

  workers_.resize(0);
}

int Fsm::addNode(std::shared_ptr<Node> n) {
  nodes_.push_back(n);
  return nodes_.size();
}

void Fsm::dismissNodes() {
  nodes_.resize(0);
}

void Fsm::close() {
  current_state_->exit(this);
}

void Fsm::handleInput(fsm_input_t input) {
  current_state_->handleInput(this, input);
}

void Fsm::changeState(fsm_state_t state_id) {
//   lock_guard<mutex> lock(fsm_mtx_);

  fsm_state_t old_state_id;
  if (current_state_) {
    current_state_->whenLeavingStateDo(this, state_id);
    old_state_id  = current_state_->id();
  } else {
    old_state_id = fsm_state_t::Undefined_s;
  }

  cout << "   going from " << old_state_id << " to " << state_id << endl;
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
    n->stop();

  this->joinWorkers();

//   for (auto &n: nodes_)
//     n->clear();
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

void Fsm::enableStats(bool e) {
  time_stats_enable_ = e;
  for(auto &w: workers_)
    w->enableStats(e);
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

void Fsm::updateStats() {
  if (time_stats_enable_) {
    stats_.resize(0);
    for (auto &w: workers_)
      stats_.push_back(w->getStats());
  }
}
