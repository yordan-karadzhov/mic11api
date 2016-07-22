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

// Mic11
#include "Worker.h"

std::mutex mtx;

WInterface::WInterface(std::string n, BaseProcessor *p, int id)
: processor_(p), name_(n), id_(id) {}

void WInterface::init(std::string s) {
  if (processor_)
    processor_->init(s);
}

bool WInterface::process() {
  return processor_->process_();
}

void WInterface::close() {
  std::unique_lock<std::mutex> lck (mtx, std::defer_lock);
  lck.lock();
  std::cout << this->getName() << " ("
            << id_ << ") : job done." << std::endl;
  if (processor_) {
    processor_->close();
    processor_->printTimeStats();
  }
  std::cout << std::endl;
  lck.unlock();
}




