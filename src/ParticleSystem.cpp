#include "ParticleSystem.h"

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
ParticleSystem::ParticleSystem(
	const WindowRef &window,
	const fs::path &vertexPath,
	gl::GlslProg::Format &glslFormat,
	std::function<void()> superFn,
	std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn,
	std::function<void( ci::Exception )> glslErrorFn )
	: System( window, vertexPath, glslFormat, superFn, glslUpdatedFn, glslErrorFn )
{
}

ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::setupBuffers()
{
	CI_LOG_D("SETTING UP BUFFERS"); 
	vector<vec4> positions;
	vector<vec4> velocities;
	vector<ivec4> info;

	mPositionVbo[0] = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof( vec4 ), positions.data(), GL_DYNAMIC_DRAW );
	mPositionVbo[1] = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );
	mVelocityVbo[0] = gl::Vbo::create( GL_ARRAY_BUFFER, velocities.size() * sizeof( vec4 ), velocities.data(), GL_DYNAMIC_DRAW );
	mVelocityVbo[1] = gl::Vbo::create( GL_ARRAY_BUFFER, velocities.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	mInfo[0] = gl::Vbo::create( GL_ARRAY_BUFFER, info.size() * sizeof( ivec4 ), info.data(), GL_DYNAMIC_DRAW );
	mInfo[1] = gl::Vbo::create( GL_ARRAY_BUFFER, info.size() * sizeof( ivec4 ), nullptr, GL_DYNAMIC_DRAW );

	for( int i = 0; i < 2; i++ ) {
		mVaos[i] = gl::Vao::create();
		mVaos[i]->bind();

		mPositionVbo[i]->bind();
		gl::vertexAttribPointer( PARTICLES_POS_INDEX, 4, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray( PARTICLES_POS_INDEX );

		mVelocityVbo[i]->bind();
		gl::vertexAttribPointer( PARTICLES_VEL_INDEX, 4, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray( PARTICLES_VEL_INDEX );

		mInfo[i]->bind();
		gl::vertexAttribIPointer( PARTICLES_INFO_INDEX, 4, GL_INT, 0, NULL );
		gl::enableVertexAttribArray( PARTICLES_INFO_INDEX );

		mFeedbackObjs[i] = gl::TransformFeedbackObj::create();
		mFeedbackObjs[i]->bind();
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PARTICLES_POS_INDEX, mPositionVbo[i] );
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PARTICLES_VEL_INDEX, mVelocityVbo[i] );
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PARTICLES_INFO_INDEX, mInfo[i] );
		mFeedbackObjs[i]->unbind();
		mPositionBufferTextures[i] = gl::BufferTexture::create( mPositionVbo[i], GL_RGBA32F );
		mVelocityBufferTextures[i] = gl::BufferTexture::create( mVelocityVbo[i], GL_RGBA32F );
	}
}

void ParticleSystem::updateBuffers()
{
	vector<vec4> positions( mTotal );
	vector<vec4> velocities( mTotal, vec4( 0.0f ) );
	vector<ivec4> infos( mTotal, ivec4( 0 ) );

	float randSize = 5.0f;
	float velSize = 1.0;
	for( int i = 0; i < mTotal; i++ ) {
		positions[i] = vec4(
			randFloat( -randSize, randSize ),
			randFloat( -randSize, randSize ),
			randFloat( -randSize, randSize ),
			randFloat() );

		velocities[i] = vec4(
			randFloat( -velSize, velSize ),
			randFloat( -velSize, velSize ),
			randFloat( -velSize, velSize ),
			randFloat( -velSize, velSize ) );

		infos[i][0] = i; //ID
		infos[i][1] = 0;
		infos[i][2] = 0;
		infos[i][3] = 0;
	}

	mPositionVbo[0]->bufferData( positions.size() * sizeof( vec4 ), positions.data(), GL_DYNAMIC_DRAW );
	mPositionVbo[1]->bufferData( positions.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );
	mVelocityVbo[0]->bufferData( velocities.size() * sizeof( vec4 ), velocities.data(), GL_DYNAMIC_DRAW );
	mVelocityVbo[1]->bufferData( velocities.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );
	mInfo[0]->bufferData( infos.size() * sizeof( ivec4 ), infos.data(), GL_DYNAMIC_DRAW );
	mInfo[1]->bufferData( infos.size() * sizeof( ivec4 ), nullptr, GL_DYNAMIC_DRAW );

	for( int i = 0; i < 2; i++ ) {
		mPositionBufferTextures[i] = gl::BufferTexture::create( mPositionVbo[i], GL_RGBA32F );
		mVelocityBufferTextures[i] = gl::BufferTexture::create( mVelocityVbo[i], GL_RGBA32F );
	}
}

void ParticleSystem::_update()
{
	if( mGlslProgRef ) {
		gl::ScopedGlslProg updateScope( mGlslProgRef );
		gl::ScopedState stateScope( GL_RASTERIZER_DISCARD, true );

		mGlslParamsRef->applyUniforms( mGlslProgRef );

		mGlslProgRef->uniform( "uPositionMass", 0 );
		mGlslProgRef->uniform( "uVelocity", 1 );

		for( int i = mIterationsPerFrame; i != 0; --i ) {
			gl::ScopedVao vaoScope( mVaos[mIterationIndex & 1] );
			//Bind Buffers
			mPositionBufferTextures[mIterationIndex & 1]->bindTexture( 0 );
			mVelocityBufferTextures[mIterationIndex & 1]->bindTexture( 1 );
			//Increment TF
			mIterationIndex++;
			mFeedbackObjs[mIterationIndex & 1]->bind();
			gl::beginTransformFeedback( GL_POINTS );
			gl::drawArrays( GL_POINTS, 0, mTotal );
			gl::endTransformFeedback();
		}
	}
}

void ParticleSystem::bind()
{
	mVaos[mIterationIndex & 1]->bind();
}

} // namespace ps
} // namespace reza