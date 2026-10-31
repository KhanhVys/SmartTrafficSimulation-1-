# CMake generated Testfile for 
# Source directory: C:/Users/tangv/Documents/SmartTrafficSimulation-1-
# Build directory: C:/Users/tangv/Documents/SmartTrafficSimulation-1-/build-asan
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(TrafficRuleTest "C:/Users/tangv/Documents/SmartTrafficSimulation-1-/build-asan/TrafficRuleTest.exe")
set_tests_properties(TrafficRuleTest PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/tangv/Documents/SmartTrafficSimulation-1-/CMakeLists.txt;48;add_test;C:/Users/tangv/Documents/SmartTrafficSimulation-1-/CMakeLists.txt;0;")
add_test(CoreSimulationTest "C:/Users/tangv/Documents/SmartTrafficSimulation-1-/build-asan/CoreSimulationTest.exe")
set_tests_properties(CoreSimulationTest PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/tangv/Documents/SmartTrafficSimulation-1-/CMakeLists.txt;60;add_test;C:/Users/tangv/Documents/SmartTrafficSimulation-1-/CMakeLists.txt;0;")
add_test(IntegrationCoordinatorTest "C:/Users/tangv/Documents/SmartTrafficSimulation-1-/build-asan/IntegrationCoordinatorTest.exe")
set_tests_properties(IntegrationCoordinatorTest PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/tangv/Documents/SmartTrafficSimulation-1-/CMakeLists.txt;66;add_test;C:/Users/tangv/Documents/SmartTrafficSimulation-1-/CMakeLists.txt;0;")
add_test(SnapshotAdapterTest "C:/Users/tangv/Documents/SmartTrafficSimulation-1-/build-asan/SnapshotAdapterTest.exe")
set_tests_properties(SnapshotAdapterTest PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/tangv/Documents/SmartTrafficSimulation-1-/CMakeLists.txt;130;add_test;C:/Users/tangv/Documents/SmartTrafficSimulation-1-/CMakeLists.txt;0;")
subdirs("src/concurrency")
