/* -*- c++ -*- */
/*
 * Copyright 2024 Jared Boone.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "symbols_to_complex_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace nabu {

using input_type = uint8_t;
using output_type = gr_complex;
symbols_to_complex::sptr symbols_to_complex::make()
{
    return gnuradio::make_block_sptr<symbols_to_complex_impl>();
}


/*
 * The private constructor
 */
symbols_to_complex_impl::symbols_to_complex_impl()
    : gr::sync_block("symbols_to_complex",
                     gr::io_signature::make(
                         1 /* min inputs */, 1 /* max inputs */, sizeof(input_type)),
                     gr::io_signature::make(
                         1 /* min outputs */, 1 /*max outputs */, sizeof(output_type)))
{
    this->_last = 0;
}

/*
 * Our virtual destructor.
 */
symbols_to_complex_impl::~symbols_to_complex_impl() {}

int symbols_to_complex_impl::work(int noutput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

    for(int i=0; i<noutput_items; i++) {
        // Differential encoder emits a "1" if the new bit differs from the last transmitted bit.
        const auto diff_enc = this->_last ^ in[i];
        this->_last = diff_enc;

        // Pull apart the two bits in the symbol.
        const auto diff_enc_0 = (diff_enc >> 0) & 1;
        const auto diff_enc_1 = (diff_enc >> 1) & 1;

        // Encode the bits of the symbol into a complex (quadrature) vector.
        const float im = diff_enc_0 ? 1 : -1;
        const float re = diff_enc_1 ? 1 : -1;

        out[i] = gr_complex(re, im);
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace nabu */
} /* namespace gr */
