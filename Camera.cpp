#include "Camera.hpp"
#include "cs488-framework/MathUtils.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <iostream>

using namespace glm;
using namespace std;

    
void Camera::linkShip( Ship &ship ) {
    m_ship = &ship;
    alpha = 0.0;
    beta = PI / 4;
    dist = 2.0; 
} // linkShip

mat4 Camera::getView() {
    float d = dist * cos( beta ); // projectin on xz plane
    
    vec3 center = m_ship->getPos();
    
    vec3 ori( d * cos( alpha ), sin( beta ) * (float) dist, d * sin( alpha ) );
    
    eye = center + normalize( ori ) * (float) dist;
    
    vec3 left = cross( ori, vec3( 0.0, 1.0, 0.0 ) );
    
    vec3 up = cross( left, ori );
    
    mat4 view = lookAt( eye, center, up );
    
    return view;
} // getView

void Camera::rotateUp( double angle ) {
    beta = clamp( beta + angle, - PI / 2, PI / 2 );
} // rotateUp

void Camera::rotateLeft( double angle ) {
    alpha += angle;
    
    if ( alpha > PI ) alpha - 2 * PI;
    else if ( alpha < -PI ) alpha + 2 * PI;
} // rotateUp

void Camera::zoom( double times ) {
    dist = clamp( dist * times, 1.0, 4.0 );
} // zoom

vec3 Camera::getPos() {
    return eye;
}

mat4 Camera::getReflectionView() {
    float d = dist * cos( -beta ); // projectin on xz plane
    
    vec3 center = m_ship->getPos();
    
    // change1:  reverse beta: put camera under the water
    vec3 ori( d * cos( alpha ), sin( -beta ) * (float) dist, d * sin( alpha ) );
    
    eye = center + normalize( ori ) * (float) dist;
    
    vec3 left = cross( ori, vec3( 0.0, -1.0, 0.0 ) );
    
    // change 2: rotate upside-down
    vec3 up = cross( left, ori );
    
    mat4 view = lookAt( eye, center, up );
    
    return view;
    
} // getReflectionView


mat4 Camera::getRefractionView() {
    double temp = beta;
    beta = clamp( beta + PI / 10, 0.0, PI / 2 );
    
    mat4 view = getView();
    beta = temp;
    
    return view;
    
} // getReflectionView

mat4 Camera::getLightView( const vec3& position ) {
    vec3 left = cross( position, vec3( 0.0, 1.0, 0.0 ) );
    
    vec3 up = cross( left, position );
    
    mat4 view = lookAt( position, vec3(0.0), up );
    
    return view;
    
} // getReflectionView







