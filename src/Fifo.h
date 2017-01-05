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

#ifndef _FIFO_H
#define _FIFO_H 1

// C++
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "Macros.h"
#include "Processor.h"

class Node {
public:
  Node();
  virtual ~Node() {}

  int  getNProducers() const;
  void setNProducers(int n);
  void addProducer();
  void rmProducer();

  int  getNConsummers() const;
  void setNConsummers(int n);
  void addConsummer();
  void rmConsummer();

  proc_status_t status();
  void setStatus(proc_status_t s) {status_.store(s);}

  void consummerError(proc_status_t e);
  void rmActiveProducer(proc_status_t e);
  void resetError();

  int  getNActiveProducers() const;
  void resetActiveProducers();

  void reset() {
    this->resetError();
    this->resetActiveProducers();
  }

  virtual size_t  size() =0;
  virtual void    clear() =0;
  virtual void    stop() =0;

  size_t  getMaxSize() const;
  void    setMaxSize(size_t ms);

protected:
  std::atomic_size_t max_size_;
  std::atomic_int n_producers_;
  std::atomic_int n_consummers_;
  std::atomic_int n_active_producers_;
  std::atomic<proc_status_t> status_;

  std::mutex mutex_;
  std::condition_variable cond_empty_;
  std::condition_variable cond_full_;
};

template <typename T>
class Fifo : public Node {
public:
  Fifo(std::size_t s=10);
  virtual ~Fifo() {}

  Fifo(const Fifo&)  = delete;  // disable copying
  Fifo(Fifo&&) = delete;  // disable moving

  Fifo& operator=(const Fifo&)  = delete; // disable assignment
  Fifo& operator=(Fifo&&) = delete;

  T pop();
  void push(const T& item);

  void rmActiveProducer(proc_status_t e);

  size_t         size()  override;
  virtual void   clear() override;
  virtual void   stop()  override {};

protected:
  std::queue<T> queue_;
};

template <typename T>
class PtrFifo : public Fifo<T> {
public:
  PtrFifo(std::size_t s=10) : Fifo<T>(s) {}
  virtual ~PtrFifo();

  PtrFifo(const PtrFifo&)  = delete;  // disable copying
  PtrFifo(PtrFifo&&) = delete;  // disable moving

  PtrFifo& operator=(const PtrFifo&)  = delete; // disable assignment
  PtrFifo& operator=(PtrFifo&&) = delete;

  void  clear() override;
  void  stop() override;
};

#include "Fifo-inl.h"

#endif

