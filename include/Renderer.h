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

#include "cinder/app/Window.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/GlslProg.h"

#include "GlslParams.h"
#include "System.h"

namespace reza {
namespace ps {

typedef std::shared_ptr<class Renderer> RendererRef;
typedef std::weak_ptr<class Renderer> RendererWeakRef;
class Renderer : public std::enable_shared_from_this<class Renderer> {
  public:
	virtual ~Renderer();

	void _setupBatch();
	virtual void setup();
	virtual void setupGlsl();
	virtual void setupBatch();
	virtual void update();
	virtual void draw();

	virtual bool isInitialized() { return mGlslInitialized; }
	virtual void setInitialized( bool initialized ) { mGlslInitialized = initialized; }
	virtual ci::gl::GlslProgRef getGlslProgRef() { return mGlslProgRef; }
	bool mDraw = true;
	bool mAdditiveBlending = false;
	bool mDepthTest = true;
	bool mDepthWrite = true;

  protected:
	Renderer( const ci::app::WindowRef &window,
		const ci::fs::path &vertexPath,
		const ci::fs::path &fragmentPath,
		reza::ps::SystemRef systemRef,
		std::function<void()> superFn = nullptr,
		std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn = nullptr,
		std::function<void( ci::Exception )> glslErrorFn = nullptr );

	bool mSetupBatch = false;
	bool mGlslInitialized = false;
	std::function<void()> mSuperFn = nullptr;
	std::function<void( reza::glsl::GlslParamsRef )> mGlslUpdatedFn = nullptr;
	std::function<void( ci::Exception )> mGlslErrorFn = nullptr;

	reza::ps::SystemRef mSystemRef;
	ci::gl::GlslProg::Format mGlslProgFormat;
	ci::gl::GlslProgRef mGlslProgRef;
	reza::glsl::GlslParamsRef mGlslParamsRef;
	ci::app::WindowRef mWindowRef;
	ci::fs::path mVertexPath;
	ci::fs::path mFragmentPath;

	virtual void _draw() = 0;
};

} // namespace ps
} // namespace reza