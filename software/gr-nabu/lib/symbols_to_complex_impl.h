/* -*- c++ -*- */
/*
 * Copyright 2024 Jared Boone.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_NABU_SYMBOLS_TO_COMPLEX_IMPL_H
#define INCLUDED_NABU_SYMBOLS_TO_COMPLEX_IMPL_H

#include <gnuradio/nabu/symbols_to_complex.h>

namespace gr {
namespace nabu {

class symbols_to_complex_impl : public symbols_to_complex
{
private:
    uint8_t _last;

public:
    symbols_to_complex_impl();
    ~symbols_to_complex_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace nabu
} // namespace gr

#endif /* INCLUDED_NABU_SYMBOLS_TO_COMPLEX_IMPL_H */
