#version 330

// render mode for this pass
#define RENDER_SCENE 1
#define RENDER_REFLECTION 2
#define RENDER_REFRACTION 3
#define RENDER_DEPTH_MAP 4


// Model-Space coordinates
in vec3 position;
in vec3 normal;
in vec2 textureCoords;  // pass directly to UV
out vec2 UV;

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};

uniform LightSource light;
uniform mat4 ModelView;
uniform mat4 Perspective;

uniform bool render_water; // is water being rendered now?

uniform int render_mode;

// Remember, this is transpose(inverse(ModelView)).  Normals should be
// transformed using this matrix instead of the ModelView matrix.
uniform mat3 NormalMatrix;

out VsOutFsIn {
	smooth vec3 position_ES; // Eye-space position
	smooth vec3 normal_ES;   // Eye-space normal
	LightSource light;
} vs_out;


void main() {
	vec4 pos4 = vec4(position, 1.0);

	//-- Convert position and normal to Eye-Space:
	
	if ( render_mode == RENDER_REFRACTION && position.y < 0.0 ) {
	    // refraction doesn't work, but still rendered and clipped...	 
	} 
	
	vs_out.position_ES = (ModelView * pos4).xyz;
	vs_out.normal_ES = normalize(NormalMatrix * normal);
	
	vs_out.light = light;
	
	UV = textureCoords;

	gl_Position = Perspective * ModelView * vec4(position, 1.0);
	
	// water surface is y = 0; and no transformation can cross the surface...
	if ( render_mode == RENDER_REFLECTION || render_mode == RENDER_SCENE ) {
	    if ( ! render_water ) gl_ClipDistance[0] = position.y;
	} else if ( render_mode == RENDER_REFRACTION ) {
	    gl_ClipDistance[0] = - position.y;
	}
}





