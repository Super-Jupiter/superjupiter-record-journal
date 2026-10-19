cmake_minimum_required(VERSION 3.16)
project(SuperJupiterRecordJournal VERSION 0.1.1 LANGUAGES CXX)

set(_superjupiter_record_journal_top_level OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(_superjupiter_record_journal_top_level ON)
endif()
option(SUPERJUPITER_RECORD_JOURNAL_BUILD_TESTS "Build RecordJournal tests" ${_superjupiter_record_journal_top_level})
option(SUPERJUPITER_RECORD_JOURNAL_BUILD_EXAMPLES "Build RecordJournal host example" ${_superjupiter_record_journal_top_level})
option(SUPERJUPITER_RECORD_JOURNAL_INSTALL "Generate RecordJournal install rules" ${_superjupiter_record_journal_top_level})

add_library(SuperJupiterRecordJournal INTERFACE)
add_library(SuperJupiter::RecordJournal ALIAS SuperJupiterRecordJournal)
set_target_properties(SuperJupiterRecordJournal PROPERTIES EXPORT_NAME RecordJournal)
target_include_directories(SuperJupiterRecordJournal INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>)
target_compile_features(SuperJupiterRecordJournal INTERFACE cxx_std_11)

if(SUPERJUPITER_RECORD_JOURNAL_BUILD_TESTS)
  enable_testing()
  add_executable(superjupiter-record-journal-review tests/review_regressions.cpp)
  target_link_libraries(superjupiter-record-journal-review PRIVATE SuperJupiter::RecordJournal)
  add_test(NAME superjupiter-record-journal-review COMMAND superjupiter-record-journal-review)
  add_executable(superjupiter-record-journal-tests tests/test.cpp)
  target_link_libraries(superjupiter-record-journal-tests PRIVATE SuperJupiter::RecordJournal)
  add_test(NAME superjupiter-record-journal-tests COMMAND superjupiter-record-journal-tests)
  add_executable(superjupiter-record-journal-namespace-test tests/namespace_isolation.cpp)
  target_link_libraries(superjupiter-record-journal-namespace-test PRIVATE SuperJupiter::RecordJournal)
  add_test(NAME superjupiter-record-journal-namespace-test COMMAND superjupiter-record-journal-namespace-test)
endif()
if(SUPERJUPITER_RECORD_JOURNAL_BUILD_EXAMPLES)
  add_executable(superjupiter-record-journal-replay tools/replay.cpp)
  target_link_libraries(superjupiter-record-journal-replay PRIVATE SuperJupiter::RecordJournal)
endif()
if(SUPERJUPITER_RECORD_JOURNAL_INSTALL)
  include(CMakePackageConfigHelpers)
  set(_superjupiter_record_journal_package_dir lib/cmake/SuperJupiterRecordJournal)
  install(TARGETS SuperJupiterRecordJournal EXPORT SuperJupiterRecordJournalTargets)
  install(FILES src/SuperJupiterRecordJournal.h DESTINATION include)
  install(EXPORT SuperJupiterRecordJournalTargets NAMESPACE SuperJupiter::
    DESTINATION ${_superjupiter_record_journal_package_dir})
  configure_package_config_file(cmake/SuperJupiterRecordJournalConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterRecordJournalConfig.cmake
    INSTALL_DESTINATION ${_superjupiter_record_journal_package_dir})
  write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterRecordJournalConfigVersion.cmake
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion ARCH_INDEPENDENT)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterRecordJournalConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/SuperJupiterRecordJournalConfigVersion.cmake
    DESTINATION ${_superjupiter_record_journal_package_dir})
  install(FILES LICENSE DESTINATION share/licenses/SuperJupiterRecordJournal)
endif()
