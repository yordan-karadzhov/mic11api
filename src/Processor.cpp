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

// Mic11Api
#include "Processor.h"

proc_status_t operator&(proc_status_t a, proc_status_t b) {
  proc_status_t s;
  int a_int = static_cast<int>(a);
  int b_int = static_cast<int>(b);

  s = (a_int > b_int)? a : b;
  return s;
}

proc_status_t operator&=(proc_status_t &a, proc_status_t b) {
  a = a & b;
  return a & b;
}

std::ostream& operator<<(std::ostream& s, proc_status_t status) {
  switch (status) {
    case proc_status_t::OK_s:
      s << "OK";
      break;

    case proc_status_t::Interrupt_s:
      s << "Interrupt";
      break;

    case proc_status_t::Error_s:
      s << "Error";
      break;

    case proc_status_t::FatalError_s:
      s << "FatalError";
      break;
  }
  return s;
}

PInterface::PInterface(std::string n)
: name_(n), process_count_(0), time_spent_(0.) {}

proc_status_t PInterface::process_with_stats() {
  hd_time t0 = GET_TIME;

  proc_status_t status = this->process();

  time_spent_ += GET_DURATION(t0);
  ++process_count_;

  return status;
}

void PInterface::printTimeStats() const {
  std::cout << name_ << "  -> Average processing time (" << process_count_ << "): "
            << 1e3*time_spent_ / process_count_ << " ms.\n";
}

