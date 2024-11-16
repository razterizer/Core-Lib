//
//  StlUtils.h
//  Core
//
//  Created by Rasmus Anthin on 2022-11-19.
//

#pragma once
#include "Math.h"
#include <numeric>
#include <cstring>
#include <assert.h>
#include <algorithm>
#include <functional>
#include <type_traits>


namespace stlutils
{

  template<typename Cont>
  void memset(Cont& c, typename Cont::value_type val)
  {
    if constexpr (std::is_trivial_v<typename Cont::value_type>)
      std::memset(c.data(), val, c.size() * sizeof(typename Cont::value_type));
    else
      std::fill(c.begin(), c.end(), val);
  }

  template<typename Cont>
  void memclr(Cont& c)
  {
    memset(c, static_cast<typename Cont::value_type>(0));
  }

  template<typename Cont>
  typename Cont::value_type sum(const Cont& c)
  {
    return std::accumulate(c.begin(), c.end(), static_cast<typename Cont::value_type>(0));
  }
  
  template<typename T, typename Cont, typename Lambda>
  T sum(const Cont& c, Lambda unary_op)
  {
    T sum_val = static_cast<T>(0);
    for (const auto& v : c)
      sum_val += unary_op(v);
    return sum_val;
  }

  template<typename T>
  T sum(const std::vector<std::vector<T>>& v)
  {
    auto ret = static_cast<T>(0);
    for (const auto& r : v)
      ret += sum(r);
    return ret;
  }

  template<typename Cont, typename RetT>
  RetT average(const Cont& c)
  {
    if (c.empty())
      return static_cast<RetT>(0);
    auto N = c.size();
    auto s = sum(c);
    return static_cast<RetT>(s)/static_cast<RetT>(N);
  }
  
  template<typename Cont>
  typename Cont::value_type prod(const Cont& c)
  {
    if (c.empty())
      return static_cast<typename Cont::value_type>(1);
    return std::accumulate(std::begin(c), std::end(c),
      static_cast<typename Cont::value_type>(1),
      std::multiplies<typename Cont::value_type>());
  }

  template<typename Cont>
  Cont comp_prod(const Cont& cA, const Cont& cB)
  {
    assert(cA.size() == cB.size());
    if (cA.size() != cB.size())
      return {};
    Cont ret = cB;
    std::transform(cA.begin(), cA.end(), cB.begin(), ret.begin(), std::multiplies<typename Cont::value_type>());
    return ret;
  }
  
  template<typename Cont>
  Cont comp_sq(const Cont& c)
  {
    return comp_prod(c, c);
  }

  template<typename T>
  std::vector<std::vector<T>> comp_prod(const std::vector<std::vector<T>>& vA,
                                        const std::vector<std::vector<T>>& vB)
  {
    auto Nar = vA.size();
    auto Nac = vA.back().size();
    auto Nbr = vB.size();
    auto Nbc = vB.back().size();
    assert(Nar == Nbr);
    assert(Nac == Nbc);
    if (Nar != Nbr || Nac != Nbc)
      return {};
    std::vector<std::vector<T>> ret = vA; // Resizing by assignment.
    for (size_t r_idx = 0; r_idx < Nar; ++r_idx)
    {
      const auto& vA_r = vA[r_idx];
      const auto& vB_r = vB[r_idx];
      auto& ret_r = ret[r_idx];
      for (size_t c_idx = 0; c_idx < Nac; ++c_idx)
        ret_r[c_idx] = vA_r[c_idx] * vB_r[c_idx];
    }
    return ret;
  }

  template<typename Cont>
  Cont mult_scalar(const Cont& c, typename Cont::value_type s)
  {
    Cont ret = c;
    for (auto& v : ret)
      v *= s;
    return ret;
  }

  template<typename T>
  std::vector<std::vector<T>> mult_scalar(const std::vector<std::vector<T>>& vec, T s)
  {
    std::vector<std::vector<T>> ret = vec;
    for (auto& row : ret)
      for (auto& v : row)
        v *= s;
    return ret;
  }
  
  template<typename Cont>
  Cont comp_div(const Cont& cA, const Cont& cB)
  {
    assert(cA.size() == cB.size());
    if (cA.size() != cB.size())
      return {};
    Cont ret = cB;
    std::transform(cA.begin(), cA.end(), cB.begin(), ret.begin(), std::divides<typename Cont::value_type>());
    return ret;
  }
  
  template<typename Cont>
  Cont scalar_div(typename Cont::value_type s, const Cont& c)
  {
    Cont ret = c;
    for (auto& v : ret)
      v = s / v;
    return ret;
  }

  template<typename Cont>
  Cont add(const Cont& cA, const Cont& cB)
  {
    assert(cA.size() == cB.size());
    if (cA.size() != cB.size())
      return {};
    Cont ret = cB;
    std::transform(cA.begin(), cA.end(), cB.begin(), ret.begin(), std::plus<typename Cont::value_type>());
    return ret;
  }

  template<typename T>
  std::vector<std::vector<T>> add(const std::vector<std::vector<T>>& vA,
                                  const std::vector<std::vector<T>>& vB)
  {
    auto Nra = vA.size();
    auto Nca = vA.back().size();
    auto Nrb = vB.size();
    auto Ncb = vB.back().size();
    auto Nr = Nra;
    auto Nc = Nca;
    assert(Nra == Nrb);
    assert(Nca == Ncb);
    if (vA.size() != vB.size())
      return {};
    auto ret = vB;
    for (size_t r_idx = 0; r_idx < Nr; ++r_idx)
    {
      auto& row_r = ret[r_idx];
      const auto& row_a = vA[r_idx];
      const auto& row_b = vB[r_idx];
      for (size_t c_idx = 0; c_idx < Nc; ++c_idx)
        row_r[c_idx] = row_a[c_idx] + row_b[c_idx];
    }
    return ret;
  }

  template<typename Cont>
  Cont subtract(const Cont& cA, const Cont& cB)
  {
    assert(cA.size() == cB.size());
    if (cA.size() != cB.size())
      return {};
    Cont ret = cB;
    std::transform(cA.begin(), cA.end(), cB.begin(), ret.begin(), std::minus<typename Cont::value_type>());
    return ret;
  }

  template<typename T>
  std::vector<std::vector<T>> subtract(const std::vector<std::vector<T>>& vA,
                                       const std::vector<std::vector<T>>& vB)
  {
    auto Nra = vA.size();
    auto Nca = vA.back().size();
    auto Nrb = vB.size();
    auto Ncb = vB.back().size();
    auto Nr = Nra;
    auto Nc = Nca;
    assert(Nra == Nrb);
    assert(Nca == Ncb);
    if (vA.size() != vB.size())
      return {};
    auto ret = vB;
    for (size_t r_idx = 0; r_idx < Nr; ++r_idx)
    {
      auto& row_r = ret[r_idx];
      const auto& row_a = vA[r_idx];
      const auto& row_b = vB[r_idx];
      for (size_t c_idx = 0; c_idx < Nc; ++c_idx)
        row_r[c_idx] = row_a[c_idx] - row_b[c_idx];
    }
    return ret;
  }

  template<typename Cont>
  Cont add_scalar(const Cont& c, typename Cont::value_type s)
  {
    Cont ret = c;
    for (auto& v : ret)
      v += s;
    return ret;
  }

  template<typename T>
  std::vector<std::vector<T>> add_scalar(const std::vector<std::vector<T>>& vec, T s)
  {
    std::vector<std::vector<T>> ret = vec;
    for (auto& row : ret)
      for (auto& v : row)
        v += s;
    return ret;
  }

  template<typename Cont>
  Cont subtract_scalar(const Cont& c, typename Cont::value_type s)
  {
    Cont ret = c;
    for (auto& v : ret)
      v -= s;
    return ret;
  }

  template<typename T>
  std::vector<std::vector<T>> subtract_scalar(const std::vector<std::vector<T>>& vec, T s)
  {
    std::vector<std::vector<T>> ret = vec;
    for (auto& row : ret)
      for (auto& v : row)
        v -= s;
    return ret;
  }
  
  template<typename Cont>
  Cont scalar_subtract(typename Cont::value_type s, const Cont& c)
  {
    Cont ret = c;
    for (auto& v : ret)
      v = s - v;
    return ret;
  }
  
  template<typename Cont>
  Cont unary_minus(const Cont& c)
  {
    auto N = c.size();
    Cont ret(N, static_cast<typename Cont::value_type>(0));
    for (size_t i = 0; i < N; ++i)
      ret[i] = -c[i];
    return ret;
  }

  template<typename Cont>
  typename Cont::value_type dot(const Cont& cA, const Cont& cB)
  {
    auto cp = comp_prod(cA, cB);
    return sum(cp);
  }

  template<typename T>
  T dot(const std::vector<std::vector<T>>& vA, const std::vector<std::vector<T>>& vB)
  {
    auto cp = comp_prod(vA, vB);
    return sum(cp);
  }
  
  template<typename Cont>
  Cont sqrt(const Cont& c)
  {
    auto N = c.size();
    Cont ret(N, static_cast<typename Cont::value_type>(0));
    for (size_t i = 0; i < N; ++i)
      ret[i] = sqrt(c[i]); // #FIXME: Replace with std::sqrt().
    return ret;
  }
  
  template<typename Cont>
  Cont sin(const Cont& c)
  {
    auto N = c.size();
    Cont ret(N, static_cast<typename Cont::value_type>(0));
    for (size_t i = 0; i < N; ++i)
      ret[i] = std::sin(c[i]);
    return ret;
  }
  
  template<typename Cont>
  Cont cos(const Cont& c)
  {
    auto N = c.size();
    Cont ret(N, static_cast<typename Cont::value_type>(0));
    for (size_t i = 0; i < N; ++i)
      ret[i] = std::cos(c[i]);
    return ret;
  }
  
  template<typename Tdst, typename Tsrc>
  std::vector<Tdst> static_cast_vector(const std::vector<Tsrc>& v)
  {
    std::vector<Tdst> ret;
    ret.reserve(v.size());
    for (auto e : v)
      ret.emplace_back(static_cast<Tdst>(e));
    return ret;
  }

  template<typename Cont>
  size_t max_element_idx(const Cont& c)
  {
    return std::distance(c.begin(), std::max_element(c.begin(), c.end()));
  }

  template<typename Cont>
  size_t min_element_idx(const Cont& c)
  {
    return std::distance(c.begin(), std::min_element(c.begin(), c.end()));
  }

  template<typename Cont>
  std::vector<size_t> argmax(const Cont& c)
  {
    // If all values are the same, then there is no maximum.
    if (std::all_of(c.begin(), c.end(), [&c](const auto v) { return v == c.back(); }))
      return {};
  
    std::vector<size_t> indices;
    typename Cont::value_type curr_max = math::get_min<typename Cont::value_type>();
    auto N = c.size();
    for (size_t e_idx = 0; e_idx < N; ++e_idx)
    {
      if (math::maximize(curr_max, c[e_idx]))
        indices.clear();
  
      if (c[e_idx] == curr_max)
        indices.push_back(e_idx);
    }
    return indices;
  }
  
  template<typename Cont>
  std::vector<size_t> argmin(const Cont& c)
  {
    // If all values are the same, then there is no minimum.
    if (std::all_of(c.begin(), c.end(), [&c](const auto v) { return v == c.back(); }))
      return {};
  
    std::vector<size_t> indices;
    typename Cont::value_type curr_min = math::get_max<typename Cont::value_type>();
    auto N = c.size();
    for (size_t e_idx = 0; e_idx < N; ++e_idx)
    {
      if (math::minimize(curr_min, c[e_idx]))
        indices.clear();
  
      if (c[e_idx] == curr_min)
        indices.push_back(e_idx);
    }
    return indices;
  }

  template<typename Cont>
  Cont cat(const Cont& c) { return c; }

  template<typename Cont, typename ... ContN>
  Cont cat(const Cont& c, ContN... cn)
  {
    Cont ret = c;
    auto ci = cat(cn...);
    ret.insert(ret.end(), ci.begin(), ci.end());
    return ret;
  }
  
  template<typename Cont>
  void append(Cont& c) {}
  
  template<typename Cont, typename ... ContN>
  void append(Cont& c, ContN... cn)
  {
    auto ci = cat(cn...);
    c.insert(c.end(), ci.begin(), ci.end());
  }

  template<typename T>
  std::vector<T> repval(T val, size_t n)
  {
    std::vector<T> ret(n, val);
    return ret;
  }

  template<typename T>
  std::vector<T> subset(const std::vector<T>& v, size_t idx_begin, size_t idx_end)
  {
    auto N = v.size();
    assert(idx_begin < N);
    assert(idx_end < N);
    std::vector<T> ret(v.begin() + idx_begin, v.begin() + idx_end + 1);
    return ret;
  }

  template<typename T>
  std::vector<std::vector<T>> subset(const std::vector<std::vector<T>>& v,
                                     size_t r_idx_begin, size_t r_idx_end,
                                     size_t c_idx_begin, size_t c_idx_end)
  {
    auto Nr = v.size();
    auto Nc = v.back().size();
    assert(r_idx_begin < Nr);
    assert(r_idx_end < Nr);
    assert(c_idx_begin < Nc);
    assert(c_idx_end < Nc);
    std::vector<std::vector<T>> ret(v.begin() + r_idx_begin, v.begin() + r_idx_end + 1);
    for (auto& row : ret)
      row = std::vector<T> { row.begin() + c_idx_begin, row.begin() + c_idx_end + 1 };
    return ret;
  }

  template<typename T>
  std::vector<std::vector<T>> subset_columns(const std::vector<std::vector<T>>& v,
                                             size_t c_idx_begin, size_t c_idx_end)
  {
    auto Nc = v.back().size();
    assert(c_idx_begin < Nc);
    assert(c_idx_end < Nc);
    std::vector<std::vector<T>> ret(v.begin(), v.end());
    for (auto& row : ret)
      row = std::vector<T> { row.begin() + c_idx_begin, row.begin() + c_idx_end + 1 };
    return ret;
  }

  template<typename T, size_t N>
  std::vector<T> to_vector(const std::array<T, N>& arr)
  {
    std::vector<T> ret(arr.begin(), arr.end());
    return ret;
  }

  template<typename T, size_t Nr, size_t Nc>
  std::vector<std::vector<T>> to_vector(const std::array<std::array<T, Nc>, Nr>& arr)
  {
    std::vector<std::vector<T>> ret(Nr);
    for (size_t r_idx = 0; r_idx < Nr; ++r_idx)
    {
      const auto& arr_row = arr[r_idx];
      ret[r_idx] = { arr_row.begin(), arr_row.end() };
    }
    return ret;
  }

  template<typename T, size_t N>
  std::array<T, N> to_array(const std::vector<T>& vec)
  {
    assert(vec.size() == N);
    std::array<T, N> ret;
    for (size_t e_idx = 0; e_idx < N; ++e_idx)
      ret[e_idx] = vec[e_idx];
    return ret;
  }

  template<typename T, size_t Nr, size_t Nc>
  std::array<std::array<T, Nc>, Nr> to_array(const std::vector<std::vector<T>>& vec)
  {
    assert(vec.size() == Nr);
    std::array<std::array<T, Nc>, Nr> ret;
    for (size_t r_idx = 0; r_idx < Nr; ++r_idx)
    {
      auto& ret_row = ret[r_idx];
      const auto& vec_row = vec[r_idx];
      assert(vec_row.size() == Nc);
      for (size_t c_idx = 0; c_idx < Nc; ++c_idx)
        ret_row[c_idx] = vec_row[c_idx];
    }
    return ret;
  }

  template<typename T>
  void resize(std::vector<T>& vec, size_t N)
  {
    vec.resize(N);
  }

  template<typename T>
  void resize(std::vector<std::vector<T>>& vec, size_t Nr, size_t Nc)
  {
    vec.resize(Nr);
    for (auto& row : vec)
      row.resize(Nc);
  }

  template<typename T>
  void resize(std::vector<std::vector<std::vector<T>>>& vec, size_t Ns, size_t Nr, size_t Nc)
  {
    vec.resize(Ns);
    for (auto& slice : vec)
      resize(slice, Nr, Nc);
  }
  
  template<typename T>
  size_t count(const std::vector<T>& vec, T val)
  {
    return std::count(vec.begin(), vec.end(), val);
  }
  
  template<typename T, size_t N>
  size_t count(const std::array<T, N>& arr, T val)
  {
    return std::count(arr.begin(), arr.end(), val);
  }
  
  template<typename T, typename Lambda>
  size_t count_if(const std::vector<T>& vec, Lambda&& pred)
  {
    return std::count_if(vec.begin(), vec.end(), pred);
  }
  
  template<typename T, size_t N, typename Lambda>
  size_t count_if(const std::array<T, N>& arr, Lambda&& pred)
  {
    return std::count_if(arr.begin(), arr.end(), pred);
  }
  
  template<typename Cont>
  void sort(Cont& c)
  {
    std::sort(c.begin(), c.end());
  }
  
  template<typename Cont, typename Lambda>
  void sort(Cont& c, Lambda pred)
  {
    std::sort(c.begin(), c.end(), pred);
  }
  
  template<typename Cont, typename Lambda>
  int find_if_idx(Cont& c, Lambda pred)
  {
    auto it = std::find_if(std::begin(c), std::end(c), pred);
    if (it == std::end(c))
      return -1;
    return static_cast<int>(std::distance(std::begin(c), it));
  }
  
  template<typename Cont, typename Lambda>
  typename Cont::iterator find_if(Cont& c, Lambda pred)
  {
    return std::find_if(std::begin(c), std::end(c), pred);
  }
  
  template<typename Cont>
  typename Cont::iterator find(Cont& c, const typename Cont::value_type& val)
  {
    return std::find(std::begin(c), std::end(c), val);
  }
  
  template<typename Cont>
  int find_idx(Cont& c, const typename Cont::value_type& val)
  {
    auto it = std::find(std::begin(c), std::end(c), val);
    if (it == std::end(c))
      return -1;
    return static_cast<int>(std::distance(std::begin(c), it));
  }
  
  template<typename Cont, typename Lambda>
  bool contains_if(const Cont& c, Lambda pred)
  {
    return std::find_if(std::begin(c), std::end(c), pred) != std::end(c);
  }
  
  template<typename Cont>
  bool contains(const Cont& c, const typename Cont::value_type& val)
  {
    return std::find(std::begin(c), std::end(c), val) != std::end(c);
  }
  
  template<typename Cont>
  void erase(Cont& c, const typename Cont::value_type& val)
  {
    c.erase(find(c, val));
  }
  
  template<typename Cont>
  bool erase_at(Cont& c, size_t idx)
  {
    if (idx >= c.size())
      return false;
    
    auto it = c.begin();
    std::advance(it, idx);
    c.erase(it);
    return true;
  }
  
  template<typename Cont, typename Lambda>
  bool erase_if(Cont& c, Lambda pred)
  {
    auto it = std::find_if(std::begin(c), std::end(c), pred);
    if (it != std::end(c))
    {
      c.erase(it);
      return true;
    }
    return false;
  }
  
  template<typename Cont>
  typename Cont::value_type try_get(const Cont& cont, int idx,
                                    typename Cont::value_type def_val = static_cast<typename Cont::value_type>(0))
  {
    if (idx < static_cast<int>(cont.size()))
      return cont[idx];
    return def_val;
  }
  
  template<typename T>
  T& at_growing(std::vector<T>& vec, int idx, T default_val = {})
  {
    if (idx < 0)
      idx = 0;
    if (idx >= static_cast<int>(vec.size()))
      vec.resize(idx + 1, default_val);
    return vec[idx];
  }
  
  // static_cast<int>(apa.size());
  // vs
  // stlutils::sizeI(apa);
  template<typename Cont>
  constexpr int sizeI(const Cont& c) noexcept
  {
    return static_cast<int>(std::size(c));
  }

}
