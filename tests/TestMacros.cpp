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

#include "TestMacros.h"

void TestMacros::setUp(void) {

}

void TestMacros::tearDown(void) {

}

IMPLEMENT_INPUT(I1, int, tproc_int1);

IMPLEMENT_USER_INPUT(I2, int, tproc_int1);
void I2::start(int n) {}

IMPLEMENT_UWORKER(UW1, int, tproc_int1);

IMPLEMENT_USER_UWORKER(UW2, int, tproc_int1);
void UW2::start(int n) {}

IMPLEMENT_TWORKER(TW1, int, std::string, tproc_str);

IMPLEMENT_USER_TWORKER(TW2, int, std::string, tproc_str);
void TW2::start(int n) {}
;

IMPLEMENT_OUTPUT(O1, int, tout_int);

IMPLEMENT_USER_OUTPUT(O2, int, tout_int);
void O2::start(int n) {}

void TestMacros::TestStreams(void) {

  I1   i1(0);
  I2   ie(1);
  UW1  uw1(2);
  UW2  uw2(3);
  TW1  tw1(4);
  TW2  tw2(5);
  O1   o1(6);
  O2   o2(7);
}
