#include "..\include\TrailPointRenderer.h"

#include "LiveCode.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"

#include "Watchdog.h"

using namespace ci;
using namespace std;
using namespace ci::app;
using namespace reza::ps;
using namespace reza::glsl;

namespace reza {
namespace ps {

TrailPointRenderer::TrailPointRenderer(
	const WindowRef &window,
	const fs::path &vertexPath,
	const fs::path &fragmentPath,
	reza::ps::ParticleSystemRef &particleSystemRef,
	reza::ps::TrailSystemRef &trailSystemRef,
	std::function<void()> superFn,
	std::function<void( GlslParamsRef )> glslUpdatedFn,
	std::function<void( ci::Exception )> glslErrorFn )
	: Renderer(
		  window,
		  vertexPath,
		  fragmentPath,
		  static_cast<SystemRef>( trailSystemRef ),
		  superFn,
		  glslUpdatedFn,
		  glslErrorFn ),
	  mParticleSystemRef( particleSystemRef ),
	  mTrailSystemRef( trailSystemRef )
{
}

void TrailPointRenderer::_draw()
{
	gl::ScopedGlslProg glslScope( mGlslProgRef );
	gl::setDefaultShaderVars();
	mGlslProgRef->uniform( "trailLength", float( mTrailSystemRef->getTotal() ) );
	mTrailSystemRef->getVao( mTrailSystemRef->getIterationIndex() & 1 )->bind();
	gl::drawArrays( GL_POINTS, 0, mParticleSystemRef->getTotal() * mTrailSystemRef->getTotal() );
}

} // namespace ps
} // namespace reza