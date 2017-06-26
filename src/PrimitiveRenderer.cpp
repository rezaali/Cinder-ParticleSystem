#include "PrimitiveRenderer.h"

#include "LiveCode.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace std;
using namespace ci::app;
using namespace reza::ps;
using namespace reza::glsl;

namespace reza {
namespace ps {

PrimitiveRenderer::Format::Format()
{
	ci::geom::Sphere *sphere = new ci::geom::Sphere();
	sphere->radius( 0.5 );
	sphere->subdivisions( 60 );
	mSourceRef = static_cast<ci::geom::SourceRef>( sphere );
}

PrimitiveRenderer::PrimitiveRenderer(
	const WindowRef &window,
	const fs::path &vertexPath,
	const fs::path &fragmentPath,
	reza::ps::ParticleSystemRef particleSystemRef,
	const Format &format,
	std::function<void()> superFn,
	std::function<void( GlslParamsRef )> glslUpdatedFn,
	std::function<void( ci::Exception )> glslErrorFn )
	: Renderer(
		  window,
		  vertexPath,
		  fragmentPath,
		  static_cast<SystemRef>( particleSystemRef ),
		  superFn,
		  glslUpdatedFn,
		  glslErrorFn ),
	  mFormat( format ),
	  mParticleSystemRef( particleSystemRef )
{
}

void PrimitiveRenderer::setupBatch()
{
	if( mGlslProgRef ) {
		for( int i = 0; i < 2; i++ ) {
			mBatchRef[i] = gl::Batch::create( *mFormat.mSourceRef, mGlslProgRef );
		}
		Renderer::setupBatch();
	}
}

void PrimitiveRenderer::_draw()
{
	if( mBatchRef[mParticleSystemRef->getIterationIndex() & 1] ) {
		mGlslParamsRef->applyUniforms( mGlslProgRef );
		mGlslProgRef->uniform( "position_mass", 0 );
		mParticleSystemRef->getPositionBufferTextureRef( mParticleSystemRef->getIterationIndex() & 1 )->bindTexture( 0 );
		mBatchRef[mParticleSystemRef->getIterationIndex() & 1]->drawInstanced( mParticleSystemRef->getTotal() );
	}
}

} // namespace ps
} // namespace reza
