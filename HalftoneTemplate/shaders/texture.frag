// colour fragment shader
// Andy Baker
// Digital Whorehouse Productions 2008

	uniform vec3	StripeColour;
	uniform vec3	BackColour;
	
	uniform float	Width = 5.0; // width of stripes
	uniform float	Fuzz = 0.1;  // Smooth transition
	uniform float	Scale = 10.0; // stripe scale
	
	
	varying vec3	DiffuseColor;
	varying vec3	SpecularColor;
	varying float	TexCoord;
	

void main()
{
	vec3 red = 	vec3(1.0, 0.0, 0.0);
	
	StripeColour = vec3(1.0, 1.0, 1.0); // stripe colour to white
	BackColour = vec3(1.0, 0.0, 0.0); // back colour to red
	
	float scaledT	= fract(TexCoord * Scale); // positions the stripe pattern
	
	float frac1		= clamp(scaledT / Fuzz, 0.0, 1.0); // determines whether the, posiiton is in or out of the transition zone
	float frac2		= clamp((scaledT - Width) / Fuzz, 0.0, 1.0); // the result is stored in frac2
	
	frac1 = frac1 * (1.0 - frac2); // linear blend between backColour and StripeColour
	frac1 = frac1 * frac1 * (3.0 - (2.0 * frac1)); // Hermite interpolation between BackColour and StripeColour
	
	vec3 finalColor			= mix(BackColour, StripeColour, frac1); // Mix both colours together
	
	gl_FragColor = vec4(vec3(finalColor), 1.0);

}




