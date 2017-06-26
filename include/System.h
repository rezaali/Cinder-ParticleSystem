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

#include <functional>

#include "cinder/Exception.h"
#include "cinder/Filesystem.h"
#include "cinder/app/Window.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/gl.h"

#include "GlslParams.h"

namespace reza {
namespace ps {

typedef std::shared_ptr<class System> SystemRef;
class System : public std::enable_shared_from_this<class System> {
  public:
	static SystemRef create(
		const ci::app::WindowRef &window,
		const ci::fs::path &vertexPath,
		ci::gl::GlslProg::Format &glslFormat,
		std::function<void()> superFn = nullptr,
		std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn = nullptr,
		std::function<void( ci::Exception )> glslErrorFn = nullptr )
	{
		return SystemRef( new System( window, vertexPath, glslFormat, superFn, glslUpdatedFn, glslErrorFn ) );
	}
	virtual ~System();

	virtual void setupGlsl();
	virtual bool isInitialized() { return mGlslInitialized; }
	virtual void setInitialized( bool initialized ) { mGlslInitialized = initialized; }
	virtual ci::gl::GlslProgRef getGlslProgRef() { return mGlslProgRef; }
	virtual ci::gl::VaoRef getVao( int index )
	{
		return mVaos[index];
	}

	virtual ci::gl::GlslProg::Format getRendererGlslFormat()
	{
		ci::gl::GlslProg::Format format;
		return format;
	}

	void setUpdateBuffers( bool value )
	{
		mUpdateBuffers = value;
	}

	void reset() { mUpdateBuffers = true; }
	int getIterationIndex() { return mIterationIndex; }
	int getTotal() { return mTotal; }
	void setTotal( int total )
	{
		if( total != mTotal ) {
			mTotalNew = total;
			mUpdateBuffers = true;
		}
	}

	virtual bool getUpdate() { return mUpdate; }
	virtual void setUpdate( bool update )
	{
		mUpdate = update;
	}
	virtual void update();

  protected:
	System(
		const ci::app::WindowRef &window,
		const ci::fs::path &vertexPath,
		ci::gl::GlslProg::Format &glslFormat,
		std::function<void()> superFn = nullptr,
		std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn = nullptr,
		std::function<void( ci::Exception )> glslErrorFn = nullptr );

	bool mGlslInitialized = false;
	std::function<void()> mSuperFn = nullptr;
	std::function<void( reza::glsl::GlslParamsRef )> mGlslUpdatedFn = nullptr;
	std::function<void( ci::Exception )> mGlslErrorFn = nullptr;

	ci::gl::GlslProg::Format mGlslFormat;
	ci::gl::GlslProgRef mGlslProgRef;
	reza::glsl::GlslParamsRef mGlslParamsRef;
	ci::app::WindowRef mWindowRef;
	ci::fs::path mVertexPath;

	virtual void setup();
	virtual void setupBuffers();
	virtual void updateBuffers();

	//PARTICLES
	ci::gl::VaoRef mVaos[2];
	ci::gl::TransformFeedbackObjRef mFeedbackObjs[2];

	int mTotalNew = 100;
	int mTotal = 100;
	int mIterationsPerFrame = 1;
	int mIterationIndex = 0;

	bool mSetupBuffers = true;
	bool mUpdateBuffers = true;
	bool mUpdate = true;

	virtual void _update();
};

} // namespace ps
} // namespace reza
