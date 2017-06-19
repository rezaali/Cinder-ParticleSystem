#include "TrailSystem.h"

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
TrailSystem::TrailSystem(
	const WindowRef &window,
	const fs::path &vertexPath,
	gl::GlslProg::Format &glslFormat,
	ParticleSystemRef &particleSystemRef,
	std::function<void()> superFn,
	std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn,
	std::function<void( ci::Exception )> glslErrorFn )
	: System( window, vertexPath, glslFormat, superFn, glslUpdatedFn, glslErrorFn ), mParticleSystemRef( particleSystemRef )
{
	mTotal = 10;
}

TrailSystem::~TrailSystem()
{
}

void TrailSystem::setupBuffers()
{
	vector<vec4> positions;
	vector<ivec4> info;

	mPositionsVbo[0] = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof( vec4 ), positions.data(), GL_DYNAMIC_DRAW );
	mPositionsVbo[1] = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );
	mInfoVbo = gl::Vbo::create( GL_ARRAY_BUFFER, info.size() * sizeof( ivec4 ), info.data(), GL_DYNAMIC_DRAW );

	for( int i = 0; i < 2; i++ ) {
		mVaos[i] = gl::Vao::create();
		mVaos[i]->bind();

		mPositionsVbo[i]->bind();
		gl::vertexAttribPointer( TRAILS_POS_INDEX, 4, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray( TRAILS_POS_INDEX );

		mInfoVbo->bind();
		gl::vertexAttribIPointer( TRAILS_INFO_INDEX, 4, GL_INT, 0, NULL );
		gl::enableVertexAttribArray( TRAILS_INFO_INDEX );

		mFeedbackObjs[i] = gl::TransformFeedbackObj::create();
		mFeedbackObjs[i]->bind();
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, TRAILS_POS_INDEX, mPositionsVbo[i] );
		mFeedbackObjs[i]->unbind();

		mPositionBufferTextures[i] = gl::BufferTexture::create( mPositionsVbo[i], GL_RGBA32F );
	}
}

void TrailSystem::updateBuffers()
{
	int ptotal = mParticleSystemRef->getTotal();
	int total = ptotal * mTotal;
	
	vector<ivec4> info( total, ivec4( 0 ) );
	for( int i = 0; i < total; i++ ) {
		info[i][0] = i;
	}
	mInfoVbo->bufferData( info.size() * sizeof( ivec4 ), info.data(), GL_STATIC_DRAW );

	vector<vec4> positions(total, vec4(0.0));
	for (int i = 0; i < 2; i++) {
		mPositionsVbo[i]->bufferData(positions.size() * sizeof(vec4), positions.data(), GL_DYNAMIC_DRAW);
		mPositionBufferTextures[i] = gl::BufferTexture::create(mPositionsVbo[i], GL_RGBA32F);
	}
}

void TrailSystem::_update()
{
	if( mParticleSystemRef ) {
		mIterationIndex = 1 - mIterationIndex;
		gl::ScopedGlslProg updateScope( mGlslProgRef );
		gl::ScopedState stateScope( GL_RASTERIZER_DISCARD, true );
		mGlslParamsRef->applyUniforms( mGlslProgRef );
		mGlslProgRef->uniform( "trailLength", mTotal );
		mGlslProgRef->uniform( "tex_particle_positions", 0 );
		mGlslProgRef->uniform( "tex_trails_positions", 1 );
		gl::ScopedVao vaoScope( mVaos[mIterationIndex] );
		mParticleSystemRef->getPositionBufferTextureRef( mParticleSystemRef->getIterationIndex() & 1 )->bindTexture( 0 );
		mPositionBufferTextures[mIterationIndex & 1]->bindTexture( 1 );
		mFeedbackObjs[mIterationIndex & 1]->bind();
		gl::beginTransformFeedback( GL_POINTS );
		gl::drawArrays( GL_POINTS, 0, mParticleSystemRef->getTotal() * mTotal );
		gl::endTransformFeedback();
	}
}

} // namespace ps
} // namespace reza