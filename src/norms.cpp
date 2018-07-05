/* Copyright (C) 2012-2017 IBM Corp.
 * This program is Licensed under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. See accompanying LICENSE file.
 */
/**
 * @file norms.cpp - computing various norms of ring elements
 **/
#include <numeric>
#include <NTL/BasicThreadPool.h>
#include "NumbTh.h"
#include "DoubleCRT.h"
#include "norms.h"
NTL_CLIENT

long sumOfCoeffs(const zzX& f) // = f(1)
{
  long sum = 0;
  for (long i=0; i<lsize(f); i++) sum += f[i];
  return sum;
}
ZZ sumOfCoeffs(const ZZX& f) // = f(1)
{
  ZZ sum = ZZ::zero();
  for (long i=0; i<=deg(f); i++) sum += coeff(f,i);
  return sum;
}
NTL::ZZ sumOfCoeffs(const DoubleCRT& f)
{
  ZZX poly;
  f.toPoly(poly);
  return sumOfCoeffs(poly);
}


long largestCoeff(const zzX& f) // l_infty norm
{
  long mx = 0;
  for (long i=0; i<lsize(f); i++) {
    if (mx < abs(f[i]))
      mx = abs(f[i]);
  }
  return mx;
}
ZZ largestCoeff(const ZZX& f)
{
  ZZ mx = ZZ::zero();
  for (long i=0; i<=deg(f); i++) {
    if (mx < abs(coeff(f,i)))
      mx = abs(coeff(f,i));
  }
  return mx;
}
NTL::ZZ largestCoeff(const DoubleCRT& f)
{
  ZZX poly;
  f.toPoly(poly);
  return largestCoeff(poly);
}


double coeffsL2NormSquared(const zzX& f) // l_2 norm square
{
  double s = 0.0;
  for (long i=0; i<lsize(f); i++) {
    double coef = f[i];
    s += coef * coef;
  }
  return s;
}
xdouble coeffsL2NormSquared(const ZZX& f) // l_2 norm square
{
  xdouble s(0.0);
  for (long i=0; i<=deg(f); i++) {
    xdouble coef(conv<xdouble>(coeff(f,i)));
    s += coef * coef;
  }
  return s;
}
xdouble coeffsL2NormSquared(const DoubleCRT& f) // l2 norm^2
{
  ZZX poly;
  f.toPoly(poly);
  return coeffsL2NormSquared(poly);
}

#if FFT_IMPL
// l_2 norm square of canonical embedding
double embeddingL2NormSquared(const zzX& f, const PAlgebra& palg)
{
  std::vector<cx_double> emb;
  canonicalEmbedding(emb, f, palg);
  double acc = 0.0;
  for (auto& x : emb)
    acc += std::norm(x);
  return 2*acc;
}

//! Computing the L-infinity norm of the canonical embedding
double embeddingLargestCoeff(const zzX& f, const PAlgebra& palg)
{
  std::vector<cx_double> emb;
  canonicalEmbedding(emb, f, palg);
  double mx = 0.0;
  for (auto& x : emb) {
    double n = std::norm(x);
    if (mx < n) mx = n;
  }
  return sqrt(mx);
}

static xdouble convertAndScale(zzX& ff, const NTL::ZZX& f)
{
  xdouble factor(1.0);
  long size = NTL::MaxBits(f);
  if (size > NTL_SP_BOUND-15) {
    ZZ zzFactor(1L);
    zzFactor <<= (NTL_SP_BOUND-15); // divide f by this factor
    ZZX scaled = f / zzFactor;
    convert(factor, zzFactor);      // remember the factor
    convert(ff, scaled);            // convert to zzX
  }
  else
    convert(ff, f);                 // convert to zzX
  return factor;
}

xdouble embeddingL2NormSquared(const NTL::ZZX& f, const PAlgebra& palg)
{
  zzX ff; // to hold a scaled-down version of ff;
  xdouble factor = convertAndScale(ff, f);
  return embeddingL2NormSquared(ff, palg)*factor;
}
xdouble embeddingLargestCoeff(const NTL::ZZX& f, const PAlgebra& palg)
{
  zzX ff; // to hold a scaled-down version of ff;
  xdouble factor = convertAndScale(ff, f);
  return embeddingLargestCoeff(ff, palg)*factor;
}
#endif