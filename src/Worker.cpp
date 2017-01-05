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

// Mic11
#include "Worker.h"

std::mutex w_mtx;

WInterface::WInterface(std::string n, PInterface *p, int id)
: processor_(p), status_(proc_status_t::OK_s), name_(n), id_(id),
  time_stats_enable_(false), input_closed_(false) {}

void WInterface::init() {
  if (processor_) {
    std::lock_guard<std::mutex> lock(w_mtx);
    input_closed_ = false;
    processor_->resetStats();
    status_ = proc_status_t::OK_s;
    processor_->init();
//     std::cout << "\n" << this->name() << " (id "
//               << this->id() <<  ") is ready to start.\n";
  }
}

proc_status_t WInterface::process() {
  if (time_stats_enable_)
    return processor_->process_with_stats();

  return processor_->process();
}

void WInterface::close() {
  if (processor_) {
    std::lock_guard<std::mutex> lock(w_mtx);
    processor_->close(status_);
  }
}

WStats WInterface::getStats() const {
  WStats s;
  s.worker_name_    = this->name_;
  s.processor_name_ = processor_->name();
  s.id_             = this->id_;
  s.process_count_   = processor_->getCount();
  s.time_spent_     = processor_->getTime();

  return s;
}

void WStats::print() {
  std::cout << "\n++++++++++++ " << this->worker_name_ << " Id:" 
            << this->id_ << " ++++++++++++\n";
  std::cout << this->processor_name_ << "  -> Average processing time ("
            << this->process_count_ << "): "
            << 1e3*this->time_spent_ / this->process_count_ << " ms.\n\n";
}

void WInterface::printStats() const {
  std::cout << "\n++++++++++++ " << this->name() << " Id:" 
            << this->id() << " ++++++++++++\n";

  processor_->printTimeStats();
  std::cout << "\n";
}

void WInterface::printGooodBy() {
//   std::lock_guard<std::mutex> lock(w_mtx);
// 
//   switch (status_) {
//     case proc_status_t::OK_s :
//       std::cout << "\n" << this->name() << " (id "
//                 << this->id() <<  "): jobe done.\n";
//       break;
// 
//     case proc_status_t::Interrupt_s :
//       std::cout << "\n" << this->name() << " (id "
//                 << this->id() <<  "): jobe interrupted!\n";
//       break;
// 
//     case proc_status_t::Error_s :
//       std::cout << "\n" << this->name() << " (id "
//                 << this->id() <<  "): job terminated with error!\n";
//       break;
// 
//     default :
//       std::cout << "\n" << this->name() << " (id "
//                 << this->id() <<  "): job terminated with fatal error!\n";
//   }
}

void WInterface::start(int n) {

  int i=0;
  while(1) {
    if (i >= n && n != 0) {
      status_ = proc_status_t::Interrupt_s;
      break;
    }

    if( !this->pull()) {
      break;
    }

    status_ = this->process();
    if(status_ != proc_status_t::OK_s) {
      break;
    }

    this->push();
    ++i;
  }

  this->stop();
}

void WInterface::closeInput() {
  std::lock_guard<std::mutex> lock(w_mtx);
  input_closed_ = true;
}


