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

#include "Fifo.h"
#include "Processor.h"

Node::Node() {
  n_producers_.store(0);
  n_active_producers_.store(0);
  n_consummers_.store(0);
  status_.store(proc_status_t::OK_s);
}

size_t Node::getMaxSize() const {
  return max_size_.load();
}

void Node::setMaxSize(size_t ms) {
  max_size_.store(ms);
}

int Node::getNProducers() const {
  return n_producers_.load();
}

void Node::setNProducers(int n) {
  n_producers_.store(n);
  n_active_producers_.store(n);
}

void Node::addProducer() {
  ++n_producers_;
  ++n_active_producers_;
}

void Node::rmProducer() {
  --n_producers_;
  --n_active_producers_;
}


int Node::getNActiveProducers() const {
  return n_active_producers_.load();
}

void Node::resetActiveProducers() {
  n_active_producers_.store( n_producers_.load() );
}

int Node::getNConsummers() const {
  return n_consummers_.load();
}

void Node::setNConsummers(int n) {
  n_consummers_.store(n);
}

void Node::addConsummer() {
  ++n_consummers_;
}

void Node::rmConsummer() {
  --n_consummers_;
}

proc_status_t Node::status() {
  return status_.load();
}

void Node::consummerError(proc_status_t e) {
//   --n_consummers_;
  if (status_ < e)
    status_.store(e);
}

void Node::rmActiveProducer(proc_status_t e)  {
  --n_active_producers_;
  if (status_ < e)
    status_.store(e);
}

void Node::resetError() {
  status_.store(proc_status_t::OK_s);
}


