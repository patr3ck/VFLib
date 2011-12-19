// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_DETAIL_TYPE_CONVERSION_VFHEADER__
#define __VF_DB_DETAIL_TYPE_CONVERSION_VFHEADER__

#include "vf/db/type_conversion_traits.h"
#include "vf/db/detail/into_type.h"
#include "vf/db/detail/use_type.h"

namespace db {

namespace detail {

// used to order into and uses that need conversions
template<typename T>
struct base_value_holder
{
  typename type_conversion<T>::base_type m_value;
};

// create into_type from type_conversion
template <typename T>
class conversion_into_type
  : private base_value_holder<T>
  , public into_type<typename type_conversion<T>::base_type>
  , VF_NAMESPACE::NonCopyable
{
public:
  typedef typename type_conversion<T>::base_type BASE_TYPE;

  conversion_into_type(T& value)
    : into_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, m_own_ind)
    , m_value(value), m_ind(m_own_ind) {}

  conversion_into_type(T& value, indicator& ind)
    : into_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, ind)
    , m_value(value), m_ind(ind) {}

private:
  void convert_from_base()
  {
    type_conversion<T>::from_base(
      detail::base_value_holder<T>::m_value, m_ind, m_value);
  }

  T& m_value;

  indicator m_own_ind;
  indicator& m_ind;
};

// used to create a use_type from a type_conversion
template<typename T>
class conversion_use_type
  : private base_value_holder<T>
  , public use_type<typename type_conversion<T>::base_type>
{
public:
  typedef typename type_conversion<T>::base_type BASE_TYPE;

  conversion_use_type(T& value)
    : use_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, m_own_ind)
    , m_value(value), m_ind(m_own_ind), m_bReadOnly(false) {}

  conversion_use_type(T const& value)
    : use_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, m_own_ind)
    , m_value(const_cast<T&>(value)), m_ind(m_own_ind), m_bReadOnly(true) {}

  conversion_use_type(T& value, indicator& ind)
    : use_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, ind)
    , m_value(value), m_ind(ind), m_bReadOnly(false) {}

  conversion_use_type(T const& value, indicator& ind)
    : use_type<BASE_TYPE>(detail::base_value_holder<T>::m_value, ind)
    , m_value(const_cast<T&>(value)), m_ind(ind), m_bReadOnly(true) {}

  void convert_from_base()
  {
    if( !m_bReadOnly )
    {
      type_conversion<T>::from_base(
        detail::base_value_holder<T>::m_value, m_ind, m_value);
    }
  }

  void convert_to_base()
  {
    type_conversion<T>::to_base(m_value,
                                detail::base_value_holder<T>::m_value, m_ind);
  }

private:
  T& m_value;
  indicator m_own_ind;
  indicator& m_ind;
  bool m_bReadOnly;

  JUCE_DECLARE_NON_COPYABLE (conversion_use_type);
};

template <typename T>
into_type_ptr do_into(T& t, user_type_tag)
{
  return into_type_ptr(new conversion_into_type<T>(t));
}

template <typename T>
into_type_ptr do_into(T& t, indicator& ind, user_type_tag)
{
  return into_type_ptr(new conversion_into_type<T>(t, ind));
}

template <typename T>
use_type_ptr do_use(T& t, user_type_tag)
{
  return use_type_ptr(new conversion_use_type<T>(t));
}

template <typename T>
use_type_ptr do_use(T const& t, user_type_tag)
{
  return use_type_ptr(new conversion_use_type<T>(t));
}

template <typename T>
use_type_ptr do_use(T& t, indicator& ind, user_type_tag)
{
  return use_type_ptr(new conversion_use_type<T>(t, ind));
}

template <typename T>
use_type_ptr do_use(T const& t, indicator& ind, user_type_tag)
{
  return use_type_ptr(new conversion_use_type<T>(t, ind));
}

}

}

#endif
