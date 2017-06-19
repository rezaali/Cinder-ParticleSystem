#include "PlexusSystem.h"

#include "cinder/Rand.h"

#include "LiveCode.h"
#include "Watchdog.h"
#include "cinder/Log.h"

using namespace ci;
using namespace std;
using namespace ci::app;
using namespace reza::glsl;

/*
vector<string> varyings(1);
varyings[PLEXUS_DISTANCE_INDEX] = "tf_plexus";

gl::GlslProg::Format updateFormat;
updateFormat.vertex(vertDataSource)
.feedbackFormat(GL_SEPARATE_ATTRIBS)
.feedbackVaryings(varyings)
.attribLocation("plexus", PLEXUS_DISTANCE_INDEX);
*/

namespace reza {
namespace ps {
PlexusSystem::PlexusSystem(
	const WindowRef &window,
	const fs::path &vertexPath,
	gl::GlslProg::Format &glslFormat,
	ParticleSystemRef &particleSystemRef,
	std::function<void()> superFn,
	std::function<void( reza::glsl::GlslParamsRef )> glslUpdatedFn,
	std::function<void( ci::Exception )> glslErrorFn )
	: System( window, vertexPath, glslFormat, superFn, glslUpdatedFn, glslErrorFn ), mParticleSystemRef( particleSystemRef )
{
}

PlexusSystem::~PlexusSystem()
{
}

void PlexusSystem::setupBuffers()
{
	vector<vec4> distances;

	int index = 0;
	for( int row = 0; row < mTotal; row++ ) {
		for( int col = 0; col < mTotal; col++ ) {
			if( col > row ) {
				distances.emplace_back( vec4( 0.0, row, col, index ) );
				index++;
			}
		}
	}

	mTotal = static_cast<int>( distances.size() );

	mDistancesVbo[0] = gl::Vbo::create( GL_ARRAY_BUFFER, distances.size() * sizeof( vec4 ), distances.data(), GL_DYNAMIC_DRAW );
	mDistancesVbo[1] = gl::Vbo::create( GL_ARRAY_BUFFER, distances.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	for( int i = 0; i < 2; i++ ) {
		mVaos[i] = gl::Vao::create();
		mVaos[i]->bind();

		mDistancesVbo[i]->bind();
		gl::vertexAttribPointer( PLEXUS_DISTANCE_INDEX, 4, GL_FLOAT, GL_FALSE, 0, NULL );
		gl::enableVertexAttribArray( PLEXUS_DISTANCE_INDEX );

		mFeedbackObjs[i] = gl::TransformFeedbackObj::create();
		mFeedbackObjs[i]->bind();
		gl::bindBufferBase( GL_TRANSFORM_FEEDBACK_BUFFER, PLEXUS_DISTANCE_INDEX, mDistancesVbo[i] );
		mFeedbackObjs[i]->unbind();

		mDistanceBufferTextures[i] = gl::BufferTexture::create( mDistancesVbo[i], GL_RGBA32F );
	}
}

void PlexusSystem::updateBuffers()
{
	vector<vec4> distances;

	int index = 0;
	for( int row = 0; row < mTotal; row++ ) {
		for( int col = 0; col < mTotal; col++ ) {
			if( col > row ) {
				distances.emplace_back( vec4( 0.0, row, col, index ) );
				index++;
			}
		}
	}

	mTotal = static_cast<int>( distances.size() );

	mDistancesVbo[0]->bufferData( distances.size() * sizeof( vec4 ), distances.data(), GL_DYNAMIC_DRAW );
	mDistancesVbo[1]->bufferData( distances.size() * sizeof( vec4 ), nullptr, GL_DYNAMIC_DRAW );

	for( int i = 0; i < 2; i++ ) {
		mDistanceBufferTextures[i] = gl::BufferTexture::create( mDistancesVbo[i], GL_RGBA32F );
	}
}

void PlexusSystem::_update()
{
	if( mParticleSystemRef ) {
		gl::ScopedGlslProg scpGlsl( mGlslProgRef );
		gl::ScopedState stateScope( GL_RASTERIZER_DISCARD, true );
		mGlslParamsRef->applyUniforms(mGlslProgRef);
		mGlslProgRef->uniform( "uPositionMass", 0 );
		gl::ScopedVao vaoScope( mVaos[mIterationIndex & 1] );
		mParticleSystemRef->getPositionBufferTextureRef( mIterationIndex & 1 )->bindTexture( 0 );
		mIterationIndex++;
		mFeedbackObjs[mIterationIndex & 1]->bind();
		gl::beginTransformFeedback( GL_POINTS );
		gl::drawArrays( GL_POINTS, 0, mTotal );
		gl::endTransformFeedback();
	}
}

} // namespace ps
} // namespace reza