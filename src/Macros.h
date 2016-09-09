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

#ifndef MACROS_H
#define MACROS_H 1


#define DUMMY bool
#define STREAM_DOWN(dataType) \
void operator>> (std::shared_ptr< Fifo<dataType*> > &fifo) \
{ fifo = this->getOutFifoPtr(); fifo->addConsummer();}



#define STREAM_UP(dataType) \
void  operator<< (std::shared_ptr< Fifo<dataType*> > &fifo_out) \
{ this->setOutFifoPtr(fifo_out);}



#define IMPLEMENT_UWORKER(name, input, processor_type) \
class name : public UpdateWorker<input> { \
public: \
name (int id) : UpdateWorker(#name, new processor_type, id) {} \
STREAM_DOWN(input) \
STREAM_UP(input) \
};



#define IMPLEMENT_USER_UWORKER(name, input, processor_type) \
class name : public UpdateWorker<input> { \
public: \
name (int id) : UpdateWorker(#name, new processor_type, id) {} \
virtual void start(int n=0);\
STREAM_DOWN(input) \
STREAM_UP(input) \
};



#define IMPLEMENT_TWORKER(name, input, output, processor_type) \
class name : public TransformWorker<input, output> { \
public: \
name (int id) : TransformWorker(#name, new processor_type, id) {} \
STREAM_DOWN(output) \
STREAM_UP(output) \
};



#define IMPLEMENT_USER_TWORKER(name, input, output, processor_type) \
class name : public TransformWorker<input, output> { \
public: \
name (int id) : TransformWorker(#name, new processor_type, id) {} \
virtual void start(int n=0);\
STREAM_DOWN(output) \
STREAM_UP(output) \
};



#define IMPLEMENT_INPUT(name, output, processor_type) \
class name : public InputWorker<output> { \
public:\
name (int id) : InputWorker(#name, new processor_type, id) {} \
STREAM_DOWN(output) \
};



#define IMPLEMENT_USER_INPUT(name, output, processor_type) \
class name : public InputWorker<output> { \
public: \
name (int id) : InputWorker(#name, new processor_type, id) {} \
virtual void start(int n=0);\
STREAM_DOWN(output) \
};



#define IMPLEMENT_OUTPUT(name, input, processor_type) \
class name : public OutputWorker<input> { \
public: \
name (int id) : OutputWorker(#name, new processor_type, id) {} \
};



#define IMPLEMENT_USER_OUTPUT(name, input, processor_type) \
class name : public OutputWorker<input> { \
public: \
name (int id) : OutputWorker(#name, new processor_type, id) {} \
virtual void start(int n=0);\
};



#define JOB_START_N(worker, n) \
( [&] { worker.start(n); } )



#define JOB_START(worker) \
( [&] { worker.start(); } )

#endif

