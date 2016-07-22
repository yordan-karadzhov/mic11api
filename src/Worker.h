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

#ifndef BASETOOLS_WORKER_H
#define BASETOOLS_WORKER_H 1

// C++
#include <iostream>
#include <mutex>
#include <memory>

// Mic11
#include "Fifo.h"
#include "Processor.h"

#define FIFO_MAX_SIZE 10


///////////////////// WInterface ////////////////////////////////////

class WInterface {
public:
  WInterface() = delete;  // Never use this default constructor!!!
  WInterface(std::string n, BaseProcessor *p, int id=0);

  virtual ~WInterface() {delete processor_;}

  void init(std::string s="");
  virtual void start(int n=0) =0;

  void close();
  bool process();

  std::string  getName() const {return name_;}
  int          getId()   const {return id_;}
  void         printTimeStats() const;

protected:
  BaseProcessor *processor_;

private:
  std::string name_;
  int id_;
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
  void push();

  outDataType **output_;
  std::shared_ptr< Fifo<outDataType*> > fifo_out_;
};

///////////////////// WInput /////////////////////////////////////

template <class inDataType>
class WInput /*: public WCore */{
public:
  WInput();
  virtual ~WInput();

  std::shared_ptr< Fifo<inDataType*> >&  getInFifoPtr()  {return fifo_in_;}
  std::shared_ptr< Fifo<inDataType*> >&  getInput()      {return fifo_in_;}

  void setInFifoPtr(std::shared_ptr< Fifo<inDataType*> >& f);

protected:
  bool pull();

  inDataType  **input_;
  std::shared_ptr< Fifo<inDataType*> > fifo_in_;
};

///////////////////// InputWorker /////////////////////////

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

  void start(int n=0) final;

private:
  void stopWork();
};

///////////////////// OutputWorker /////////////////////////

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

  void start(int n=0) final;

private:
  void stopWork();
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

  virtual ~UpdateWorker() {}

  void start(int n=0) final;

private:
  void stopWork();
};

///////////////// TransformWorker /////////////////////////////////////

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

  void start(int n=0) final;

private:
  void stopWork();
};

#include "Worker-inl.h"
#endif

