/*
Copyright(c) 2017 Reza Ali syed.reza.ali@gmail.com www.syedrezaali.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "ParticleSystem.h"
#include "PlexusSystem.h"
#include "Renderer.h"

namespace reza {
namespace ps {
typedef std::shared_ptr<class PlexusRenderer> PlexusRendererRef;
class PlexusRenderer : public Renderer {
  public:
	static PlexusRendererRef create(
		const ci::app::WindowRef &window,
		const ci::fs::path &vertexPath,
		const ci::fs::path &fragmentPath,
		reza::ps::ParticleSystemRef particleSystemRef,
		reza::ps::PlexusSystemRef plexusSystemRef,
		std::function<void()> superFn = nullptr,
		std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn = nullptr,
		std::function<void( ci::Exception )> glslErrorFn = nullptr )
	{
		return PlexusRendererRef( new PlexusRenderer( window, vertexPath, fragmentPath, particleSystemRef, plexusSystemRef, superFn, glslUpdatedFn, glslErrorFn ) );
	}

	void setupBatch() override;

  protected:
	PlexusRenderer(
		const ci::app::WindowRef &window,
		const ci::fs::path &vertexPath,
		const ci::fs::path &fragmentPath,
		reza::ps::ParticleSystemRef particleSystemRef,
		reza::ps::PlexusSystemRef plexusSystemRef,
		std::function<void()> superFn = nullptr,
		std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn = nullptr,
		std::function<void( ci::Exception )> glslErrorFn = nullptr );

	PlexusSystemRef mPlexusSystemRef;
	ParticleSystemRef mParticleSystemRef;
	ci::gl::VboMeshRef mVboMeshRef;
	ci::gl::BatchRef mBatchRef;
	int mTotal = 0;

	void _draw() override;
};

} // namespace ps
} // namespace reza
