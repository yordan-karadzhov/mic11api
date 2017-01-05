

#include "Fsm.h"
#include "State.h"


void Idle::handleInput(Fsm *sm, fsm_input_t i) {
  switch (i) {
    case fsm_input_t::On_i:
      sm->changeState(fsm_state_t::Standby_s);
      break;

    case fsm_input_t::Off_i:
      std::cout << "   already Idle\n";
      this->whenSameStateDo(sm);
      break;

    case fsm_input_t::Start_i:
      sm->changeState(fsm_state_t::Standby_s);
      if (sm->stateId() == fsm_state_t::Standby_s)
        sm->changeState(fsm_state_t::Active_s);
      break;

    case fsm_input_t::Stop_i:
      sm->changeState(fsm_state_t::Standby_s);
      break;

    case fsm_input_t::Error_i:
      sm->changeState(fsm_state_t::Failure_s);
      break;
  }
}

void Idle::exit(Fsm* sm) {
  this->whenLeavingStateDo(sm, fsm_state_t::Undefined_s);
}

void Active::handleInput(Fsm *sm, fsm_input_t i) {
  switch (i) {
    case fsm_input_t::On_i:
      sm->changeState(fsm_state_t::Standby_s);
      break;

    case fsm_input_t::Off_i:
      sm->changeState(fsm_state_t::Standby_s);
      if (sm->stateId() == fsm_state_t::Standby_s)
        sm->changeState(fsm_state_t::Idle_s);
      break;

    case fsm_input_t::Start_i:
      std::cout << "   already Active\n";
      this->whenSameStateDo(sm);
      break;

    case fsm_input_t::Stop_i:
      sm->changeState(fsm_state_t::Standby_s);
      break;

    case fsm_input_t::Error_i:
      sm->changeState(fsm_state_t::Failure_s);
      break;
  }
}

void Active::exit(Fsm* sm) {
  sm->changeState(fsm_state_t::Standby_s);
  sm->close();
}

void Standby::handleInput(Fsm *sm, fsm_input_t i) {
  switch (i) {
    case fsm_input_t::On_i:
      std::cout << "   already Standby\n";
      this->whenSameStateDo(sm);
      break;

    case fsm_input_t::Off_i:
      sm->changeState(fsm_state_t::Idle_s);
      break;

    case fsm_input_t::Stop_i:
      this->whenSameStateDo(sm);
      break;

    case fsm_input_t::Start_i:
      sm->changeState(fsm_state_t::Active_s);
      break;

    case fsm_input_t::Error_i:
      sm->changeState(fsm_state_t::Failure_s);
      break;
  }
}

void Standby::exit(Fsm* sm) {
  sm->changeState(fsm_state_t::Idle_s);
  sm->close();
}

void Failure::handleInput(Fsm *sm, fsm_input_t i) {
  switch (i) {
    case fsm_input_t::Off_i:
      sm->changeState(fsm_state_t::Idle_s);
      break;

    default:
      std::cout << "   already Failure\n";
      this->whenSameStateDo(sm);
      break;
  }
}

void Failure::exit(Fsm* sm) {
  sm->changeState(fsm_state_t::Idle_s);
  sm->close();
}

void FatalError::handleInput(Fsm *sm, fsm_input_t i) {
  switch (i) {

    default:
      std::cout << " FatalError - exit\n";
      this->whenLeavingStateDo(sm, fsm_state_t::Undefined_s);
      std::exit(EXIT_FAILURE);
  }
}

void FatalError::exit(Fsm* sm) {}



