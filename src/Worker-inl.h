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

///////////////////// WOutput /////////////////////////////////

template <class outDataType>
WOutput<outDataType>::WOutput()
: output_(new outDataType*), fifo_out_(new Fifo<outDataType*> (FIFO_MAX_SIZE)) {
  fifo_out_->addProducer();
}

template <class outDataType>
WOutput<outDataType>::~WOutput() {
  if (*output_) {
    delete *output_;
    *output_ = nullptr;
  }
  delete output_;
}

template <class outDataType>
void WOutput<outDataType>::setOutFifoPtr(std::shared_ptr< Fifo<outDataType*> >& f) {
  fifo_out_ = f;
  fifo_out_->addProducer();
}
template <class outDataType>
void WOutput<outDataType>::push() {
  fifo_out_->push(*output_);
  *output_ = nullptr;
}

///////////////////// WInput /////////////////////////////////////

template <class inDataType>
WInput<inDataType>::WInput()
: input_(new inDataType*), fifo_in_(new Fifo<inDataType*> (FIFO_MAX_SIZE)) {
  fifo_in_->addConsummer();
}

template <class inDataType>
WInput<inDataType>::~WInput() {
  if (*input_) {
    delete *input_;
    *input_ = nullptr;
  }
  delete input_;
}

template <class inDataType>
void WInput<inDataType>::setInFifoPtr(std::shared_ptr< Fifo<inDataType*> >& f) {
  fifo_in_ = f;
  fifo_in_->addConsummer();
}

template <class inDataType>
bool WInput<inDataType>::pull() {
  *input_ = fifo_in_->pop();

  if (*input_)
    return true;

  return false;
}


///////////////////// InputWorker /////////////////////////

template <class outDataType>
void InputWorker<outDataType>::start(int n) {

  int i=0;
  while(i<n || n==0) {
    *(WOutput<outDataType>::output_) = new outDataType;
    if( !this->process() )
      this->stopWork();

    this->push();
    ++i;
  }

  this->stopWork();
}

template <class outDataType>
void InputWorker<outDataType>::stopWork() {
  WOutput<outDataType>::fifo_out_->rmProducer();
  this->close();
}


///////////////////// OutputWorker /////////////////////////

template <class inDataType>
void OutputWorker<inDataType>::start(int n) {

  int i=0;
  while(i<n || n==0) {
    if( !this->pull()) {
      this->stopWork();
      return;
    }

    if( !this->process() ) {
      this->stopWork();
      return;
    }

    delete *(WInput<inDataType>::input_);
    ++i;
  }

  this->stopWork();
}

template <class inDataType>
void OutputWorker<inDataType>::stopWork() {
  WInput<inDataType>::fifo_in_->rmConsummer();
  this->close();
}

/////////////////// UpdateWorker /////////////////////////////////////

template <class dataType>
void UpdateWorker<dataType>::start(int n) {

  int i=0;
  while(i<n || n==0) {
    if( !this->pull()) {
      this->stopWork();
      return;
    }

    if( !this->process() )
      this->stopWork();

    this->push();
    ++i;
  }

  this->stopWork();
}

template <class dataType>
void UpdateWorker<dataType>::stopWork() {
  WInput<dataType>::fifo_in_->rmConsummer();
  WOutput<dataType>::fifo_out_->rmProducer();
  this->close();
}

/////////////////// TransformWorker /////////////////////////////////////

template <class inDataType, class outDataType>
void TransformWorker<inDataType, outDataType>::start(int n) {

  int i=0;
  while(i<n || n==0) {
    if( !this->pull()) {
      this->stopWork();
      return;
    }

    *(WOutput<outDataType>::output_) = new outDataType;
    if( !this->process() )
      this->stopWork();

    this->push();
    ++i;
  }

  this->stopWork();
}

template <class inDataType, class outDataType>
void TransformWorker<inDataType, outDataType>::stopWork() {
  WInput<inDataType>::fifo_in_->rmConsummer();
  WOutput<outDataType>::fifo_out_->rmProducer();
  this->close();
}
