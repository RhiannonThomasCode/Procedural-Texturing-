// colour vertex shader
// Andy Baker
// Digital Whorehouse Productions 2008


	uniform vec3	LightPos;  // Position of Lighting
	uniform vec3	LightColour;  // Colour of Light
	uniform vec3	EyePosition;
	uniform vec3	SpecularLight;  // Specular Light
	uniform vec3	AmbientLight; // Ambient Light
	uniform float	Kd; // Koefficient
	
	uniform mat4	MVMatrix;  // Model View Matrix
	uniform mat4	MVPMatrix;  // Model View Projection Matrix
	uniform mat3	NormalMatrix;  // Matrix Normal
	
	varying vec4	Vertex; // Vertex Matrix
	varying vec3	Normal;  // Matrix Normal
	varying vec2	TexCoord0;  
	
	varying vec3	DiffuseColor; 
	varying vec3	SpecularColor;
	varying float	TexCoord;


void main()
{	
	// Declaring Vectors
	LightPos		= vec3(1.0, 1.0, 1.0);
	LightColour		= vec3(1.0, 1.0, 1.0);
	EyePosition		= vec3(1.0, 1.0, 1.0);
	SpecularLight	= vec3(1.0, 1.0, 1.0);
	AmbientLight	= vec3(1.0, 1.0, 1.0);
	Kd				= 0.8;
	
	// Creating the Camera Position
	vec3 ecPosition		= vec3(MVMatrix * Vertex); // Camera Position
	vec3 tnorm			= normalize(NormalMatrix * Normal); // Normal position
	vec3 lightVec		= normalize(LightPos - ecPosition);  // Light Vector
	vec3 viewVec		= normalize(EyePosition - ecPosition);  // View Vector
	vec3 hvec			= normalize(viewVec + lightVec);  // Halfway Vector
	
	
	float spec		= clamp(dot(hvec, tnorm), 0.0, 1.0); // Dot Product of Halfway Vector and Normal Position
	spec			= pow(spec, 16.0); 
	
	// Creating Light
	DiffuseColor	= LightColour * vec3(Kd * dot(lightVec, tnorm)); 
	DiffuseColor	= clamp(AmbientLight + DiffuseColor, 0.0, 1.0);
	SpecularColor	= clamp((LightColour * SpecularLight * spec), 0.0, 1.0);

	
	TexCoord		= TexCoord0.t;
	gl_Position 	= ftransform();
	
}


