/* -*- c++ -*- */
/*
 * Copyright 2024 Jared Boone.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "differential_encoder_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace nabu {

using input_type = uint8_t;
using output_type = uint8_t;
differential_encoder::sptr differential_encoder::make()
{
    return gnuradio::make_block_sptr<differential_encoder_impl>();
}


/*
 * The private constructor
 */
differential_encoder_impl::differential_encoder_impl()
    : gr::sync_block("differential_encoder",
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
differential_encoder_impl::~differential_encoder_impl() {}

int differential_encoder_impl::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    auto in = static_cast<const input_type*>(input_items[0]);
    auto out = static_cast<output_type*>(output_items[0]);

    for(int i=0; i<noutput_items; i++) {
        // Differential encoder emits a "1" if the new bit differs from the last transmitted bit.
        const auto diff_enc = this->_last ^ in[i];
        this->_last = diff_enc;
        out[i] = diff_enc;
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace nabu */
} /* namespace gr */
