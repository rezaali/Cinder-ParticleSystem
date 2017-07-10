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

#include "System.h"

#include "cinder/Exception.h"
#include "cinder/Filesystem.h"
#include "cinder/app/Window.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/gl.h"

#include "GlslParams.h"

namespace reza {
namespace ps {

const int PARTICLES_POS_INDEX = 0;
const int PARTICLES_VEL_INDEX = 1;
const int PARTICLES_CLR_INDEX = 2;
const int PARTICLES_ORI_INDEX = 3;

typedef std::shared_ptr<class ParticleSystem> ParticleSystemRef;
class ParticleSystem : public System {
  public:
	static ParticleSystemRef create(
		const ci::app::WindowRef &window,
		const ci::fs::path &vertexPath,
		ci::gl::GlslProg::Format &glslFormat,
		std::function<void()> superFn = nullptr,
		std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn = nullptr,
		std::function<void( ci::Exception )> glslErrorFn = nullptr )
	{
		return ParticleSystemRef( new ParticleSystem( window, vertexPath, glslFormat, superFn, glslUpdatedFn, glslErrorFn ) );
	}
	virtual ~ParticleSystem();

	virtual void bind();

	ci::gl::VboRef getPositionVbo( int index )
	{
		return mPositionVbo[index];
	}

	ci::gl::VboRef getVelocityVbo( int index )
	{
		return mVelocityVbo[index];
	}

	ci::gl::VboRef getColorVbo( int index )
	{
		return mColorVbo[index];
	}

	ci::gl::VboRef getOrientationVbo( int index )
	{
		return mOrientationVbo[index];
	}

	ci::gl::Batch::AttributeMapping getRendererGlslAttributeMapping()
	{
		ci::geom::BufferLayout instancePositionDataLayout;
		instancePositionDataLayout.append( ci::geom::Attrib::CUSTOM_0, 4, 0, 0, 1 /* per instance */ );

		ci::geom::BufferLayout instanceVelocityDataLayout;
		instanceVelocityDataLayout.append( ci::geom::Attrib::CUSTOM_1, 4, 0, 0, 1 /* per instance */ );

		ci::geom::BufferLayout instanceColorDataLayout;
		instanceColorDataLayout.append( ci::geom::Attrib::CUSTOM_2, 4, 0, 0, 1 /* per instance */ );

		ci::geom::BufferLayout instanceOrientationDataLayout;
		instanceOrientationDataLayout.append( ci::geom::Attrib::CUSTOM_3, 4, 0, 0, 1 /* per instance */ );

		return {
			{ ci::geom::Attrib::CUSTOM_0, "position_id" },
			{ ci::geom::Attrib::CUSTOM_1, "velocity_mass" },
			{ ci::geom::Attrib::CUSTOM_2, "color" },
			{ ci::geom::Attrib::CUSTOM_3, "orientation" }
		};
	}

	ci::gl::BufferTextureRef &getPositionBufferTextureRef( int index )
	{
		return mPositionBufferTextures[index];
	}

	ci::gl::BufferTextureRef &getVelocityBufferTextureRef( int index )
	{
		return mVelocityBufferTextures[index];
	}

	ci::gl::BufferTextureRef &getColorBufferTextureRef( int index )
	{
		return mColorBufferTextures[index];
	}

	ci::gl::BufferTextureRef &getOrientationBufferTextureRef( int index )
	{
		return mOrientationBufferTextures[index];
	}

	ci::gl::GlslProg::Format getRendererGlslFormat() override
	{
		ci::gl::GlslProg::Format format;
		format.attribLocation( "position_id", PARTICLES_POS_INDEX );
		format.attribLocation( "velocity_mass", PARTICLES_VEL_INDEX );
		format.attribLocation( "color", PARTICLES_CLR_INDEX );
		format.attribLocation( "orientation", PARTICLES_ORI_INDEX );
		return format;
	}

	SystemRef getSystemRef()
	{
		return static_cast<SystemRef>( this );
	}

  protected:
	ParticleSystem(
		const ci::app::WindowRef &window,
		const ci::fs::path &vertexPath,
		ci::gl::GlslProg::Format &glslFormat,
		std::function<void()> superFn = nullptr,
		std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn = nullptr,
		std::function<void( ci::Exception )> glslErrorFn = nullptr );

	void setupBuffers() override;
	void updateBuffers() override;

	ci::gl::VboRef mPositionVbo[2];
	ci::gl::VboRef mVelocityVbo[2];
	ci::gl::VboRef mColorVbo[2];
	ci::gl::VboRef mOrientationVbo[2];

	ci::gl::BufferTextureRef mPositionBufferTextures[2];
	ci::gl::BufferTextureRef mVelocityBufferTextures[2];
	ci::gl::BufferTextureRef mColorBufferTextures[2];
	ci::gl::BufferTextureRef mOrientationBufferTextures[2];

	void _update() override;
};

} // namespace ps
} // namespace reza