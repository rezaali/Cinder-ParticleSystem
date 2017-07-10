#include "../include/System.h"

#include "cinder/Rand.h"

#include "LiveCode.h"
#include "Watchdog.h"
#include "cinder/Log.h"

using namespace ci;
using namespace std;
using namespace ci::app;
using namespace reza::glsl;

namespace reza {
namespace ps {

System::System(
	const WindowRef &window,
	const fs::path &vertexPath,
	gl::GlslProg::Format &glslFormat,
	std::function<void()> superFn,
	std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn,
	std::function<void( ci::Exception )> glslErrorFn )
	: mWindowRef( window ),
	  mVertexPath( vertexPath ),
	  mGlslFormat( glslFormat ),
	  mSuperFn( superFn ),
	  mGlslUpdatedFn( glslUpdatedFn ),
	  mGlslErrorFn( glslErrorFn ),
	  mGlslParamsRef( GlslParams::create() )
{
	setup();
}

System::~System()
{
}

void System::setup()
{
	setupGlsl();
}

void System::setupGlsl()
{
	wd::unwatch( mVertexPath );

	auto cb = [this]( const fs::path &path ) {
		reza::live::glsl(
			mVertexPath,
			mGlslFormat,
			[this]() {
				if( mSuperFn ) {
					mSuperFn();
				}
			},
			[this]( gl::GlslProgRef result, vector<string> sources ) {
				mWindowRef->getRenderer()->makeCurrentContext( true );
				mGlslProgRef = result;
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

	wd::watch( mVertexPath, cb );
}

void System::setupBuffers()
{
}

void System::updateBuffers()
{
}

void System::_update()
{
}

void System::update()
{
	if( mSetupBuffers ) {
		setupBuffers();
		mSetupBuffers = false;
	}

	if( mUpdateBuffers ) {
		mIterationIndex = 0;
		mTotal = mTotalNew;
		updateBuffers();
		mUpdateBuffers = false;
	}

	if( mUpdate && mGlslProgRef ) {
		_update();
	}
}

} // namespace ps
} // namespace reza