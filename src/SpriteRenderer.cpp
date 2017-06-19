#include "..\include\SpriteRenderer.h"

#include "LiveCode.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace std;
using namespace ci::app;
using namespace reza::ps;
using namespace reza::glsl;

namespace reza {
namespace ps {

SpriteRenderer::SpriteRenderer(
	const WindowRef &window,
	const fs::path &vertexPath,
	const fs::path &fragmentPath,
	reza::ps::ParticleSystemRef &particleSystemRef,
	std::function<void()> superFn,
	std::function<void( GlslParamsRef )> glslUpdatedFn,
	std::function<void( ci::Exception )> glslErrorFn )
	: Renderer( window, vertexPath, fragmentPath, static_cast<SystemRef>( particleSystemRef ), superFn, glslUpdatedFn, glslErrorFn ), mParticleSystemRef( particleSystemRef )
{
}

void SpriteRenderer::setupBatch()
{
	if( mGlslProgRef ) {
		geom::BufferLayout instancePositionDataLayout;
		instancePositionDataLayout.append( geom::Attrib::CUSTOM_0, 4, 0, 0, 1 /* per instance */ );

		geom::BufferLayout instanceVelocityDataLayout;
		instanceVelocityDataLayout.append( geom::Attrib::CUSTOM_1, 4, 0, 0, 1 /* per instance */ );

		for( int i = 0; i < 2; i++ ) {
			gl::VboMeshRef mesh = gl::VboMesh::create( ci::geom::Rect( Rectf( 0.0f, 0.0f, 0.0f, 0.0f ) ) );
			mesh->appendVbo( instancePositionDataLayout, mParticleSystemRef->getPositionVbo( i ) );
			mesh->appendVbo( instanceVelocityDataLayout, mParticleSystemRef->getVelocityVbo( i ) );
			mBatchRef[i] = gl::Batch::create( mesh, mGlslProgRef, mParticleSystemRef->getRendererGlslAttributeMapping() );
		}
		Renderer::setupBatch();
	}
}

void SpriteRenderer::_draw()
{
	if( mBatchRef[mParticleSystemRef->getIterationIndex() & 1] ) {
		mGlslParamsRef->applyUniforms( mGlslProgRef );
		mBatchRef[mParticleSystemRef->getIterationIndex() & 1]->drawInstanced( mParticleSystemRef->getTotal() );
	}
}

} // namespace ps
} // namespace reza
