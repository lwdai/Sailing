#pragma once

#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "Loader.hpp"
#include "GeometryNode.hpp"
#include "SceneNode.hpp"
#include "Camera.hpp"
#include "FrameBuffers.hpp"

#include <glm/glm.hpp>
#include <memory>

#define RENDER_SCENE 1
#define RENDER_REFLECTION 2
#define RENDER_REFRACTION 3
#define RENDER_DEPTH_MAP 4


struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

class Renderer {
    glm::mat4 m_perpsective;
	glm::mat4 m_view;

	LightSource m_light;
	
	GLuint m_vao_meshData;
	
	ShaderProgram *m_shader;
	
	BatchInfoMap m_batchInfoMap;
    std::shared_ptr<SceneNode> m_rootNode;
    
    GLuint terrainTexture;
    GLuint shipTexture;   
       
    int &m_Width;
    int &m_Height;

    Camera *m_camera;
    
    int render_mode;
    
    int counter; // timer
   
    
    FrameBuffers FBOs;

//==============================================================	
    void initViewMatrix();
	void initLightSources();
	void initPerspectiveMatrix();
	
	void renderSceneGraph(SceneNode &node);
	
	void renderSceneRecur(SceneNode *node, glm::mat4 trans_acc);
	
	void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix );
    
    void setRenderWater( bool renderWater );
    
public:
    
    int transparency;
	int reflection;
	int waveStr;
        
    Renderer( int &width, int &height ): m_Width( width ), m_Height( height ),
        FBOs( width, height ), counter( 0 ), transparency( 50 ), 
        reflection( 50 ), waveStr( 50 ) { }
    void init( Loader &loader );
    
    void linkCamera( Camera &camera );
    
    // called every frame to prepare uniform values
    void updateEnv();
    
    void uploadCommonSceneUniforms();
    void renderScene();

};
