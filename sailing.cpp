#include "sailing.hpp"
#include "scene_lua.hpp"
#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp> 

#include <iostream>
using namespace std;
using namespace glm;

static bool show_gui = true;


//----------------------------------------------------------------------------------------
// Constructor
Sailing::Sailing(const std::string & luaSceneFile)
	: m_luaSceneFile( luaSceneFile ), 
	renderer( m_framebufferWidth, m_framebufferHeight )	,
	transparency( &(renderer.transparency) ),
	reflection( &(renderer.reflection) ),
	waveStr( &(renderer.waveStr) )
{

}

//----------------------------------------------------------------------------------------
// Destructor
Sailing::~Sailing()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void Sailing::init()
{
	loader.prepareShaderPrograms();
	
	loader.processLuaSceneFile( m_luaSceneFile.c_str() );
	cerr << "process lua" << endl;
	
	renderer.init( loader );
	cerr << "renderer init" << endl;
	
	ship.init( loader );
    cerr << "ship init " << endl;
	
	camera.linkShip( ship );
	cerr << "link ship " << endl;
	renderer.linkCamera( camera );
	
	glfwGetCursorPos(m_window, &mouse_x, &mouse_y);
}


//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void Sailing::appLogic()
{
	renderer.uploadCommonSceneUniforms();
	
	if (ImGui::IsMouseReleased(GLFW_MOUSE_BUTTON_RIGHT)) {
	    mouse_right_press = false;
	} // if
	
	ship.move();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void Sailing::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);
	
	if (ImGui::BeginMainMenuBar()) {
	    if (ImGui::BeginMenu("Application")) {
            if (ImGui::MenuItem("Quit", "Q")) {
                glfwSetWindowShouldClose(m_window, GL_TRUE);
            }          
            ImGui::EndMenu();
        } // if     
        ImGui::EndMainMenuBar();
    } // if


	ImGui::Begin("Panel", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);
		
        if ( ImGui::Button("Quit ( Q )") ) {
           glfwSetWindowShouldClose(m_window, GL_TRUE);                     
        } // if 
        
		ImGui::PushID(1);	
		ImGui::SliderInt("Transparency", transparency, 1, 99 );
		ImGui::PopID();	
		
		ImGui::PushID(2);
		ImGui::SliderInt("Reflection", reflection, 1, 99 );
		ImGui::PopID();
		
		ImGui::PushID(3);
		ImGui::SliderInt("Wave Strength", waveStr, 1, 99 );
		ImGui::PopID();		
			
		ImGui::Text("Framerate: %.1f FPS", ImGui::GetIO().Framerate);

	ImGui::End();
} // guiLogic


//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void Sailing::draw() {
    renderer.updateEnv();
    renderer.renderScene();
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void Sailing::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool Sailing::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool Sailing::mouseMoveEvent( double xPos, double yPos ) {
    bool eventHandled( false );
    // Fill in with event handling code...
    if (!ImGui::IsMouseHoveringAnyWindow()) {
        if ( mouse_right_press ) {
            camera.rotateLeft( ( xPos - mouse_x ) * 0.005 );
            camera.rotateUp( ( yPos - mouse_y ) * 0.002 );
            eventHandled = true;
        } // if
        
        mouse_x = xPos;
	    mouse_y = yPos;
    } // if

	return eventHandled;
} // mouseMoveEvent

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool Sailing::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

    if (!ImGui::IsMouseHoveringAnyWindow()) {
        // The user clicked in the window. 
        if (actions == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_RIGHT) {
            mouse_right_press = true;
            eventHandled = true;
        } // if
    }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool Sailing::mouseScrollEvent( double xOffSet, double yOffSet ) {
	bool eventHandled(false);
	// Fill in with event handling code...
	if ( 0.0 > yOffSet ) camera.zoom( 1.1 );
	else camera.zoom( 0.9 );
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool Sailing::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	//renderer.initPerspectiveMatrix();
	
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool Sailing::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
	    
		if ( key == GLFW_KEY_M ) {
		    // show/hide panel
			show_gui = !show_gui;
			eventHandled = true;
		}		
		if (GLFW_KEY_Q == key) {
		    // quit
		    glfwSetWindowShouldClose(m_window, GL_TRUE);
		    eventHandled = true;
		}
		// WSAD control the ship
		if ( GLFW_KEY_W == key ) {
		    ship.pushAcc();
		    eventHandled = true;
		}
		if ( GLFW_KEY_S == key ) {
		    ship.reverseAcc();
		    eventHandled = true;
		}
		if ( GLFW_KEY_A == key ) {
		    ship.startTurnLeft();
		    eventHandled = true;
		}
		if ( GLFW_KEY_D == key ) {
            ship.startTurnRight();
            eventHandled = true;
        }
        
	} 
	
	if ( action == GLFW_RELEASE ) {
	    if ( GLFW_KEY_W == key ) {
		    ship.releaseAcc();
		    eventHandled = true;
		}
		if ( GLFW_KEY_S == key ) {
		    ship.releaseAcc();
		    eventHandled = true;
		}
		if ( GLFW_KEY_A == key ) {
		    ship.cancelTurn();
		    eventHandled = true;
		}
		if ( GLFW_KEY_D == key ) {
            ship.cancelTurn();
            eventHandled = true;
        }
	}
	// Fill in with event handling code...

	return eventHandled;
}
