/* -*- c++ -*- */
/*
 * Copyright 2024 Jared Boone.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_NABU_SYMBOLS_TO_COMPLEX_H
#define INCLUDED_NABU_SYMBOLS_TO_COMPLEX_H

#include <gnuradio/nabu/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace nabu {

/*!
 * \brief <+description of block+>
 * \ingroup nabu
 *
 */
class NABU_API symbols_to_complex : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<symbols_to_complex> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of nabu::symbols_to_complex.
     *
     * To avoid accidental use of raw pointers, nabu::symbols_to_complex's
     * constructor is in a private implementation
     * class. nabu::symbols_to_complex::make is the public interface for
     * creating new instances.
     */
    static sptr make();
};

} // namespace nabu
} // namespace gr

#endif /* INCLUDED_NABU_SYMBOLS_TO_COMPLEX_H */
