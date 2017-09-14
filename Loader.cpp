#include "Loader.hpp"
#include "scene_lua.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include <iostream>

using namespace glm;
using namespace std;

void Loader::prepareShaderPrograms() {
	createShaderProgram();
	
	// TODO: more shaders?
		
	enableVertexShaderInputSlots();	
} // prepareShaderPrograms



ShaderProgram *Loader::getSceneShader() {
    return & m_shader;
} // getSceneShader



// load obj files, init batchInfo map, and upload all vertex data to VBOs in GPU
void Loader::loadMeshData( BatchInfoMap &batchInfoMap, GLuint &vao_meshData ) {
    unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{			
			// WTF, every obj must have an uv map..
			("Assets/terrain.obj" ),
			("Assets/water.obj" ),
			("Assets/ship.obj" )
	});
	
    // Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap( batchInfoMap );
	
    // Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();
	
	vao_meshData = m_vao_meshData;
} // loadMeshData


std::shared_ptr<SceneNode> Loader::getRootNode() {
    return m_rootNode;
}


void Loader::processLuaSceneFile( const char *m_luaSceneFile ) {
    m_rootNode = std::shared_ptr<SceneNode>( import_lua( m_luaSceneFile ) );
	if (!m_rootNode) {
		std::cerr << "Could not open " << m_luaSceneFile << std::endl;
	}
} // processLuaSceneFile 

/**
 * Stolen from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
 *  with modification
 */
GLuint Loader::loadTexture( const char *bmpPath ) {
    GLuint textureID;
      
    int width, height;

    unsigned char * data;

    FILE * file;
    file = fopen( bmpPath, "rb" );

    if ( file == NULL ) return 0;
    
    unsigned char header[54];
    if ( fread(header, 1, 54, file)!=54 ){ // 54 bytes header
        printf("Not a vlie BMP file\n");
        return -1;
    }
    
    // TODO: read width and height from file
    width = 1024;
    height = 1024;
    data = new unsigned char[width * height * 3];
    // I'll just make all .bmp files have this dimension
      
    int total = fread( data, 1, width * height * 3, file );
    fclose( file );
    cerr << ".bmp bytes read = " << total << endl;
    glGenTextures( 1, &textureID );
    
    glBindTexture( GL_TEXTURE_2D, textureID );
    
    // TODO: filtering?
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    
    delete[] data;

    return textureID;   

} // loadTexture

//================================================================
// private methods

void Loader::createShaderProgram() {
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( "Assets/VertexShader.vs" );
	m_shader.attachFragmentShader( "Assets/FragmentShader.fs" );
	m_shader.link();
	
	// set texture unit
	m_shader.enable();
	GLuint textureLocation = m_shader.getUniformLocation( "mTexture" );
	GLuint backgroundLocation = m_shader.getUniformLocation( "background" );
	
	// depth map doesn't work
	//GLuint depthMapLocation = m_shader.getUniformLocation( "depthMap" );
		
	glUniform1i( textureLocation, 0 );
	glUniform1i( backgroundLocation, 1 );
	//glUniform1i( depthMapLocation, 2 );
	
	m_shader.disable();
	

} // createShaderProgram

void Loader::enableVertexShaderInputSlots() {

    glGenVertexArrays(1, &m_vao_meshData);
    
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);
		
		// also, for texture mapping
		m_uvAttribLocation = m_shader.getAttribLocation("textureCoords");
		glEnableVertexAttribArray(m_uvAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
} // enableVertexShaderInputSlots

void Loader::uploadVertexDataToVbos( const MeshConsolidator & meshConsolidator ) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
	
	cout << meshConsolidator.getNumVertexPositionBytes() << endl;

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
	
	// Generate VBO to store all texture UV data for texture mapping
	{
		glGenBuffers(1, &m_vbo_vertexUVs);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUVs);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexUVBytes(),
				meshConsolidator.getVertexUVDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
} // uploadVertexDataToVbos

void Loader::mapVboDataToVertexShaderInputLocations() {
	// Bind VAO in order to record the data mapping.
    glBindVertexArray(m_vao_meshData);

    // Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
    // "position" vertex attribute location for any bound vertex shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
    glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    // Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
    // "normal" vertex attribute location for any bound vertex shader program.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
    glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	
	// map texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexUVs);
    glVertexAttribPointer(m_uvAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    //-- Unbind target, and restore default values:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    CHECK_GL_ERRORS;
} // mapVboDataToVertexShaderInputLocations








