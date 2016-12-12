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

#ifndef BASETOOLS_PROCESSOR_H
#define BASETOOLS_PROCESSOR_H 1

// C++
#include <string>
#include <iostream>
#include <chrono>

#include "Macros.h"

enum class proc_status_t {
  OK_s=0,
  Interrupt_s=1,
  Error_s=2,
  FatalError_s=3
};

proc_status_t operator&(proc_status_t a, proc_status_t b);
proc_status_t operator&=(proc_status_t &a, proc_status_t b);
std::ostream& operator<<(std::ostream& s, proc_status_t b);

///////////////////// PInterface //////////////////////////////////
class PInterface {
public:
  PInterface() = delete;  // Never use this default constructor!!!
  PInterface(std::string n);
  virtual ~PInterface() {}

//   virtual void init(std::string) =0;
  virtual void init(std::string, void*) =0;
  virtual proc_status_t process() =0;
  virtual void close(proc_status_t st) =0;

  proc_status_t process_with_stats();

  std::string name() const {return name_;}
  int getCount()        const {return process_count_;}
  void resetStats()   {process_count_ = 0, time_spent_ = 0;}
  int getTime()         const {return time_spent_;}
  void printTimeStats() const;

protected:
  std::string name_;
  int process_count_;
  double time_spent_;
};

///////////////////// PInput /////////////////////////////////////
template <class objOut>
class PInput {
public:
  PInput() : output_(nullptr) {}
  virtual ~PInput() {}

  virtual void setOutputObj(objOut **o) {output_ = o;}

  objOut** getOutputObj() const {return output_;}
  objOut*  getOutputPtr() const {return *output_;}

protected:
  objOut **output_;
};

///////////////////// POutput /////////////////////////////////////
template <class objIn>
class POutput {
public:
  POutput() : input_(nullptr) {}
  virtual ~POutput() {}

  virtual void setInputObj(objIn  **i) {input_ = i;}

  objIn** getInputObj() const {return input_;}
  objIn*  getInputPtr() const {return *input_;}

protected:
  objIn  **input_;
};

///////////////////// InProcessor /////////////////////////////////////
template <class objOut>
class InProcessor : public PInterface, public PInput<objOut> {

public:
  InProcessor() = delete;  // Never use this default constructor!!!
  InProcessor(std::string n) : PInterface(n), PInput<objOut>() {}

  virtual ~InProcessor() {};
};

///////////////////// OutProcessor /////////////////////////////////////
template <class objIn>
class OutProcessor : public PInterface, public POutput<objIn> {

public:
  OutProcessor() = delete;  // Never use this default constructor!!!
  OutProcessor(std::string n) : PInterface(n), POutput<objIn>() {}

  virtual ~OutProcessor() {};
};

///////////////////// InOutProcessor /////////////////////////////////////
template <class objIn, class objOut>
class InOutProcessor : public PInterface, public PInput<objOut>, public POutput<objIn> {

public:
  InOutProcessor() = delete;  // Never use this default constructor!!!
  InOutProcessor(std::string n) : PInterface(n), PInput<objOut>(), POutput<objIn>() {}

  virtual ~InOutProcessor() {};
};

#endif

