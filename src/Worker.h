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

#ifndef BASETOOLS_WORKER_H
#define BASETOOLS_WORKER_H 1

// C++
#include <iostream>
#include <mutex>
#include <memory>
#include <atomic>

// Mic11
#include "Fifo.h"
#include "Processor.h"

#define FIFO_MAX_SIZE 10

struct WStats {
  std::string worker_name_, processor_name_;
  int id_;
  int process_count_;
  double time_spent_;
};

///////////////////// WInterface ////////////////////////////////////
class WInterface {
public:
  WInterface(std::string n, PInterface *p, int id=0);
  virtual ~WInterface() {delete processor_;}

  WInterface(const WInterface&)  = delete;  // disable copying
  WInterface( WInterface&&) = delete;  // disable moving

  WInterface& operator=(const WInterface&)  = delete; // disable assignment
  WInterface& operator=(WInterface&&) = delete;

  virtual void start(int n=0);
  virtual void stop() =0;
  virtual void push() =0;
  virtual bool pull() =0;
  void operator () (int n=0) {this->start(n);}

//   void init(std::string s="");
  void           init(std::string s, void *atg=nullptr);
  proc_status_t  process();
  void           close();

  std::string    name()   const   {return name_;}
  int            id()     const   {return id_;}
  proc_status_t  status() const   {return status_.load();}

  void setStatus(proc_status_t s) {status_.store(s);}

  int     getCount() const   {return processor_->getCount();}
  void    enableStats()      {time_stats_enable_ = true;}
  void    closeInput();
  WStats  getStats()   const;
  void    printStats() const;
  void    printGooodBy();

protected:
  PInterface *processor_;

  std::atomic<proc_status_t> status_;

private:
  std::string name_;
  int  id_;
  bool time_stats_enable_;

protected:
  bool input_closed_;
};

///////////////////// WOutput /////////////////////////////////////
template <class outDataType>
class WOutput {
public:
  WOutput();
  virtual ~WOutput();

  std::shared_ptr< Fifo<outDataType*> >& getOutFifoPtr() {return fifo_out_;}
  std::shared_ptr< Fifo<outDataType*> >& getOutput()     {return fifo_out_;}

  void setOutFifoPtr(std::shared_ptr< Fifo<outDataType*> >& f);

protected:
  void push_();

  outDataType **output_;
  std::shared_ptr< Fifo<outDataType*> > fifo_out_;
};

///////////////////// WInput ///////////////////////////////////////
template <class inDataType>
class WInput {
public:
  WInput();
  virtual ~WInput();

  std::shared_ptr< Fifo<inDataType*> >&  getInFifoPtr()  {return fifo_in_;}
  std::shared_ptr< Fifo<inDataType*> >&  getInput()      {return fifo_in_;}

  void setInFifoPtr(std::shared_ptr< Fifo<inDataType*> >& f);

protected:
  bool pull_();

  inDataType  **input_;
  std::shared_ptr< Fifo<inDataType*> > fifo_in_;
};

///////////////////// InputWorker //////////////////////////////////
template <class outDataType>
class InputWorker
: public WInterface, public WOutput<outDataType> {
public:
  InputWorker() = delete;  // Never use this default constructor!!!
  InputWorker(std::string n, InProcessor<outDataType> *p, int id=0)
  : WInterface(n, p, id), WOutput<outDataType>() {
    p->setOutputObj( WOutput<outDataType>::output_ );
  }
  virtual ~InputWorker() {}

protected:
  bool pull() final;
  void push() final;
  void stop() final;
};

///////////////////// OutputWorker /////////////////////////////////
template <class inDataType>
class OutputWorker
: public WInterface, public WInput<inDataType> {
public:
  OutputWorker() = delete;  // Never use this default constructor!!!
  OutputWorker(std::string n, OutProcessor<inDataType> *p, int id=0)
  : WInterface(n, p, id), WInput<inDataType>() {
    p->setInputObj( WInput<inDataType>::input_ );
  }
  virtual ~OutputWorker() {}

protected:
  bool pull() final;
  void push() final;
  void stop() final;
};

///////////////// UpdateWorker /////////////////////////////////////
template <class dataType>
class UpdateWorker
: public WInterface, public WInput<dataType>, public WOutput<dataType> {
 public:
  UpdateWorker() = delete;  // Never use this default constructor!!!
  UpdateWorker(std::string n, InProcessor<dataType> *p, int id=0)
  : WInterface(n, p, id), WInput<dataType>(), WOutput<dataType>() {
    delete WOutput<dataType>::output_;
    WOutput<dataType>::output_ = WInput<dataType>::input_;
    p->setOutputObj( WOutput<dataType>::output_ );
  }

  virtual ~UpdateWorker() {
    WOutput<dataType>::output_ = nullptr;
  }

protected:
  bool pull() final;
  void push() final;
  void stop() final;
};

///////////////// TransformWorker //////////////////////////////////
template <class inDataType, class outDataType>
class TransformWorker
: public WInterface, public WInput<inDataType>, public WOutput<outDataType> {
 public:
  TransformWorker() = delete;  // Never use this default constructor!!!
  TransformWorker(std::string n, InOutProcessor<inDataType, outDataType> *p, int id=0)
  : WInterface(n, p, id), WInput<inDataType>(), WOutput<outDataType>() {
    p->setInputObj( WInput<inDataType>::input_ );
    p->setOutputObj( WOutput<outDataType>::output_ );
  }

  virtual ~TransformWorker() {}

protected:
  bool pull() final;
  void push() final;
  void stop() final;

};

#include "Worker-inl.h"
#endif

