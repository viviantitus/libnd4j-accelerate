/******************************************************************************
 * Copyright (c) 2010-2011, Duane Merrill.  All rights reserved.
 * Copyright (c) 2011-2013, NVIDIA CORPORATION.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the NVIDIA CORPORATION nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NVIDIA CORPORATION BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

/******************************************************************************
 * CTA-processing functionality for radix sort upsweep reduction kernels
 ******************************************************************************/

#pragma once

#include <b40c/partition/upsweep/cta.cuh>

#include <b40c/radix_sort/upsweep/tile.cuh>

namespace b40c {
namespace radix_sort {
namespace upsweep {

/**
 * Radix sort upsweep reduction CTA
 *
 * Derives from partition::upsweep::Cta
 */
template <typename KernelPolicy>
struct Cta :
	partition::upsweep::Cta<
		KernelPolicy,
		Cta<KernelPolicy>,			// This class
		Tile>						// radix_sort::upsweep::Tile
{
	//---------------------------------------------------------------------
	// Typedefs and Constants
	//---------------------------------------------------------------------

	// Base class type
	typedef partition::upsweep::Cta<KernelPolicy, Cta, Tile> Base;

	typedef typename KernelPolicy::KeyType 					KeyType;
	typedef typename KernelPolicy::SizeT 					SizeT;
	typedef typename KernelPolicy::SmemStorage				SmemStorage;


	//---------------------------------------------------------------------
	// Methods
	//---------------------------------------------------------------------

	/**
	 * Constructor
	 */
	__device__ __forceinline__ Cta(
		SmemStorage 	&smem_storage,
		KeyType 		*d_in_keys,
		SizeT 			*d_spine) :
			Base(smem_storage, d_in_keys, d_spine)
	{}

};



} // namespace upsweep
} // namespace radix_sort
} // namespace b40c

