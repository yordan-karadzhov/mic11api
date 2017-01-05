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

///////////////////// WOutput /////////////////////////////////
template <class outDataType>
WOutput<outDataType>::WOutput()
: output_(new outDataType*), fifo_out_(new PtrFifo<outDataType*> (FIFO_MAX_SIZE)) {
  *output_ = nullptr;
  fifo_out_->addProducer();
}

template <class outDataType>
WOutput<outDataType>::~WOutput() {
  if (output_) {
    if (*output_) {
      delete *output_;
      *output_ = nullptr;
    }
    delete output_;
  }
}

template <class outDataType>
void WOutput<outDataType>::setOutFifoPtr(std::shared_ptr< PtrFifo<outDataType*> >& f) {
  fifo_out_ = f;
  fifo_out_->addProducer();
}
template <class outDataType>
void WOutput<outDataType>::push_() {
  fifo_out_->push(*output_);
  *output_ = nullptr;
}

///////////////////// WInput /////////////////////////////////////
template <class inDataType>
WInput<inDataType>::WInput()
: input_(new inDataType*), fifo_in_(new PtrFifo<inDataType*> (FIFO_MAX_SIZE)) {
  *input_ = nullptr;
  fifo_in_->addConsummer();
}

template <class inDataType>
WInput<inDataType>::~WInput() {
  if (input_) {
    if (*input_) {
      delete *input_;
      *input_ = nullptr;
    }
    delete input_;
  }
}

template <class inDataType>
void WInput<inDataType>::setInFifoPtr(std::shared_ptr< PtrFifo<inDataType*> >& f) {
  fifo_in_ = f;
  fifo_in_->addConsummer();
}

template <class inDataType>
bool WInput<inDataType>::pull_() {
  *input_ = fifo_in_->pop();

  if (*input_)
    return true;

  return false;
}

///////////////////// InputWorker /////////////////////////
template <class outDataType>
void InputWorker<outDataType>::push() {
//   std::cout << "\n" << this->name() << " (id " << this->id() <<  "): push.\n";
  this->push_();
}

template <class outDataType>
bool InputWorker<outDataType>::pull() {
//   std::cout << "\n" << this->name() << " (id " << this->id() <<  "): pull.\n";
  if (input_closed_)
    return false;

  *(WOutput<outDataType>::output_) = new outDataType;
  return true;
}

template <class outDataType>
void InputWorker<outDataType>::stop() {
  WOutput<outDataType>::fifo_out_->rmActiveProducer(status_);
  this->printGooodBy();
}

///////////////////// OutputWorker /////////////////////////
template <class inDataType>
bool OutputWorker<inDataType>::pull() {
//   std::cout << "\n" << this->name() << " (id " << this->id() <<  "): pull.\n";
  return this->pull_();
}

template <class inDataType>
void OutputWorker<inDataType>::push() {
//   std::cout << "\n" << this->name() << " (id " << this->id() <<  "): push.\n";
  delete *(WInput<inDataType>::input_);

}

template <class inDataType>
void OutputWorker<inDataType>::stop() {
  *WInput<inDataType>::input_ = nullptr;
  if (status_ == proc_status_t::Error_s || status_ == proc_status_t::FatalError_s)
    WInput<inDataType>::fifo_in_->consummerError(status_);
  this->printGooodBy();
}

/////////////////// UpdateWorker /////////////////////////////////////
template <class dataType>
void UpdateWorker<dataType>::push() {
//   std::cout << "\n" << this->name() << " (id " << this->id() <<  "): push.\n";
  this->push_();
}

template <class dataType>
bool UpdateWorker<dataType>::pull() {
//   std::cout << "\n" << this->name() << " (id " << this->id() <<  "): pull.\n";
  return this->pull_();
}

template <class dataType>
void UpdateWorker<dataType>::stop() {
  if (status_ == proc_status_t::Error_s || status_ == proc_status_t::FatalError_s)
    WInput<dataType>::fifo_in_->consummerError(status_);

  WOutput<dataType>::fifo_out_->rmActiveProducer(status_);

  this->printGooodBy();
}

/////////////////// TransformWorker /////////////////////////////////////
template <class inDataType, class outDataType>
void TransformWorker<inDataType, outDataType>::push() {
  this->push_();
  delete *(WInput<inDataType>::input_);
}

template <class inDataType, class outDataType>
bool TransformWorker<inDataType, outDataType>::pull() {
  *(WOutput<outDataType>::output_) = new outDataType;
  return this->pull_();
}

template <class inDataType, class outDataType>
void TransformWorker<inDataType, outDataType>::stop() {
  *WInput<inDataType>::input_ = nullptr;
  if (status_ == proc_status_t::Error_s || status_ == proc_status_t::FatalError_s)
    WInput<inDataType>::fifo_in_->consummerError(status_);

  WOutput<outDataType>::fifo_out_->rmActiveProducer(status_);

  this->printGooodBy();
}
