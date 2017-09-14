#include "Renderer.hpp"
#include "Loader.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp> 

#include <iostream>
using namespace std;
using namespace glm;

void Renderer::init( Loader &loader ) {
    glClearColor(0.35, 0.35, 0.35, 1.0);

    // only 1 shader program..
    m_shader = loader.getSceneShader();
    
    m_rootNode = loader.getRootNode();
    loader.loadMeshData( m_batchInfoMap, m_vao_meshData );
    
    terrainTexture = loader.loadTexture( "Assets/terrain.bmp" );
    shipTexture = loader.loadTexture( "Assets/wood.bmp" );
    cerr << "texture loaded " << endl;
    
    FBOs.prepareWaterFBOs();
    FBOs.unbindFBO();
} // init

void Renderer::renderScene() {
    glEnable( GL_DEPTH_TEST );
    
    //====================
    // render reflection:
    render_mode = RENDER_REFLECTION;
    FBOs.bindFBO( FBOs.reflectFBO, m_Width, m_Height );
    
    // necessary to clear; don't know why...
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glActiveTexture( GL_TEXTURE0 + 0 );
    renderSceneGraph( *m_rootNode );        

    //=====================
    // render refraction
    render_mode = RENDER_REFRACTION;
    FBOs.bindFBO( FBOs.refractFBO, m_Width, m_Height );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glActiveTexture( GL_TEXTURE0 + 1 );
    glBindTexture( GL_TEXTURE_2D, FBOs.reflectTexture );
    glActiveTexture( GL_TEXTURE0 + 0 );
    renderSceneGraph( *m_rootNode );
    
    //=====================
    // render depth map (shadow)
    // doens't work; something is rendered but has no effect
    render_mode = RENDER_DEPTH_MAP;
    FBOs.bindFBO( FBOs.shadowFBO, m_Width, m_Height );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glActiveTexture( GL_TEXTURE0 + 0 );
    // no texture needed?
    renderSceneGraph( *m_rootNode );
    
    //=====================
    // render scene:
    FBOs.unbindFBO();
    
    render_mode = RENDER_SCENE;
    // shadow
    glActiveTexture( GL_TEXTURE0 + 2 );
    glBindTexture( GL_TEXTURE_2D, FBOs.shadowTexture );
    
    glActiveTexture( GL_TEXTURE0 + 1 );
    glBindTexture( GL_TEXTURE_2D, FBOs.refractTexture );
    
    glActiveTexture( GL_TEXTURE0 + 0 );
    renderSceneGraph( *m_rootNode );
} // renderScene()

//===================================================================
// private methods


void Renderer::renderSceneGraph( SceneNode &root ) {
    // Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);
	
	for (SceneNode * node : root.children) {
	    renderSceneRecur(node, root.trans * root.rot_mat);	    
	}

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
} // renderSceneGraph

/**
 *  Recursively render SceneNodes. Called by renderSceneGraph
 */
void Renderer::renderSceneRecur( SceneNode *node, mat4 trans_acc ) {
    
    if (node->m_nodeType == NodeType::GeometryNode) {
        GeometryNode * geometryNode = static_cast<GeometryNode *>(node);
        
        mat4 view_adjust;
        if ( RENDER_REFLECTION == render_mode ) {
            view_adjust = m_camera->getReflectionView();
            glEnable( GL_CLIP_DISTANCE0 );
        } else if ( RENDER_REFRACTION == render_mode ) {
            // TODO: doesn't work... don't know how to adjust angle
            //view_adjust = m_camera->getRefractionView();
            view_adjust = m_view;
            glEnable( GL_CLIP_DISTANCE0 );
        } else if ( RENDER_DEPTH_MAP == render_mode ) {
            view_adjust = Camera::getLightView( m_light.position );
            glEnable( GL_CLIP_DISTANCE0 ); // ?
        } else {
            view_adjust = m_view;
            glEnable( GL_CLIP_DISTANCE0 );
        } // if
        updateShaderUniforms( *m_shader, *geometryNode, view_adjust * trans_acc );
        BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];

        GLuint texture;
        if ( "terrain" == node->m_name ) {
            texture = terrainTexture;
            setRenderWater( false );
        } else if ( "ship" == node->m_name ) {
            texture = shipTexture;
            setRenderWater( false );
        } else if ( "water" == node->m_name ) {
            // skip water itself when rendering reflection
            
            setRenderWater( true );
            if ( RENDER_REFLECTION == render_mode ||
                 RENDER_REFRACTION == render_mode ) goto L_RECUR;
        }
        glBindTexture( GL_TEXTURE_2D, texture );
        m_shader->enable();
	    glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
	    m_shader->disable();
	  
	 L_RECUR:;  
	    for (SceneNode *child : node->children) {
            renderSceneRecur(child, trans_acc * node->trans * node->rot_mat);
        } 
        
    } else if (node->m_nodeType == NodeType::JointNode) {
        JointNode *jointNode = static_cast<JointNode *>(node);
        trans_acc = 
            (trans_acc * jointNode->trans * jointNode->rot_mat *
            rotate(float(radians(jointNode->m_joint_x.init)), vec3(1.f, 0.f, 0.f)) *
            rotate(float(radians(jointNode->m_joint_y.init)), vec3(0.f, 1.f, 0.f)) );
            
        for (SceneNode *child : node->children) {
            renderSceneRecur(child, trans_acc);
        } 
    }  // if
}

//----------------------------------------------------------------------------------------
void Renderer::uploadCommonSceneUniforms() {
    //cerr << "uploadCommonSceneUniforms0" << endl;
	m_shader->enable();
	{
	    GLint location = m_shader->getUniformLocation("screenSize");
		glUniform2f(location, (float) m_Width, (float) m_Height);
		//glUniform2f(location, (float) 1024, (float) 768);
		CHECK_GL_ERRORS;
		
	    //cerr << "uploadCommonSceneUniforms1" << endl;
		//-- Set Perpsective matrix uniform for the scene:
		location = m_shader->getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;
		
		//cerr << "persp uploaded" << endl;
		location = m_shader->getUniformLocation("render_water");	
		glUniform1i( location, 0 );
		CHECK_GL_ERRORS;

        
		//-- Set LightSource uniform for the scene:
		{
			location = m_shader->getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader->getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}
        //cerr << "light uploaded" << endl;
		//-- Set background light ambient intensity
		{
			location = m_shader->getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.5f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
		
		{ 
		    counter += 1;
		    location = m_shader->getUniformLocation("counter");
			glUniform1i(location, counter);
			CHECK_GL_ERRORS;
	    }
	    
	    { 
		    location = m_shader->getUniformLocation("reflection");
			glUniform1f(location, reflection / 100.0);
			CHECK_GL_ERRORS;
	    }
	    
	    { 
		    location = m_shader->getUniformLocation("transparency");
			glUniform1f(location, transparency / 100.0);
			CHECK_GL_ERRORS;
	    }
	    
	    { 
		    location = m_shader->getUniformLocation("wave");
			glUniform1f(location, waveStr / 100.0);
			CHECK_GL_ERRORS;
	    }
		
	}
	m_shader->disable();
}

void Renderer::updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix ) {

	shader.enable();
	{
	    //-- Switch to current render mode:
		GLint location = shader.getUniformLocation("render_mode");	
	    glUniform1i( location, render_mode );
	    CHECK_GL_ERRORS;
	    
	    //cerr << "render_mode updated " << endl;
		
		//-- Set ModelView matrix:
		location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans * node.rot_mat * node.scale_mat;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;
        
        //-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;
		
        //-- Set Material values:
		location = shader.getUniformLocation("material.kd");
		// change color for picked objects
		vec3 kd = node.material.kd;
		
		  
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
        
		CHECK_GL_ERRORS;

	}
	shader.disable();
} // updateShaderUniforms

void Renderer::initLightSources() {
	// World-space position
	m_light.position = vec3(20.0f, 10.0f, 20.f);
	m_light.rgbIntensity = vec3(0.99f); // White light
} // initLightSources


void Renderer::initViewMatrix() {
	m_view = m_camera->getView();
} // initViewMatrix

//----------------------------------------------------------------------------------------
void Renderer::initPerspectiveMatrix() {
	float aspect = ((float)m_Width) / m_Height;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.2f, 40.0f);
} // initPerspectiveMatrix

void Renderer::linkCamera( Camera &camera ) {
    m_camera = &camera;
}

void Renderer::updateEnv() {
    initViewMatrix();
    initPerspectiveMatrix();
    initLightSources();
} // updateEnv

void Renderer::setRenderWater( bool renderWater ) {
    m_shader->enable();
    GLint location = m_shader->getUniformLocation("render_water");	
	glUniform1i( location, ( int ) renderWater );
	CHECK_GL_ERRORS;
	m_shader->enable();
}


















