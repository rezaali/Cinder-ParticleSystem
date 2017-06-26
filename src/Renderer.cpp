#include "Renderer.h"
#include "Watchdog.h"

#include "LiveCode.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace std;
using namespace ci::app;
using namespace reza::ps;
using namespace reza::glsl;

namespace reza {
namespace ps {

Renderer::Renderer( const ci::app::WindowRef &window,
	const ci::fs::path &vertexPath,
	const ci::fs::path &fragmentPath,
	SystemRef systemRef,
	std::function<void()> superFn,
	std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn,
	std::function<void( ci::Exception )> glslErrorFn )
	: mWindowRef( window ),
	  mVertexPath( vertexPath ),
	  mFragmentPath( fragmentPath ),
	  mSystemRef( systemRef ),
	  mSuperFn( superFn ),
	  mGlslUpdatedFn( glslUpdatedFn ),
	  mGlslErrorFn( glslErrorFn ),
	  mGlslParamsRef( GlslParams::create() ),
	  mGlslProgFormat( systemRef->getRendererGlslFormat() )

{
}

Renderer::~Renderer()
{
}

void Renderer::setup()
{
	setupGlsl();
}

void Renderer::update()
{
	if( mSetupBatch ) {
		setupBatch();
	}
}

void Renderer::draw()
{
	if( mDraw && mGlslProgRef ) {
		mGlslParamsRef->applyUniforms( mGlslProgRef );
		if( mAdditiveBlending ) {
			gl::enableAdditiveBlending();
		}
		else {
			gl::enableAlphaBlending();
		}
		gl::ScopedDepthTest scpDrd( mDepthTest );
		gl::ScopedDepthWrite scpDwt( mDepthWrite );
		_draw();
	}
}

void Renderer::setupGlsl()
{
	auto vertex = mVertexPath;
	auto fragment = mFragmentPath;

	wd::unwatch( vertex );
	wd::unwatch( fragment );

	auto cb = [this, vertex, fragment]( const fs::path &path ) {
		reza::live::glsl(
			vertex,
			fragment,
			mGlslProgFormat,
			[this]() {
				if( mSuperFn ) {
					mSuperFn();
				}
			},
			[this]( gl::GlslProgRef result, vector<string> sources ) {
				mWindowRef->getRenderer()->makeCurrentContext( true );
				mGlslProgRef = result;
				_setupBatch();
				mGlslParamsRef->clearUniforms();
				mGlslParamsRef->parseUniforms( sources );
				if( mGlslUpdatedFn ) {
					mGlslUpdatedFn( mGlslParamsRef );
				}
				mGlslInitialized = true;
			},
			[this]( ci::Exception exc ) {
				if( mGlslErrorFn ) {
					mGlslErrorFn( exc );
				}
			} );
	};

	wd::watch( vertex, cb );
	wd::watch( fragment, cb );
}

void Renderer::setupBatch()
{
	mSetupBatch = false;
}

void Renderer::_setupBatch()
{
	mSetupBatch = true;
}

} // namespace ps
} // namespace reza