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

std::mutex mtx;

WInterface::WInterface(std::string n, BaseProcessor *p, int id)
: processor_(p), name_(n), id_(id), time_stats_enable_(false) {}

void WInterface::init(std::string s) {
  if (processor_) {
    std::lock_guard<std::mutex> lock(mtx);
    processor_->init(s);
  }
}

bool WInterface::process() {
  if (time_stats_enable_)
    return processor_->process_with_stats();

  return processor_->process();
}

void WInterface::close() {
  if (processor_) {
    std::lock_guard<std::mutex> lock(mtx);
    processor_->close();
    std::cout << "\n" << this->getName() << ": jobe done.\n";
  }
}

WStats WInterface::getStats() const {
  WStats s;
  s.worker_name_    = this->name_;
  s.processor_name_ = processor_->getName();
  s.id_             = this->id_;
  s.processCount_   = processor_->getCount();
  s.time_spent_     = processor_->getTime();

  return s;
}

void WInterface::printStats() const {
  std::cout << "\n++++++++++++ " << this->getName() << " Id:" 
            << this->getId() << " ++++++++++++\n\n";

  processor_->printTimeStats();
}
