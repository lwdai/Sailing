#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "SceneNode.hpp"
#include "Renderer.hpp"
#include "Loader.hpp"
#include "Ship.hpp"
#include "Camera.hpp"
#include "Renderer.hpp"
#include <glm/glm.hpp>
#include <vector>

struct JointNode;

class Sailing : public CS488Window {
public:
	Sailing(const std::string & luaSceneFile);
	virtual ~Sailing();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	std::string m_luaSceneFile;
	
	Loader loader;
	Renderer renderer; 
	
	Ship ship;
	Camera camera;
    
    bool mouse_right_press;
		
    // the mouse position at the last frame
	double mouse_x;
	double mouse_y;
	
	int *transparency;
	int *reflection;
	int *waveStr;
	
};



