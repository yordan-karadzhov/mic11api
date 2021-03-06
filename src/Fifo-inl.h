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

template <typename T>
Fifo<T>::Fifo(std::size_t s) {
  max_size_ = s;
  n_producers_ = 0;
  n_active_producers_ = 0;
  n_consummers_ = 0;
}

template <typename T>
T Fifo<T>::pop() {
//   std::cerr << "@@@ pop\n";
  std::unique_lock<std::mutex> mlock(mutex_);
  cond_empty_.wait(mlock, [&](){ return !queue_.empty(); });
  T item = queue_.front();
//       std::cerr << "@@@ pop " << reinterpret_cast<void*>(item) << std::endl;
  queue_.pop();
  cond_full_.notify_one();

  return item;
}

template <typename T>
void Fifo<T>::push(const T& item) {
//   std::cerr << "@@@ push\n";
  std::unique_lock<std::mutex> mlock(mutex_);
  cond_full_.wait(mlock, [&](){ return queue_.size()<max_size_; });
//   std::cerr << "@@@ push " << reinterpret_cast<void*>(item) << std::endl;
  queue_.push(item);
  mlock.unlock();
  cond_empty_.notify_one();
}


template <typename T>
void Fifo<T>::rmActiveProducer(proc_status_t e) {
   --n_active_producers_;
   if (!n_active_producers_) {
     for (int i=0;i<n_consummers_;++i)
       this->push(nullptr);
   }

   if (status_ < e)
    status_.store(e);
}

template <typename T>
size_t  Fifo<T>::size() {
  std::lock_guard<std::mutex> lock(mutex_);
  int size = this->queue_.size();
  return size;
}

template <typename T>
void Fifo<T>::clear() {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    int size = this->queue_.size();
    for (int i=0;i<size;++i) {
      queue_.pop();
    }
  }

  this->reset();
  cond_full_.notify_all();
  cond_empty_.notify_all();
}

template <typename T>
PtrFifo<T>::~PtrFifo() {
  this->clear();
}

template <typename T>
void PtrFifo<T>::clear() {
//   std::cerr << "@@@ clear\n";
  std::lock_guard<std::mutex> lock(Fifo<T>::mutex_);
  int size = this->queue_.size();
  for (int i=0;i<size;++i) {
    T obj = Fifo<T>::queue_.front();
    Fifo<T>::queue_.pop();
    if (obj) delete obj;
  }

  this->reset();
  Fifo<T>::cond_full_.notify_all();
  Fifo<T>::cond_empty_.notify_all();
}

template <typename T>
void PtrFifo<T>::stop() {
//   std::cerr << "@@@ stop\n";
  {
    std::lock_guard<std::mutex> lock(Fifo<T>::mutex_);
    int size = this->queue_.size();
    for (int i=0;i<size;++i) {
      T obj = Fifo<T>::queue_.front();
      Fifo<T>::queue_.pop();
      if (obj) delete obj;
    }

    for (int i=0;i<Fifo<T>::n_consummers_;++i)
      Fifo<T>::queue_.push(static_cast<T>(nullptr));
  }

  Fifo<T>::cond_full_.notify_all();
  Fifo<T>::cond_empty_.notify_all();
}
