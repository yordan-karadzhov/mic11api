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

// C++
#include <iostream>

// CppUnit
#include <cppunit/TestCase.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>

// Mic11
#include "tests/TestFifo.h"
#include "tests/TestProcessor.h"
#include "tests/TestWorker.h"
#include "tests/TestMacros.h"

CPPUNIT_TEST_SUITE_REGISTRATION(TestNWorker);
CPPUNIT_TEST_SUITE_REGISTRATION(TestNProcessor);
CPPUNIT_TEST_SUITE_REGISTRATION(TestNFifo);
CPPUNIT_TEST_SUITE_REGISTRATION(TestMacros);

int main(int argc, char* argv[]) {
  using namespace CppUnit;

  // Informs test-listener about testresults.
  CPPUNIT_NS::TestResult testresult;

  // Register listener for collecting the test-results.
  CPPUNIT_NS::TestResultCollector collectedresults;
  testresult.addListener (&collectedresults);

  // Register listener for per-test progress output.
  CPPUNIT_NS::BriefTestProgressListener progress;
  testresult.addListener (&progress);

  // Insert test-suite at test-runner by registry.
  CPPUNIT_NS::TestRunner testrunner;
  testrunner.addTest (CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest ());
  testrunner.run(testresult);

  // Output results in compiler-format.
  CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
  compileroutputter.write ();

  // Output XML for Jenkins CPPunit plugin.
//   std::ofstream xmlFileOut("Mic11_cppUnitTests.xml");
//   XmlOutputter xmlOut(&collectedresults, xmlFileOut);
//   xmlOut.write();

  // Return 0 if tests were successful.
  bool success = collectedresults.wasSuccessful();
  return  success ? 0 : 1;
}
