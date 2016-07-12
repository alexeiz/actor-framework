/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright (C) 2011 - 2016                                                  *
 * Dominik Charousset <dominik.charousset (at) haw-hamburg.de>                *
 *                                                                            *
 * Distributed under the terms and conditions of the BSD 3-Clause License or  *
 * (at your option) under the terms and conditions of the Boost Software      *
 * License 1.0. See accompanying files LICENSE and LICENSE_ALTERNATIVE.       *
 *                                                                            *
 * If you did not receive a copy of the license files, see                    *
 * http://opensource.org/licenses/BSD-3-Clause and                            *
 * http://www.boost.org/LICENSE_1_0.txt.                                      *
 ******************************************************************************/

#include <poll.h>

#include <chrono>

#include "caf/config.hpp"

#define CAF_SUITE flare_actor
#include "caf/test/unit_test.hpp"

#include "caf/actor_system.hpp"
#include "caf/actor_system_config.hpp"
#include "caf/scoped_actor.hpp"
#include "caf/event_based_actor.hpp"

#include "caf/detail/flare_actor.hpp"

using namespace std;
using namespace caf;

using chrono::milliseconds;
using chrono::seconds;

namespace {

bool is_ready(detail::flare_actor* a, seconds secs = seconds::zero()) {
  pollfd p = {a->descriptor(), POLLIN, {}};
  auto n = ::poll(&p, 1, secs.count() * 1000);
  if (n < 0)
    terminate();
  return n == 1 && p.revents & POLLIN;
}

behavior dispatcher(event_based_actor* self, actor sink) {
  return [=](int i) {
    this_thread::sleep_for(milliseconds{100}); // simulate work
    self->send(sink, i);
  };
}

} // namespace <anonymous>

CAF_TEST(direct) {
  actor_system_config cfg;
  actor_system sys{cfg};
  scoped_actor self{sys};
  auto a = sys.spawn<detail::flare_actor>();
  auto f = actor_cast<detail::flare_actor*>(a);
  CAF_MESSAGE("one message");
  CAF_CHECK(!is_ready(f));
  self->send(a, 42);
  CAF_CHECK(is_ready(f));
  f->receive([&](int i) { CAF_CHECK_EQUAL(i, 42); });
  CAF_CHECK(!is_ready(f));
  CAF_MESSAGE("three messages");
  self->send(a, 42);
  self->send(a, 43);
  self->send(a, 44);
  CAF_CHECK(is_ready(f));
  f->receive([&](int i) { CAF_CHECK_EQUAL(i, 42); });
  CAF_CHECK(is_ready(f));
  f->receive([&](int i) { CAF_CHECK_EQUAL(i, 43); });
  CAF_CHECK(is_ready(f));
  f->receive([&](int i) { CAF_CHECK_EQUAL(i, 44); });
  CAF_CHECK(!is_ready(f));
}

CAF_TEST(indirect) {
  actor_system_config cfg;
  actor_system sys{cfg};
  scoped_actor self{sys};
  auto a = self->spawn<detail::flare_actor, linked>();
  auto b = self->spawn<linked>(dispatcher, a);
  auto c = self->spawn<linked>(dispatcher, b);
  auto d = self->spawn<linked>(dispatcher, c);
  auto f = actor_cast<detail::flare_actor*>(a);
  CAF_MESSAGE("one message");
  self->send(d, 42);
  CAF_CHECK(is_ready(f, seconds{1}));
  f->receive([&](int i) { CAF_CHECK_EQUAL(i, 42); });
  CAF_CHECK(!is_ready(f));
  CAF_MESSAGE("three messages");
  self->send(d, 42);
  self->send(d, 43);
  self->send(d, 44);
  CAF_CHECK(is_ready(f, seconds{1}));
  f->receive([&](int i) { CAF_CHECK_EQUAL(i, 42); });
  CAF_CHECK(is_ready(f));
  f->receive([&](int i) { CAF_CHECK_EQUAL(i, 43); });
  CAF_CHECK(is_ready(f));
  f->receive([&](int i) { CAF_CHECK_EQUAL(i, 44); });
  CAF_CHECK(!is_ready(f));
}
