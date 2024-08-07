#pragma once
// Copyright Takatoshi Kondo 2020
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#if !defined(ASYNC_MQTT_BROKER_RETAINED_MESSAGES_HPP)
#define ASYNC_MQTT_BROKER_RETAINED_MESSAGES_HPP

#include <functional>  // reference_wrapper

#include <broker/retain_type.hpp>
#include <broker/retained_topic_map.hpp>

namespace async_mqtt {

using retained_messages = retained_topic_map<retain_type>;

}  // namespace async_mqtt

#endif  // ASYNC_MQTT_BROKER_RETAINED_MESSAGES_HPP
