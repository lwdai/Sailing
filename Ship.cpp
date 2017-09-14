#include "Ship.hpp"
#include "SceneNode.hpp"
#include "JointNode.hpp"
#include "Loader.hpp"
#include "cassert"
#include <iostream>
using namespace std;
using namespace glm;

bool Ship::fetchJoint( SceneNode &node ) {
    if ( node.m_nodeType == NodeType::JointNode ) {       
        for ( SceneNode *child : node.children ) {
            if ( child->m_name == "ship" ) {
                m_joint = static_cast<JointNode *>( &node );
                return true;
            } // if
        } // for
    } // if
   
    for ( SceneNode *child: node.children ) {
        if ( fetchJoint( *child ) ) return true;
    } // for
    
    return false;       
} // fetchJoint

void Ship::init( Loader &loader ) {
    speed = 0.0;
    acc = 0.0;
    turnDeg = 0.0;
   
    bool jointFetched = fetchJoint( *loader.getRootNode() );
    assert( jointFetched );    
    
} // init

void Ship::move() {
    if ( 0.0 < speed ) m_joint->rotate_y( turnDeg * speed * speed / 0.0001 );
    else  m_joint->rotate_y( - turnDeg * speed * speed / 0.0001 );
   
    // current orientation
    double angle = ( m_joint->m_joint_y.init / 180.0 )* PI;
    
    m_joint->translate( vec3( speed * sin( angle ),
                              0.0,
                              speed * cos( angle ) ) );
                              
    if ( 0.0 == acc ) {
        double delta = 0.0 < speed ? -0.00002 : 0.00002;
        speed += delta;
    } else {
        speed += acc;
    }                            
    
    speed = clamp( speed, -0.01, 0.01 );
} // move

void Ship::startTurnLeft() {
    turnDeg = 0.4;    
} // startTurnLeft

void Ship::startTurnRight() {
    turnDeg = -0.4;
} // startTurnRight

void Ship::cancelTurn() {
    turnDeg = 0.0;
} // cancelTurn
    
void Ship::pushAcc() {
    acc = 0.00005;
} // pushAcc

void Ship::reverseAcc() {
    acc = -0.00005;
} // reverseAcc
    
void Ship::releaseAcc() {
    acc = 0.0;
} // releaseAcc

vec3 Ship::getPos() {
    // z should be 0
    vec3 pos( m_joint->trans[3][0],
              m_joint->trans[3][1],
              m_joint->trans[3][2] );
              
    return pos;
} // getPos










