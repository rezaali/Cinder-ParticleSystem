#include "PlexusRenderer.h"

#include "LiveCode.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace std;
using namespace ci::app;
using namespace reza::ps;
using namespace reza::glsl;

namespace reza {
namespace ps {

PlexusRenderer::PlexusRenderer(
	const WindowRef &window,
	const fs::path &vertexPath,
	const fs::path &fragmentPath,
	reza::ps::ParticleSystemRef particleSystemRef,
	reza::ps::PlexusSystemRef plexusSystemRef,
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
	  mParticleSystemRef( particleSystemRef ),
	  mPlexusSystemRef( plexusSystemRef )
{
}

void PlexusRenderer::setupBatch()
{
	if( mGlslProgRef ) {
		int total = mTotal * 2;
		vector<vec4> positions( total );

		for( uint32_t i = 0; i < total; i++ ) {
			positions[i] = vec4( 0.0f, 0.0f, 0.0f, i );
		}

		mVboMeshRef = gl::VboMesh::create( total, GL_LINES, { gl::VboMesh::Layout().attrib( geom::POSITION, 4 ) } );
		mVboMeshRef->bufferAttrib( geom::POSITION, sizeof( vec4 ) * positions.size(), positions.data() );
		mBatchRef = gl::Batch::create( mVboMeshRef, mGlslProgRef );

		Renderer::setupBatch();
	}
}

void PlexusRenderer::_draw()
{
	if( mParticleSystemRef && mPlexusSystemRef && mBatchRef ) {
		if( mTotal != mPlexusSystemRef->getTotal() ) {
			mTotal = mPlexusSystemRef->getTotal();
			setupBatch();
		}
		mGlslProgRef->uniform( "uPositionMass", 0 );
		mParticleSystemRef->getPositionBufferTextureRef( mParticleSystemRef->getIterationIndex() & 1 )->bindTexture( 0 );
		mGlslProgRef->uniform( "uPlexus", 1 );
		mPlexusSystemRef->getDistanceBufferTextureRef( mPlexusSystemRef->getIterationIndex() & 1 )->bindTexture( 1 );
		mBatchRef->draw();
	}
}

} // namespace ps
} // namespace reza