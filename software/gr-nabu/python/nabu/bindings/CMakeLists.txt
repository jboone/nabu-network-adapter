# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT nabu_sources)
    message(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT nabu_sources)

########################################################################
# Check for pygccxml
########################################################################
gr_python_check_module_raw("pygccxml" "import pygccxml" PYGCCXML_FOUND)

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND nabu_python_files
    differential_encoder_python.cc
    symbols_to_complex_python.cc python_bindings.cc)

gr_pybind_make_oot(nabu ../../.. gr::nabu "${nabu_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(
    TARGET nabu_python
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:nabu_python>
            ${PROJECT_BINARY_DIR}/test_modules/gnuradio/nabu/)

install(
    TARGETS nabu_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/nabu
    COMPONENT pythonapi)
