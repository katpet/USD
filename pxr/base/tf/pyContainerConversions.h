//
// Copyright 2016 Pixar
//
// Licensed under the terms set forth in the LICENSE.txt file available at
// https://openusd.org/license.
//
#ifndef PXR_BASE_TF_PY_CONTAINER_CONVERSIONS_H
#define PXR_BASE_TF_PY_CONTAINER_CONVERSIONS_H

/// \file tf/pyContainerConversions.h
/// Utilities for providing C++ <-> Python container support.

/*
 * Adapted (modified) from original at http://cctbx.sourceforge.net
 * Original file:
 * cctbx/scitbx/include/scitbx/boost_python/container_conversions.h
 * License:
 * http://cvs.sourceforge.net/viewcvs.py/cctbx/cctbx/
 *                                      LICENSE.txt?rev=1.2&view=markup
 */

#include "pxr/pxr.h"

#include "pxr/base/tf/refPtr.h"
#include "pxr/base/tf/weakPtr.h"
#include "pxr/base/tf/diagnostic.h"
#include "pxr/base/tf/iterator.h"
#include "pxr/base/tf/pyUtils.h"

#include "pxr/external/boost/python/list.hpp"
#include "pxr/external/boost/python/tuple.hpp"
#include "pxr/external/boost/python/extract.hpp"
#include "pxr/external/boost/python/to_python_converter.hpp"

#include <deque>
#include <list>
#include <set>
#include <utility>
#include <vector>

PXR_NAMESPACE_OPEN_SCOPE

// Converts any iterable C++ container to a Python list.
template <typename ContainerType>
struct TfPySequenceToPython
{
    static PyObject* convert(ContainerType const &c)
    {
        pxr_boost::python::list result;
        TF_FOR_ALL(i, c) {
            result.append(*i);
        }
        return pxr_boost::python::incref(result.ptr());
    }
};

// Converts any iterable C++ container to a Python set.
template <typename ContainerType>
struct TfPySequenceToPythonSet
{
    static PyObject* convert(ContainerType const &c)
    {
        PyObject* result = PySet_New(nullptr);
        for (const auto &elem : c) {
            PySet_Add(result, pxr_boost::python::object(elem).ptr());
        }
        return result;
    }
};

template <typename ContainerType>
struct TfPyMapToPythonDict
{
    static PyObject* convert(ContainerType const &c)
    {
        return pxr_boost::python::incref(TfPyCopyMapToDictionary(c).ptr());
    }    
};

namespace TfPyContainerConversions {

  template <typename ContainerType>
  struct to_tuple
  {
    static PyObject* convert(ContainerType const& a)
    {
      pxr_boost::python::list result;
      typedef typename ContainerType::const_iterator const_iter;
      for(const_iter p=a.begin();p!=a.end();p++) {
        result.append(pxr_boost::python::object(*p));
      }
      return pxr_boost::python::incref(pxr_boost::python::tuple(result).ptr());
    }
  };

  template <typename First, typename Second>
  struct to_tuple<std::pair<First, Second> > {
    static PyObject* convert(std::pair<First, Second> const& a)
    {
      pxr_boost::python::tuple result =
        pxr_boost::python::make_tuple(a.first, a.second);
      return pxr_boost::python::incref(result.ptr());
    }
  };

  template <typename... T>
  struct to_tuple<std::tuple<T...>> {
    static PyObject* convert(std::tuple<T...> const& a)
    {
      return std::apply(
        [](T const&... v) {
          pxr_boost::python::tuple result =
            pxr_boost::python::make_tuple(v...);
          return pxr_boost::python::incref(result.ptr());
        }, a);
    }
  };

  struct default_policy
  {
    static bool check_convertibility_per_element() { return false; }

    template <typename ContainerType>
    static bool check_size(ContainerType*, std::size_t sz)
    {
      return true;
    }

    template <typename ContainerType>
    static void assert_size(ContainerType*, std::size_t sz) {}

    template <typename ContainerType>
    static void reserve(ContainerType& a, std::size_t sz) {}
  };

  struct fixed_size_policy
  {
    static bool check_convertibility_per_element() { return true; }

    template <typename ContainerType>
    static bool check_size(ContainerType*, std::size_t sz)
    {
      return ContainerType::size() == sz;
    }

    template <typename ContainerType>
    static void assert_size(ContainerType* c, std::size_t sz)
    {
      if (!check_size(c, sz)) {
        PyErr_SetString(PyExc_RuntimeError,
          "Insufficient elements for fixed-size array.");
        pxr_boost::python::throw_error_already_set();
      }
    }

    template <typename ContainerType>
    static void reserve(ContainerType& a, std::size_t sz)
    {
      if (sz > ContainerType::size()) {
        PyErr_SetString(PyExc_RuntimeError,
          "Too many elements for fixed-size array.");
        pxr_boost::python::throw_error_already_set();
      }
    }

    template <typename ContainerType, typename ValueType>
    static void set_value(ContainerType& a, std::size_t i, ValueType const& v)
    {
      reserve(a, i+1);
      a[i] = v;
    }
  };

  struct variable_capacity_policy : default_policy
  {
    template <typename ContainerType>
    static void reserve(ContainerType& a, std::size_t sz)
    {
      a.reserve(sz);
    }

    template <typename ContainerType, typename ValueType>
    static void set_value(ContainerType& a, std::size_t i, ValueType const& v)
    {
      TF_AXIOM(a.size() == i);
      a.push_back(v);
    }
  };
  
  struct variable_capacity_all_items_convertible_policy : variable_capacity_policy
  {
      static bool check_convertibility_per_element() { return true; }
  };

  struct fixed_capacity_policy : variable_capacity_policy
  {
    template <typename ContainerType>
    static bool check_size(ContainerType*, std::size_t sz)
    {
      return ContainerType::max_size() >= sz;
    }
  };

  struct linked_list_policy : default_policy
  {
    template <typename ContainerType, typename ValueType>
    static void set_value(ContainerType& a, std::size_t i, ValueType const& v)
    {
      a.push_back(v);
    }
  };

  struct set_policy : default_policy
  {
    template <typename ContainerType, typename ValueType>
    static void set_value(ContainerType& a, std::size_t i, ValueType const& v)
    {
      a.insert(v);
    }
  };

  template <typename ContainerType, typename ConversionPolicy>
  struct from_python_sequence
  {
    typedef typename ContainerType::value_type container_element_type;

    from_python_sequence()
    {
      pxr_boost::python::converter::registry::push_back(
        &convertible,
        &construct,
        pxr_boost::python::type_id<ContainerType>());
    }

    static void* convertible(PyObject* obj_ptr)
    {
      if (!(   PyList_Check(obj_ptr)
            || PyTuple_Check(obj_ptr)
            || PySet_Check(obj_ptr)
            || PyFrozenSet_Check(obj_ptr)
            || PyIter_Check(obj_ptr)
            || PyRange_Check(obj_ptr)
            || (   !PyBytes_Check(obj_ptr)
                && !PyUnicode_Check(obj_ptr)
                && (   Py_TYPE(obj_ptr) == 0
                    || Py_TYPE(Py_TYPE(obj_ptr)) == 0
                    || Py_TYPE(Py_TYPE(obj_ptr))->tp_name == 0
                    || std::strcmp(
                         Py_TYPE(Py_TYPE(obj_ptr))->tp_name,
                         "Boost.Python.class") != 0)
                && PyObject_HasAttrString(obj_ptr, "__len__")
                && PyObject_HasAttrString(obj_ptr, "__getitem__")))) return 0;
      pxr_boost::python::handle<> obj_iter(
        pxr_boost::python::allow_null(PyObject_GetIter(obj_ptr)));
      if (!obj_iter.get()) { // must be convertible to an iterator
        PyErr_Clear();
        return 0;
      }
      if (ConversionPolicy::check_convertibility_per_element()) {
        Py_ssize_t obj_size = PyObject_Length(obj_ptr);
        if (obj_size < 0) { // must be a measurable sequence
          PyErr_Clear();
          return 0;
        }
        if (!ConversionPolicy::check_size(
          (ContainerType*)nullptr, obj_size)) return 0;
        bool is_range = PyRange_Check(obj_ptr);
        std::size_t i=0;
        if (!all_elements_convertible(obj_iter, is_range, i)) return 0;
        if (!is_range) assert(i == (std::size_t)obj_size);
      }
      return obj_ptr;
    }

    // This loop factored out by Achim Domma to avoid Visual C++
    // Internal Compiler Error.
    static bool
    all_elements_convertible(
      pxr_boost::python::handle<>& obj_iter,
      bool is_range,
      std::size_t& i)
    {
      for(;;i++) {
        pxr_boost::python::handle<> py_elem_hdl(
          pxr_boost::python::allow_null(PyIter_Next(obj_iter.get())));
        if (PyErr_Occurred()) {
          PyErr_Clear();
          return false;
        }
        if (!py_elem_hdl.get()) break; // end of iteration
        pxr_boost::python::object py_elem_obj(py_elem_hdl);
        pxr_boost::python::extract<container_element_type>
          elem_proxy(py_elem_obj);
        if (!elem_proxy.check()) return false;
        if (is_range) break; // in a range all elements are of the same type
      }
      return true;
    }

    static void construct(
      PyObject* obj_ptr,
      pxr_boost::python::converter::rvalue_from_python_stage1_data* data)
    {
      pxr_boost::python::handle<> obj_iter(PyObject_GetIter(obj_ptr));
      void* storage = (
        (pxr_boost::python::converter::rvalue_from_python_storage<ContainerType>*)
          data)->storage.bytes;
      new (storage) ContainerType();
      data->convertible = storage;
      ContainerType& result = *((ContainerType*)storage);
      std::size_t i=0;
      for(;;i++) {
        pxr_boost::python::handle<> py_elem_hdl(
          pxr_boost::python::allow_null(PyIter_Next(obj_iter.get())));
        if (PyErr_Occurred()) pxr_boost::python::throw_error_already_set();
        if (!py_elem_hdl.get()) break; // end of iteration
        pxr_boost::python::object py_elem_obj(py_elem_hdl);
        pxr_boost::python::extract<container_element_type> elem_proxy(py_elem_obj);
        ConversionPolicy::set_value(result, i, elem_proxy());
      }
      ConversionPolicy::assert_size((ContainerType*)nullptr, i);
    }
  };

  template <typename Indexes, typename TupleType, typename... T>
  struct from_python_tuple_impl;

  template <size_t... I, typename TupleType, typename... T>
  struct from_python_tuple_impl<std::index_sequence<I...>, TupleType, T...>
  {
    from_python_tuple_impl()
    {
      pxr_boost::python::converter::registry::push_back(
        &convertible,
        &construct,
        pxr_boost::python::type_id<TupleType>());
    }

    static void* convertible(PyObject* obj_ptr)
    {
      if (!PyTuple_Check(obj_ptr) || PyTuple_Size(obj_ptr) != sizeof...(T)) {
        return 0;
      }
      if ((!pxr_boost::python::extract<T>(
             PyTuple_GetItem(obj_ptr, I)).check() || ...)) {
        return 0;
      }
      return obj_ptr;
    }

    static void construct(
      PyObject* obj_ptr,
      pxr_boost::python::converter::rvalue_from_python_stage1_data* data)
    {
      void* storage = (
        (pxr_boost::python::converter::rvalue_from_python_storage<TupleType>*)
          data)->storage.bytes;
      new (storage) TupleType(
        pxr_boost::python::extract<T>(PyTuple_GetItem(obj_ptr, I))()...);
      data->convertible = storage;
    }
  };

  template <typename PairType>
  struct from_python_tuple_pair
    : from_python_tuple_impl<
        std::make_index_sequence<2>, PairType, 
        typename PairType::first_type, typename PairType::second_type
      >
  {
  };

  template <typename TupleType>
  struct from_python_tuple;

  template <typename... T>
  struct from_python_tuple<std::tuple<T...>>
    : from_python_tuple_impl<
        std::index_sequence_for<T...>, std::tuple<T...>, T...
      >
  {
  };

  template <typename ContainerType>
  struct to_tuple_mapping
  {
    to_tuple_mapping() {
      pxr_boost::python::to_python_converter<
        ContainerType,
        to_tuple<ContainerType> >();
    }
  };

  template <typename ContainerType, typename ConversionPolicy>
  struct tuple_mapping : to_tuple_mapping<ContainerType>
  {
    tuple_mapping() {
      from_python_sequence<
        ContainerType,
        ConversionPolicy>();
    }
  };

  template <typename ContainerType>
  struct tuple_mapping_fixed_size
  {
    tuple_mapping_fixed_size() {
      tuple_mapping<
        ContainerType,
        fixed_size_policy>();
    }
  };

  template <typename ContainerType>
  struct tuple_mapping_fixed_capacity
  {
    tuple_mapping_fixed_capacity() {
      tuple_mapping<
        ContainerType,
        fixed_capacity_policy>();
    }
  };

  template <typename ContainerType>
  struct tuple_mapping_variable_capacity
  {
    tuple_mapping_variable_capacity() {
      tuple_mapping<
        ContainerType,
        variable_capacity_policy>();
    }
  };

  template <typename ContainerType>
  struct tuple_mapping_set
  {
    tuple_mapping_set() {
      tuple_mapping<
        ContainerType,
        set_policy>();
    }
  };

  template <typename ContainerType>
  struct tuple_mapping_pair
  {
    tuple_mapping_pair() {
      pxr_boost::python::to_python_converter<
        ContainerType,
        to_tuple<ContainerType> >();
      from_python_tuple_pair<ContainerType>();
    }
  };

} // namespace TfPyContainerConversions

template <class T>
void TfPyRegisterStlSequencesFromPython()
{
    using namespace TfPyContainerConversions;
    from_python_sequence<
        std::vector<T>, variable_capacity_all_items_convertible_policy>();
    from_python_sequence<
        std::list<T>, variable_capacity_all_items_convertible_policy>();
    from_python_sequence<
        std::deque<T>, variable_capacity_all_items_convertible_policy>();
}

PXR_NAMESPACE_CLOSE_SCOPE

#endif // PXR_BASE_TF_PY_CONTAINER_CONVERSIONS_H
