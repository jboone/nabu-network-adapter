/* -*- c++ -*- */
/*
 * Copyright 2024 Jared Boone.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_NABU_DIFFERENTIAL_ENCODER_IMPL_H
#define INCLUDED_NABU_DIFFERENTIAL_ENCODER_IMPL_H

#include <gnuradio/nabu/differential_encoder.h>

namespace gr {
namespace nabu {

class differential_encoder_impl : public differential_encoder
{
private:
    uint8_t _last;
    
public:
    differential_encoder_impl();
    ~differential_encoder_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace nabu
} // namespace gr

#endif /* INCLUDED_NABU_DIFFERENTIAL_ENCODER_IMPL_H */
