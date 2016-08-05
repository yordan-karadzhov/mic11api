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

#ifndef BASETOOLS_NFIFO_H
#define BASETOOLS_NFIFO_H 1

// C++
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

template <typename T>
class Fifo {
public:
  Fifo(std::size_t s=6);
  Fifo(const Fifo&)  = delete;  // disable copying
  Fifo(Fifo&&) = delete;  // disable moving

  Fifo& operator=(const Fifo&)  = delete; // disable assignment
  Fifo& operator=(Fifo&&) = delete;

  T pop();
  void push(const T& item);

  void stopWork();

  size_t  size();
  int  getMaxSize() const     {return max_size_.load();}
  void setMaxSize(int ms)     {max_size_.store(ms);}

  int  getNProducers() const  {return n_producers_.load();}
  void setNProducers(int n)   {n_producers_.store(n);}
  void addProducer()          {++n_producers_;}
  void rmProducer();

  int  getNConsummers() const {return n_consummers_.load();}
  void setNConsummers(int n)  {n_consummers_.store(n);}
  void addConsummer()         {++n_consummers_;}
  void rmConsummer()          {--n_consummers_;}

private:
  std::queue<T> queue_;
  std::atomic_size_t max_size_;
  std::atomic_int    n_producers_;
  std::atomic_int    n_consummers_;

  std::mutex mutex_;
  std::condition_variable cond_empty_;
  std::condition_variable cond_full_;
};

#include "Fifo-inl.h"

#endif

