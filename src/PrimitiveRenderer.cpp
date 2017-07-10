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
	mGeom = sphere;
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

void PrimitiveRenderer::setGeometry( ci::geom::Source *geom )
{
	mFormat.geometry( geom );
	setupBatch();
}

void PrimitiveRenderer::setupBatch()
{
	if( mGlslProgRef ) {
		for( int i = 0; i < 2; i++ ) {
			mBatchRef[i] = gl::Batch::create( *mFormat.mGeom, mGlslProgRef );
		}
		Renderer::setupBatch();
	}
}

void PrimitiveRenderer::_draw()
{
	if( mBatchRef[mParticleSystemRef->getIterationIndex() & 1] ) {
		mGlslParamsRef->applyUniforms( mGlslProgRef );

		mGlslProgRef->uniform( "position_id", 0 );
		mGlslProgRef->uniform( "velocity_mass", 1 );
		mGlslProgRef->uniform( "color", 2 );
		mGlslProgRef->uniform( "orientation", 3 );

		mParticleSystemRef->getPositionBufferTextureRef( mParticleSystemRef->getIterationIndex() & 1 )->bindTexture( 0 );
		mParticleSystemRef->getVelocityBufferTextureRef( mParticleSystemRef->getIterationIndex() & 1 )->bindTexture( 1 );
		mParticleSystemRef->getColorBufferTextureRef( mParticleSystemRef->getIterationIndex() & 1 )->bindTexture( 2 );
		mParticleSystemRef->getOrientationBufferTextureRef( mParticleSystemRef->getIterationIndex() & 1 )->bindTexture( 3 );

		mBatchRef[mParticleSystemRef->getIterationIndex() & 1]->drawInstanced( mParticleSystemRef->getTotal() );
	}
}

} // namespace ps
} // namespace reza
