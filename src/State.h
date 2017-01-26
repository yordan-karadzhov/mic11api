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

#ifndef _FSM_STATE_H
#define _FSM_STATE_H 1

#include <iostream>
#include <functional>
#include <memory>
#include <vector>
#include <thread>

#include "Worker.h"
#include "Fsm.h"
// class Fsm;

struct State {
  State() {}
  virtual ~State() {}

  virtual void handleInput(Fsm* sm, fsm_input_t i) =0;
  virtual void exit(Fsm* sm) =0;

  virtual fsm_state_t id() {return fsm_state_t::Undefined_s;}

  // To be redeffined by the user.
  virtual void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) =0;
  virtual void whenSameStateDo(Fsm *sm) =0;
  virtual void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) =0;
};


struct Idle: public State {
  Idle() {}
  virtual ~Idle() {}

  void handleInput(Fsm *sm, fsm_input_t i);
  void exit(Fsm* sm);
  fsm_state_t id() {return fsm_state_t::Idle_s;}

  // To be redeffined by the user.
  virtual void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) {}
  virtual void whenSameStateDo(Fsm *sm) {}
  virtual void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) {}
};


struct Standby : public State {
public:
  Standby() {}
  virtual ~Standby() {}

  void handleInput(Fsm *sm, fsm_input_t i);
  void exit(Fsm* sm);
  fsm_state_t id() {return fsm_state_t::Standby_s;}

  // To be redeffined by the user.
  virtual void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) override {
    for (auto &w: sm->getWorkers())
      w->init();
  }
  virtual void whenSameStateDo(Fsm *sm) {}
  virtual void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) {}
};


struct Active: public State {
  Active() {}
  virtual ~Active() {}

  void handleInput(Fsm *sm, fsm_input_t i);
  void exit(Fsm* sm);
  fsm_state_t id() {return fsm_state_t::Active_s;}

  // To be redeffined by the user.
  virtual void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) override {
    sm->startWork();
  }

  virtual void whenSameStateDo(Fsm *sm) {}
  virtual void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) override {
    sm->stopWork();
    if (new_state != fsm_state_t::Failure_s && new_state != fsm_state_t::FatalError_s) {
      for (auto &n: sm->getNodes()) {
        n->clear();
        n->reset();
      }
    }

    sm->updateStats();
    for (auto &w: sm->getWorkers())
      w->close();
  }
};


struct Failure : public State {
  Failure() {}
  virtual ~Failure() {}

  void handleInput(Fsm *sm, fsm_input_t i);
  void exit(Fsm* sm);
  fsm_state_t id() {return fsm_state_t::Failure_s;}

  // To be redeffined by the user.
  virtual void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) {}
  virtual void whenSameStateDo(Fsm *sm) {}
  virtual void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) override {
    for (auto &w: sm->getWorkers())
      w->setStatus(proc_status_t::OK_s);

    for (auto &n: sm->getNodes()) {
      n->clear();
      n->reset();
    }
  }
};


struct FatalError : public State {
  FatalError() {}
  virtual ~FatalError() {}

  void handleInput(Fsm *sm, fsm_input_t i);
  void exit(Fsm* sm);
  fsm_state_t id() {return fsm_state_t::FatalError_s;}

  // To be redeffined by the user.
  virtual void whenEnteringStateDo(Fsm *sm, fsm_state_t old_state) {}
  virtual void whenSameStateDo(Fsm *sm) {}
  virtual void whenLeavingStateDo(Fsm *sm, fsm_state_t new_state) {
    for (auto &n: sm->getNodes())
      n->clear();
  }
};

#endif


