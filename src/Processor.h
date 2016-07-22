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

#ifndef BASETOOLS_PROCESSOR_H
#define BASETOOLS_PROCESSOR_H 1

// C++
#include <string>
#include <chrono>

#define GET_TIME std::chrono::high_resolution_clock::now()
#define GET_DURATION(t0) std::chrono::duration_cast<std::chrono::duration<double>>( \
std::chrono::high_resolution_clock::now()-t0).count()

typedef  std::chrono::high_resolution_clock::time_point  hd_time;

///////////////////// BaseProcessor //////////////////////////////////

class BaseProcessor {
public:
  BaseProcessor() = delete;  // Never use this default constructor!!!
  BaseProcessor(std::string n);
  virtual ~BaseProcessor() {}

  virtual void init(std::string) =0;
  virtual bool process() =0;
  virtual void close() =0;

  bool process_();

  std::string getName() const {return name_;}
  int getCount()        const {return processCount_;}
  void printTimeStats() const;

protected:
  std::string name_;
  int processCount_;
  double time_spent_;
};

///////////////////// InProcessor /////////////////////////////////////

template <class objOut>
class InProcessor : public BaseProcessor {

public:
  InProcessor() = delete;  // Never use this default constructor!!!
  InProcessor(std::string n) : BaseProcessor(n), output_(nullptr) {}

  virtual ~InProcessor() {};

  virtual void setOutputObj(objOut **o) {output_ = o;}

  objOut** getOutputObj() const {return output_;}
  objOut*  getOutputPtr() const {return *output_;}

protected:
  objOut **output_;
};

///////////////////// OutProcessor /////////////////////////////////////

template <class objIn>
class OutProcessor : public BaseProcessor {

public:
  OutProcessor() = delete;  // Never use this default constructor!!!
  OutProcessor(std::string n) : BaseProcessor(n), input_(nullptr) {}

  virtual ~OutProcessor() {};

  virtual void setInputObj(objIn  **i) {input_ = i;}

  objIn** getInputObj() const {return input_;}
  objIn*  getInputPtr() const {return *input_;}

protected:
  objIn  **input_;
};

///////////////////// InOutProcessor /////////////////////////////////////

template <class objIn, class objOut>
class InOutProcessor : public BaseProcessor {

public:
  InOutProcessor() = delete;  // Never use this default constructor!!!
  InOutProcessor(std::string n) : BaseProcessor(n), input_(nullptr), output_(nullptr) {}

  virtual ~InOutProcessor() {};

  virtual void setInputObj(objIn   **i) {input_ = i;}
  virtual void setOutputObj(objOut **o) {output_ = o;}

  objIn**  getInputObj()  const {return input_;}
  objIn*   getInputPtr()  const {return *input_;}

  objOut** getOutputObj() const {return output_;}
  objOut*  getOutputPtr() const {return *output_;}

protected:
  objIn  **input_;
  objOut **output_;
};

#endif

