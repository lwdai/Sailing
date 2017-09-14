/**
 * Learned (stole?) how to use FBOs from  
 * http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
 * and
 * https://open.gl/framebuffers
 * However the code they provide just don't work.
 
 * I can't recall what I have changed to make this work.
 */ 


#include "FrameBuffers.hpp"
#include <iostream>
#include <cassert>

using namespace std;

void FrameBuffers::prepareWaterFBOs() {
    // shadow
    shadowFBO = createFBO();
  
    //glDrawBuffer( GL_NONE );
    shadowTexture = createTexture( m_Width, m_Height );
    shadowDepthBuffer = createDepthBuffer( m_Width, m_Height );  
    glDrawBuffer( GL_COLOR_ATTACHMENT0 );
    
    assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );
    
    // reflection
    reflectFBO = createFBO();
    reflectTexture = createTexture( m_Width, m_Height );
    reflectDepthBuffer = createDepthBuffer( m_Width, m_Height );  
    glDrawBuffer( GL_COLOR_ATTACHMENT0 );
    assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE ); 
    
    // refraction
    refractFBO = createFBO();
    refractTexture = createTexture( m_Width, m_Height );
    refractDepthBuffer = createDepthBuffer( m_Width, m_Height );  
    glDrawBuffer( GL_COLOR_ATTACHMENT0 );
    assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE );    
}

GLuint FrameBuffers::createFBO() {
    GLuint fbo;
    glGenFramebuffers( 1, &fbo );
    glBindFramebuffer( GL_FRAMEBUFFER, fbo );
    return fbo;
}

GLuint FrameBuffers::createTexture( int width, int height ) {
    GLuint texture;
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );
    
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, 
        height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );    

    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0 );
    
    assert( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE ); 
    return texture;
}

// doesn't work
GLuint FrameBuffers::createDepthTexture( int width, int height ) {
     // create depth texture attachment
    GLuint depth_texture;
    glGenTextures( 1, &depth_texture );
    glBindTexture( GL_TEXTURE_2D, depth_texture );
    
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, 
        height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
        
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST ); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0 );
    
    return depth_texture;
}

GLuint FrameBuffers::createDepthBuffer( int width, int height ) {
    GLuint depth_buffer;
    glGenRenderbuffers( 1, &depth_buffer );
    glBindRenderbuffer( GL_RENDERBUFFER, depth_buffer );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 
        width, height );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, depth_buffer ); 
        
    return depth_buffer;  
}

void FrameBuffers::bindFBO( GLuint fbo, int width, int height ) {
    glBindTexture( GL_TEXTURE_2D, 0 );
    glBindFramebuffer( GL_FRAMEBUFFER, fbo );
    
    // fixed a bug when resizing the window
    
    //glViewport( 0, 0, width, height );
    glViewport( 0, 0, 1024, 768 );
}

void FrameBuffers::unbindFBO() {
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, m_Width, m_Height );
    //glViewport( 0, 0, 1024, 768 );
}


