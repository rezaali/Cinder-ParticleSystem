#include "PointRenderer.h"

#include "LiveCode.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace std;
using namespace ci::app;
using namespace reza::ps;
using namespace reza::glsl;

namespace reza {
namespace ps {

PointRenderer::PointRenderer(
	const WindowRef &window,
	const fs::path &vertexPath,
	const fs::path &fragmentPath,
	ParticleSystemRef particleSystemRef,
	std::function<void()> superFn,
	std::function<void( GlslParamsRef )> glslUpdatedFn,
	std::function<void( ci::Exception )> glslErrorFn )
	: Renderer( window, vertexPath, fragmentPath, static_cast<SystemRef>( particleSystemRef ), superFn, glslUpdatedFn, glslErrorFn ), mParticleSystemRef( particleSystemRef )
{
}

void PointRenderer::_draw()
{
	mParticleSystemRef->bind();
	gl::ScopedGlslProg glslScope( mGlslProgRef );
	gl::setDefaultShaderVars();
	gl::enable( GL_POINT_SPRITE_ARB );
	gl::enable( GL_VERTEX_PROGRAM_POINT_SIZE );
	gl::drawArrays( GL_POINTS, 0, mParticleSystemRef->getTotal() );
	gl::disableBlending();
}

} // namespace ps
} // namespace reza