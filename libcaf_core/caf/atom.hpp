/******************************************************************************
 *                       ____    _    _____                                   *
 *                      / ___|  / \  |  ___|    C++                           *
 *                     | |     / _ \ | |_       Actor                         *
 *                     | |___ / ___ \|  _|      Framework                     *
 *                      \____/_/   \_|_|                                      *
 *                                                                            *
 * Copyright (C) 2011 - 2015                                                  *
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

#ifndef CAF_ATOM_HPP
#define CAF_ATOM_HPP

#include <string>
#include <functional>
#include <type_traits>

#include "caf/detail/atom_val.hpp"

namespace caf {

/// The value type of atoms.
enum class atom_value : uint64_t {
  /// @cond PRIVATE
  dirty_little_hack = 31337
  /// @endcond
};

std::string to_string(const atom_value& x);

/// Creates an atom from given string literal.
template <size_t Size>
constexpr atom_value atom(char const (&str)[Size]) {
  // last character is the NULL terminator
  static_assert(Size <= 11, "only 10 characters are allowed");
  return static_cast<atom_value>(detail::atom_val(str));
}

/// Lifts an `atom_value` to a compile-time constant.
template <atom_value V>
struct atom_constant {
  constexpr atom_constant() {
    // nop
  }
  /// Returns the wrapped value.
  constexpr operator atom_value() const {
    return V;
  }
  static constexpr uint64_t uint_value() {
    return static_cast<uint64_t>(V);
  }
  /// Returns an instance *of this constant* (*not* an `atom_value`).
  static const atom_constant value;
};

template <class T>
struct is_atom_constant {
  static constexpr bool value = false;
};

template <atom_value X>
struct is_atom_constant<atom_constant<X>> {
  static constexpr bool value = true;
};

template <atom_value V>
std::string to_string(const atom_constant<V>&) {
  return to_string(V);
}

template <atom_value V>
const atom_constant<V> atom_constant<V>::value = atom_constant<V>{};

/// Used for request operations.
using add_atom = atom_constant<atom("add")>;

/// Used for request operations.
using get_atom = atom_constant<atom("get")>;

/// Used for request operations.
using put_atom = atom_constant<atom("put")>;

/// Used for signalizing updates, e.g., in a key-value store.
using update_atom = atom_constant<atom("update")>;

/// Used for request operations.
using delete_atom = atom_constant<atom("delete")>;

/// Used for response messages.
using ok_atom = atom_constant<atom("ok")>;

/// Used for triggering system-level message handling.
using sys_atom = atom_constant<atom("sys")>;

/// Used for signaling group subscriptions.
using join_atom = atom_constant<atom("join")>;

/// Used for signaling group unsubscriptions.
using leave_atom = atom_constant<atom("leave")>;

/// Used for signaling forwarding paths.
using forward_atom = atom_constant<atom("forward")>;

/// Used for buffer management.
using flush_atom = atom_constant<atom("flush")>;

/// Used for I/O redirection.
using redirect_atom = atom_constant<atom("redirect")>;

/// Used for link requests over network.
using link_atom = atom_constant<atom("link")>;

/// Used for removing networked links.
using unlink_atom = atom_constant<atom("unlink")>;

/// Used for publishing actors at a given port.
using publish_atom = atom_constant<atom("publish")>;

/// Used for removing an actor/port mapping.
using unpublish_atom = atom_constant<atom("unpublish")>;

/// Used for signalizing group membership.
using subscribe_atom = atom_constant<atom("subscribe")>;

/// Used for withdrawing group membership.
using unsubscribe_atom = atom_constant<atom("unsubscrib")>;

/// Used for establishing network connections.
using connect_atom = atom_constant<atom("connect")>;

/// Used for opening ports or files.
using open_atom = atom_constant<atom("open")>;

/// Used for closing ports or files.
using close_atom = atom_constant<atom("close")>;

/// Used for spawning remote actors.
using spawn_atom = atom_constant<atom("spawn")>;

/// Used for migrating actors to other nodes.
using migrate_atom = atom_constant<atom("migrate")>;

/// Used for triggering periodic operations.
using tick_atom = atom_constant<atom("tick")>;

} // namespace caf

namespace std {

template <>
struct hash<caf::atom_value> {
  size_t operator()(caf::atom_value x) const {
    hash<uint64_t> f;
    return f(static_cast<uint64_t>(x));
  }
};

} // namespace std

#endif // CAF_ATOM_HPP
