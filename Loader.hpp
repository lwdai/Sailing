#pragma once

#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"
#include "SceneNode.hpp"
#include <memory>

class Loader {
    GLuint m_vao_meshData;
    GLuint m_vbo_vertexPositions;
    GLuint m_vbo_vertexNormals;
    GLuint m_vbo_vertexUVs;
    	
    GLint m_positionAttribLocation;
    GLint m_normalAttribLocation;
    GLint m_uvAttribLocation;
    
    ShaderProgram m_shader;
    std::shared_ptr<SceneNode> m_rootNode;
   
    void createShaderProgram();
    void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
    
public:
    
    void prepareShaderPrograms();
    
    ShaderProgram *getSceneShader();
    
    void loadMeshData( BatchInfoMap &batchInfoMap, GLuint &vao_meshData );
    
    std::shared_ptr<SceneNode> getRootNode();
    
    void processLuaSceneFile( const char *m_luaSceneFile );
    
    GLuint loadTexture( const char *bmpPath );
    
};
