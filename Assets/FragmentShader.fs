#version 330

// render mode for this pass
#define RENDER_SCENE 1
#define RENDER_REFLECTION 2
#define RENDER_REFRACTION 3
#define RENDER_DEPTH_MAP 4


in vec2 UV; // texture coordinate

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};

in VsOutFsIn {
	smooth vec3 position_ES; // Eye-space position
	smooth vec3 normal_ES;   // Eye-space normal
	LightSource light;
} fs_in;


layout(location = 0) out vec4 fragColour;

struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
};

// use uniforms to control the state

uniform int render_mode;
uniform bool render_water; // is water being rendered now?

uniform vec2 screenSize;

uniform sampler2D mTexture; // unit 0
uniform sampler2D background; // unit 1
// doesn't work
//uniform sampler2D depthMap; // unit 2

uniform float reflection;
uniform float transparency;
uniform float wave;


uniform Material material;

// Ambient light intensity for each RGB component.
uniform vec3 ambientIntensity;
uniform int counter;

// modified from A3 household code
vec3 phongModel(vec3 fragPosition, vec3 fragNormal) {
	LightSource light = fs_in.light;

    // Direction from fragment to light source.
    vec3 l = normalize(light.position - fragPosition);

    // Direction from fragment to viewer (origin - fragPosition).
    vec3 v = normalize(-fragPosition.xyz);

    float n_dot_l = max(dot(fragNormal, l), 0.0);
    
    vec3 texture_kd = texture( mTexture, UV ).rgb;

	vec3 diffuse;
	if ( ! render_water ) {
	    diffuse = texture_kd * n_dot_l;
	} else {
	    diffuse = material.kd * n_dot_l;
	}

    vec3 specular = vec3(0.0);

    if (n_dot_l > 0.0) {
		// Halfway vector.
		vec3 h = normalize(v + l);
        float n_dot_h = max(dot(fragNormal, h), 0.0);
        if ( ! render_water ) {
            specular = texture( mTexture, UV ).rgb * pow(n_dot_h, material.shininess);
        } else {
            specular = material.ks * pow(n_dot_h, material.shininess);
        }
    }
    
    // ambientIntensity is used as factor; otherwise things are too dark
    vec3 ambient = vec3( ambientIntensity.x * texture_kd.x,
                         ambientIntensity.y * texture_kd.y,
                         ambientIntensity.z * texture_kd.z );

    return ambient + light.rgbIntensity * (diffuse + specular);
}


void main() {
    
    vec4 sceneColour = vec4(phongModel(fs_in.position_ES, fs_in.normal_ES), 1.0);
    
    vec2 texCoord = gl_FragCoord.xy / screenSize;
    
    // framebufferobjects have fixed size
    if ( render_mode != RENDER_SCENE ) {
        // fixed a bug when resizing the window
        texCoord = gl_FragCoord.xy / vec2( 1024.0, 768.0 );
    }

    if ( render_mode == RENDER_REFLECTION ) {
        fragColour = sceneColour;
    } 
    
    else if ( render_mode == RENDER_REFRACTION ) {
        texCoord.x = screenSize.x - texCoord.x;
        
        vec4 backgroundColour = vec4( texture( background, texCoord ).rgb, 1.0 );
        
        fragColour = mix( backgroundColour, sceneColour, 0.5 + (transparency - reflection)  / 2 );
    } 
    
    else if ( render_mode == RENDER_DEPTH_MAP ) {
        // doesn't work
    }
    
    else if ( render_mode == RENDER_SCENE ) {       
        if ( render_water ) {
            // distortion
            texCoord.x += 0.01 * wave * cos( 200.0 * (texCoord.y + 0.0005 * float(counter) ) );
        
            vec4 backgroundColour = vec4( texture( background, texCoord ).rgb, 1.0 );
            fragColour = mix( backgroundColour, sceneColour,  0.7 - transparency * 0.7 ); // alpha
        } else {
            fragColour = sceneColour;
        }   
    }
}










