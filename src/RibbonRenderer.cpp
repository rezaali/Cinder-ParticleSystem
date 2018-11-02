#include "RibbonRenderer.h"

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

RibbonRenderer::RibbonRenderer(
	const WindowRef &window,
	const fs::path &vertexPath,
	const fs::path &fragmentPath,
	const fs::path &geometryPath,
	reza::ps::ParticleSystemRef particleSystemRef,
	reza::ps::TrailSystemRef trailSystemRef,
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
	  mTrailSystemRef( trailSystemRef ),
	  mGeometryPath( geometryPath )
{
	mGlslProgFormat = gl::GlslProg::Format();
}

void RibbonRenderer::setupGlsl()
{
	auto vertex = mVertexPath;
	auto fragment = mFragmentPath;
	auto geometry = mGeometryPath;

	wd::unwatch( vertex );
	wd::unwatch( fragment );
	wd::unwatch( geometry );

	auto cb = [this, vertex, fragment, geometry]( const fs::path &path ) {
		reza::live::glsl(
			vertex,
			fragment,
			geometry,
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
	wd::watch( geometry, cb );
}

void RibbonRenderer::setupBatch()
{
	int total = mTrailSystemRef->getTotal();

	vector<vec3> positions( total );
	vector<vec2> texcoods( total );

	for( int i = 0; i < total; i++ ) {
		texcoods[i] = vec2( float( i ), 0.0f );
		positions[i] = vec3( 0.0f, 0.0f, 0.0f );
	}

	mVboMeshRef = gl::VboMesh::create( total, GL_POINTS, { gl::VboMesh::Layout().attrib( geom::POSITION, 3 ).attrib( geom::TEX_COORD_0, 2 ) } );
	mVboMeshRef->bufferAttrib( geom::POSITION, sizeof( vec3 ) * positions.size(), positions.data() );
	mVboMeshRef->bufferAttrib( geom::TEX_COORD_0, sizeof( vec2 ) * texcoods.size(), texcoods.data() );
	mBatchRef = gl::Batch::create( mVboMeshRef, mGlslProgRef );
	Renderer::setupBatch();
}

void RibbonRenderer::_draw()
{
	gl::ScopedGlslProg glslScope( mGlslProgRef );
	mGlslProgRef->uniform( "trailLength", mTrailSystemRef->getTotal() );
	int index = mTrailSystemRef->getIterationIndex() & 1;
	mGlslProgRef->uniform( "tex_position", 0 );
	mTrailSystemRef->getPositionBufferTextureRef( index )->bindTexture( 0 );
    
    mGlslProgRef->uniform( "tex_position", 0 );
    mTrailSystemRef->getPositionBufferTextureRef( index )->bindTexture( 0 );
    
    mGlslProgRef->uniform( "velocity_mass", 1 );
    mParticleSystemRef->getVelocityBufferTextureRef( index )->bindTexture( 1 );
    
    mGlslProgRef->uniform( "color", 2 );
    mParticleSystemRef->getColorBufferTextureRef( index )->bindTexture( 2 );
    
    mGlslProgRef->uniform( "orientation", 3 );
    mParticleSystemRef->getOrientationBufferTextureRef( index )->bindTexture( 3 );
    
	mBatchRef->drawInstanced( mParticleSystemRef->getTotal() );
}

} // namespace ps
} // namespace reza
