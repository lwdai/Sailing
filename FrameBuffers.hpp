#pragma once
#include "cs488-framework/OpenGLImport.hpp"

struct FrameBuffers {
    int &m_Width;
    int &m_Height;
    
    GLuint shadowFBO;
    GLuint shadowTexture;
    GLuint shadowDepthBuffer;

    GLuint reflectFBO;
    GLuint reflectTexture;
    GLuint reflectDepthBuffer;
    
    GLuint refractFBO;
    GLuint refractTexture;
    GLuint refractDepthBuffer;
    
    FrameBuffers( int &width, int &height ): m_Width( width ), m_Height( height ) { }
    
    // just repeat opengl routines..
    void prepareWaterFBOs();
    GLuint createFBO();
    GLuint createTexture( int width, int height );
    GLuint createDepthTexture( int width, int height );
    GLuint createDepthBuffer( int width, int height );
    void bindFBO( GLuint fbo, int width, int height );
    void unbindFBO();
};
