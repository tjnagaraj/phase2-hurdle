/* -*- c++ -*- */
/*
 * Copyright 2017 DARPA.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/* The code in this module is largely inspired by concepts from
 * https://github.com/osh/gr-eventstream
 */

#ifndef INCLUDED_ENVSIM_ENVSIM_SOURCE_IMPL_H
#define INCLUDED_ENVSIM_ENVSIM_SOURCE_IMPL_H

#include <envsim/envsim_source.h>
#include <algorithm>
//#include <cstring>
#include <sys/time.h>
#include <time.h>
#include <mutex>
#include <queue>

#include "iq_packet.h"

#define DEFAULT_QUEUE_SIZE 128
#define DEFAULT_RESIDUE_SIZE 4096
#define TIMESTAMP_COUNTS pmt::mp("timestamp_counts")
#define PACKET_PORT_ID pmt::mp("packets")

namespace gr {
namespace envsim {

class envsim_source_impl : public envsim_source {
 private:
  // add some variables used to convert between wall time
  // and sample counts
  int64_t d_start_time_s;
  int64_t d_start_time_ps;
  bool d_time_initialized;

  double d_sample_rate;

  int d_packet_counter;

  // add an event queue used to store incoming IQ packets.
  std::queue<pmt::pmt_t> d_packet_queue;

  // mutex used to protect access to d_packet_queue
  std::mutex d_packet_queue_mutex;

  // container to store samples that didn't fit in the output
  // buffer when handling a packet
  std::vector<std::complex<float> > d_residue;

  // convenience for getting at timestamp counts field of packet
  // metadata
  uint64_t packet_timestamp_counts(pmt::pmt_t iq_packet);
  int packet_length(pmt::pmt_t iq_packet);
  int packet_counter(pmt::pmt_t iq_packet);

  // update incoming packets with a timestamp_counts field
  void add_counts_timestamp(pmt::pmt_t pkt);

  void packet_handler(pmt::pmt_t pkt);

 public:
  envsim_source_impl(double sample_rate, int start_time_int_s,
                     double start_time_frac_s);
  ~envsim_source_impl();

  void set_start_time(int64_t start_time_s, int64_t start_time_ps);

  pmt::pmt_t start_time();

  // Where all the action really happens
  int work(int noutput_items, gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
};

}  // namespace envsim
}  // namespace gr

#endif /* INCLUDED_ENVSIM_ENVSIM_SOURCE_IMPL_H */
