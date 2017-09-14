#pragma once
#include "JointNode.hpp"
#include "cs488-framework/MathUtils.hpp"

class SceneNode;
class Loader;

class Ship {
    JointNode *m_joint;
    double speed;
    double acc;  // acceleration distance/(frame^2)
    
    double turnDeg; // degree per frame 
    
    // return true on success withou recursion
    bool fetchJoint( SceneNode &node );
    
public:
    void init( Loader &loader );
    
    // change state/tranformation to the next frame
    void move();
    
    void startTurnLeft();
    void startTurnRight();
    void cancelTurn();
    
    void pushAcc();
    void reverseAcc();
    void releaseAcc();
    
    glm::vec3 getPos();
    
};
