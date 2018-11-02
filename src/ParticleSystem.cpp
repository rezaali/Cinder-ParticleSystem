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
	vector<vec4> positions;
	vector<vec4> velocities;
	vector<vec4> colors;
	vector<vec4> orientations;

	mPositionVbo[0] = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof( vec4 ), positions.data(), GL_DYNAMIC_DRAW );
	mPositionVbo[1] = gl::Vbo::create( GL_ARRAY_BUFFER, positions.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	mVelocityVbo[0] = gl::Vbo::create( GL_ARRAY_BUFFER, velocities.size() * sizeof( vec4 ), velocities.data(), GL_DYNAMIC_DRAW );
	mVelocityVbo[1] = gl::Vbo::create( GL_ARRAY_BUFFER, velocities.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	mColorVbo[0] = gl::Vbo::create( GL_ARRAY_BUFFER, colors.size() * sizeof( vec4 ), colors.data(), GL_DYNAMIC_DRAW );
	mColorVbo[1] = gl::Vbo::create( GL_ARRAY_BUFFER, colors.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	mOrientationVbo[0] = gl::Vbo::create( GL_ARRAY_BUFFER, orientations.size() * sizeof( vec4 ), orientations.data(), GL_DYNAMIC_DRAW );
	mOrientationVbo[1] = gl::Vbo::create( GL_ARRAY_BUFFER, orientations.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	for( int i = 0; i < 2; i++ ) {
		mVaos[i] = gl::Vao::create();
		mVaos[i]->bind();

		mPositionVbo[i]->bind();
		gl::vertexAttribPointer( PARTICLES_POS_INDEX, 4, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray( PARTICLES_POS_INDEX );

		mVelocityVbo[i]->bind();
		gl::vertexAttribPointer( PARTICLES_VEL_INDEX, 4, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray( PARTICLES_VEL_INDEX );

		mColorVbo[i]->bind();
		gl::vertexAttribPointer( PARTICLES_CLR_INDEX, 4, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray( PARTICLES_CLR_INDEX );

		mOrientationVbo[i]->bind();
		gl::vertexAttribPointer( PARTICLES_ORI_INDEX, 4, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray( PARTICLES_ORI_INDEX );

		mFeedbackObjs[i] = gl::TransformFeedbackObj::create();
		mFeedbackObjs[i]->bind();
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PARTICLES_POS_INDEX, mPositionVbo[i] );
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PARTICLES_VEL_INDEX, mVelocityVbo[i] );
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PARTICLES_CLR_INDEX, mColorVbo[i] );
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PARTICLES_ORI_INDEX, mOrientationVbo[i] );
		mFeedbackObjs[i]->unbind();

		mPositionBufferTextures[i] = gl::BufferTexture::create( mPositionVbo[i], GL_RGBA32F );
		mVelocityBufferTextures[i] = gl::BufferTexture::create( mVelocityVbo[i], GL_RGBA32F );
		mColorBufferTextures[i] = gl::BufferTexture::create( mColorVbo[i], GL_RGBA32F );
		mOrientationBufferTextures[i] = gl::BufferTexture::create( mOrientationVbo[i], GL_RGBA32F );
	}
}

void ParticleSystem::setPositions( std::vector<ci::vec3> *positions )
{
	mIterationIndex = 0;
	int total = int( positions->size() );
	if( total != mTotal ) {
		setTotal( total );
	}

	vector<vec4> ps( mTotal );
	for( int i = 0; i < mTotal; i++ ) {
		vec3 p = ( *positions )[i];
		ps[i] = vec4( p.x, p.y, p.z, float( i ) );
	}

	mPositionVbo[0]->bufferData( ps.size() * sizeof( vec4 ), ps.data(), GL_DYNAMIC_DRAW );
	mPositionVbo[1]->bufferData( ps.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	for( int i = 0; i < 2; i++ ) {
		mPositionBufferTextures[i] = gl::BufferTexture::create( mPositionVbo[i], GL_RGBA32F );
	}
}

void ParticleSystem::updateBuffers()
{
	vector<vec4> positions( mTotal );
	vector<vec4> velocities( mTotal, vec4( 0.0f ) );
	vector<vec4> colors( mTotal, vec4( 0.0f ) );
	vector<vec4> orientations( mTotal, vec4( 0.0f ) );

	float randSize = 5.0f;
	float velSize = 1.0;
	for( int i = 0; i < mTotal; i++ ) {
		positions[i] = vec4(
			randFloat( -randSize, randSize ),
			randFloat( -randSize, randSize ),
			randFloat( -randSize, randSize ),
			float( i ) );

		velocities[i] = vec4(
			randFloat( -velSize, velSize ),
			randFloat( -velSize, velSize ),
			randFloat( -velSize, velSize ),
			randFloat( -velSize, velSize ) );

		colors[i] = vec4(
			randFloat(),
			randFloat(),
			randFloat(),
			randFloat() );

		orientations[i] = vec4(
			randFloat(),
			randFloat(),
			randFloat(),
			randFloat() );
	}

	mPositionVbo[0]->bufferData( positions.size() * sizeof( vec4 ), positions.data(), GL_DYNAMIC_DRAW );
	mPositionVbo[1]->bufferData( positions.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	mVelocityVbo[0]->bufferData( velocities.size() * sizeof( vec4 ), velocities.data(), GL_DYNAMIC_DRAW );
	mVelocityVbo[1]->bufferData( velocities.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	mColorVbo[0]->bufferData( colors.size() * sizeof( vec4 ), colors.data(), GL_DYNAMIC_DRAW );
	mColorVbo[1]->bufferData( colors.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	mOrientationVbo[0]->bufferData( orientations.size() * sizeof( vec4 ), orientations.data(), GL_DYNAMIC_DRAW );
	mOrientationVbo[1]->bufferData( orientations.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	for( int i = 0; i < 2; i++ ) {
		mPositionBufferTextures[i] = gl::BufferTexture::create( mPositionVbo[i], GL_RGBA32F );
		mVelocityBufferTextures[i] = gl::BufferTexture::create( mVelocityVbo[i], GL_RGBA32F );
		mColorBufferTextures[i] = gl::BufferTexture::create( mColorVbo[i], GL_RGBA32F );
		mOrientationBufferTextures[i] = gl::BufferTexture::create( mOrientationVbo[i], GL_RGBA32F );
	}
}

void ParticleSystem::_update()
{
	if( mGlslProgRef ) {
		gl::ScopedGlslProg updateScope( mGlslProgRef );
		gl::ScopedState stateScope( GL_RASTERIZER_DISCARD, true );

		mGlslParamsRef->applyUniforms( mGlslProgRef );

		mGlslProgRef->uniform( "uPositionId", 0 );
		mGlslProgRef->uniform( "uVelocityMass", 1 );

		for( int i = mIterationsPerFrame; i != 0; --i ) {
			gl::ScopedVao vaoScope( mVaos[mIterationIndex & 1] );
			mPositionBufferTextures[mIterationIndex & 1]->bindTexture( 0 );
			mVelocityBufferTextures[mIterationIndex & 1]->bindTexture( 1 );
			mIterationIndex++;
			mFeedbackObjs[mIterationIndex & 1]->bind();
			gl::beginTransformFeedback( GL_POINTS );
			gl::drawArrays( GL_POINTS, 0, mTotal );
			gl::endTransformFeedback();
			mFeedbackObjs[mIterationIndex & 1]->unbind();
			mPositionBufferTextures[mIterationIndex & 1]->unbindTexture();
			mVelocityBufferTextures[mIterationIndex & 1]->unbindTexture();
		}
	}
}

void ParticleSystem::bind()
{
	mVaos[mIterationIndex & 1]->bind();
}

} // namespace ps
} // namespace reza