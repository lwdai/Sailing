#pragma once
#include "SceneNode.hpp"
#include "JointNode.hpp"
#include "Ship.hpp"

class Camera {
    
    double alpha; // [-pi, pi]
    double beta; // [0, pi/2]
    double dist; // distance between camera and ship
    glm::vec3 eye;
  
    Ship* m_ship;

public: 
    void linkShip( Ship &ship );
    
    
    glm::mat4 getView();
    glm::mat4 getReflectionView();
    glm::mat4 getRefractionView();
    static glm::mat4 getLightView( const glm::vec3& position );
    
    
    void rotateUp( double angle );
    void rotateLeft( double angle );
    
    // times > 0 => zoom out; times < 0 => zoom in
    void zoom( double times );
    
    glm::vec3 getPos();
};
