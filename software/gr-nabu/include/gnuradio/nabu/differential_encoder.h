/* -*- c++ -*- */
/*
 * Copyright 2024 Jared Boone.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_NABU_DIFFERENTIAL_ENCODER_H
#define INCLUDED_NABU_DIFFERENTIAL_ENCODER_H

#include <gnuradio/nabu/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace nabu {

/*!
 * \brief <+description of block+>
 * \ingroup nabu
 *
 */
class NABU_API differential_encoder : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<differential_encoder> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of nabu::differential_encoder.
     *
     * To avoid accidental use of raw pointers, nabu::differential_encoder's
     * constructor is in a private implementation
     * class. nabu::differential_encoder::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace nabu
} // namespace gr

#endif /* INCLUDED_NABU_DIFFERENTIAL_ENCODER_H */
