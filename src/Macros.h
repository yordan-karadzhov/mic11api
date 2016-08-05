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

#define IMPLEMENT_UWORKER(name, input, processor_type) \
class name : public UpdateWorker<input> { \
public: \
name (int id) : UpdateWorker(#name, new processor_type, id) {} \
}; \
std::shared_ptr< Fifo<input*> > & operator>> (std::shared_ptr< Fifo<input*> > &fifo_in, name &worker) \
{ worker.setInFifoPtr(fifo_in); return fifo_in;} \
std::shared_ptr< Fifo<input*> > &  operator<< (name &worker, std::shared_ptr< Fifo<input*> > &fifo_out) \
{ worker.setOutFifoPtr(fifo_out); return fifo_out;}

#define IMPLEMENT_TWORKER(name, input, output, processor_type) \
class name : public TransformWorker<input, output> { \
public: \
name (int id) : TransformWorker(#name, new processor_type, id) {} \
}; \
std::shared_ptr< Fifo<input*> > & operator>> (std::shared_ptr< Fifo<input*> > &fifo_in, name &worker) \
{ worker.setInFifoPtr(fifo_in); return fifo_in;} \
std::shared_ptr< Fifo<output*> > &  operator<< (name &worker, std::shared_ptr< Fifo<output*> > &fifo_out) \
{ worker.setOutFifoPtr(fifo_out); return fifo_out;}

#define IMPLEMENT_INPUT(name, output, processor_type) \
class name : public InputWorker<output> { \
public: \
name (int id) : InputWorker(#name, new processor_type, id) {} \
};

#define IMPLEMENT_OUTPUT(name, input, processor_type) \
class name : public OutputWorker<input> { \
public: \
name (int id) : OutputWorker(#name, new processor_type, id) {} \
}; \
std::shared_ptr< Fifo<input*> > & operator>> (std::shared_ptr< Fifo<input*> > &fifo_in, name &worker) \
{ worker.setInFifoPtr(fifo_in); return fifo_in;}


#define JOB_STAR_N(worker, n) \
( [&] { worker.start(n); } )

#define JOB_STAR(worker) \
( [&] { worker.start(); } )

#endif

