/**
 * This file is part of the "libterminal" project
 *   Copyright (c) 2019-2020 Christian Parpart <christian@parpart.family>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#include <algorithm>

namespace crispy
{

// XXX Some C++20 backports

template <typename Container, typename Pred>
auto find_if(Container&& _container, Pred&& _pred)
{
    return std::find_if(begin(_container), end(_container), std::forward<Pred>(_pred));
}

template <typename Container, typename Fn>
constexpr bool any_of(Container&& _container, Fn&& _fn)
{
    return std::any_of(begin(_container), end(_container), std::forward<Fn>(_fn));
}

template <typename Container, typename Fn>
bool none_of(Container&& _container, Fn&& _fn)
{
    return std::none_of(begin(_container), end(_container), std::forward<Fn>(_fn));
}

template <typename ExecutionPolicy, typename Container, typename Fn>
bool any_of(ExecutionPolicy _ep, Container&& _container, Fn&& _fn)
{
    return std::any_of(_ep, begin(_container), end(_container), std::forward<Fn>(_fn));
}

template <typename Container, typename OutputIterator>
void copy(Container&& _container, OutputIterator _outputIterator)
{
    std::copy(std::begin(_container), std::end(_container), _outputIterator);
}

template <typename Container, typename Fn>
void for_each(Container&& _container, Fn&& _fn)
{
    std::for_each(begin(_container), end(_container), std::forward<Fn>(_fn));
}

template <typename ExecutionPolicy, typename Container, typename Fn>
void for_each(ExecutionPolicy _ep, Container&& _container, Fn&& _fn)
{
    std::for_each(_ep, begin(_container), end(_container), std::forward<Fn>(_fn));
}

template <typename Container, typename T>
auto count(Container&& _container, T&& _value)
{
    return std::count(begin(_container), end(_container), std::forward<T>(_value));
}

} // namespace crispy
