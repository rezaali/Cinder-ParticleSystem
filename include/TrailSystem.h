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
#include "System.h"

namespace reza {
namespace ps {

const int TRAILS_POS_INDEX = 0;
const int TRAILS_INFO_INDEX = 1;

typedef std::shared_ptr<class TrailSystem> TrailSystemRef;
class TrailSystem : public System {
  public:
	static TrailSystemRef create(
		const ci::app::WindowRef &window,
		const ci::fs::path &vertexPath,
		ci::gl::GlslProg::Format &glslFormat,
		ParticleSystemRef particleSystemRef,
		std::function<void()> superFn = nullptr,
		std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn = nullptr,
		std::function<void( ci::Exception )> glslErrorFn = nullptr )
	{
		return TrailSystemRef( new TrailSystem( window, vertexPath, glslFormat, particleSystemRef, superFn, glslUpdatedFn, glslErrorFn ) );
	}

	virtual ~TrailSystem();

	ci::gl::GlslProg::Format getRendererGlslFormat() override
	{
		ci::gl::GlslProg::Format format;
		format.attribLocation( "position", TRAILS_POS_INDEX );
		format.attribLocation( "info", TRAILS_INFO_INDEX );
		return format;
	}

	ci::gl::BufferTextureRef &getPositionBufferTextureRef( int index )
	{
		return mPositionBufferTextures[index];
	}

  protected:
	TrailSystem(
		const ci::app::WindowRef &window,
		const ci::fs::path &vertexPath,
		ci::gl::GlslProg::Format &glslFormat,
		ParticleSystemRef particleSystemRef,
		std::function<void()> superFn = nullptr,
		std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn = nullptr,
		std::function<void( ci::Exception )> glslErrorFn = nullptr );

	void setupBuffers() override;
	void updateBuffers() override;

	ParticleSystemRef mParticleSystemRef;
	ci::gl::BufferTextureRef mPositionBufferTextures[2];
	ci::gl::VboRef mPositionsVbo[2];
	ci::gl::VboRef mInfoVbo;

	void _update() override;
};

} // namespace ps
} // namespace reza
