#version 130

precision mediump float;

// Uniforms
uniform vec2 uResolution;
uniform float uTimeParameter;

// Varying
varying vec2 vScenePosition;
varying vec2 vTexCoord;

// Standard 2x2 hash algorithm.
vec2 hash22(vec2 p) { 
    float n = sin(dot(p,vec2(41, 289))); 
    p = fract(vec2(8.0*n, n)*262144.);
    return sin(p*6.2831853 + uTimeParameter*2.);
}

float Voronoi3Tap(vec2 p){
    
    vec2 s = floor(p + (p.x+p.y)*0.3660254); // Skew the current point.
    p -= s - (s.x+s.y)*0.2113249; // Use it to attain the vector to the base vertice (from p).

    // Determine which triangle we're in. Much easier to visualize than the 3D version.
    float i = step(0.0, p.x-p.y); 
    
    // Vectors to the other two triangle vertices.
    vec2 p1 = p - vec2(i, 1.0-i) + 0.2113249, p2 = p - 0.5773502; 

    // Add some random gradient offsets to the three vectors above.
    p += hash22(s)*0.125;
    p1 += hash22(s +  vec2(i, 1.0-i))*0.125;
    p2 += hash22(s + 1.0)*0.125;
    
    // Determine the minimum Euclidean distance. You could try other distance metrics, 
    // if you wanted.
    float d = min(min(dot(p, p), dot(p1, p1)), dot(p2, p2))/0.425;
   
    // That's all there is to it.
    return sqrt(d); // Take the square root, if you want, but it's not mandatory.

}

void main(void) {

	//vec2 uv = (fragCoord.xy - iResolution.xy*0.5)/ iResolution.y;

    vec2 uv = .5 + (vTexCoord - .5) * (uResolution / uResolution.yy);//vTexCoord.xy;
    
    // Take two 3-tap Voronoi samples near one another.
    float c = Voronoi3Tap(uv*5.);
    float c2 = Voronoi3Tap(uv*5. - 10./uResolution.y);
    
    // Coloring the cell.
    //
    // Use the Voronoi value, "c," above to produce a couple of different colors.
    // Mix those colors according to some kind of moving geometric patten.
    // Setting "pattern" to zero or one displays just one of the colors.
    float pattern = cos(uv.x*0.75*3.14159-0.9)*cos(uv.y*1.5*3.14159-0.75)*0.5 + 0.5;
    
    // Just to confuse things a little more, two different color schemes are faded in out.
    //
    // Color scheme one - Mixing a firey red with some bio green in a sinusoidal kind of pattern.
    vec3 col = mix(vec3(c*1.3, pow(c, 2.), pow(c, 10.)), vec3(c*c*0.8, c, c*c*0.35), pattern );
    // Color scheme two - Farbrausch fr-025 neon, for that disco feel. :)
    vec3 col2 = mix(vec3(c*1.2, pow(c, 8.), pow(c, 2.)), vec3(c*1.3, pow(c, 2.), pow(c, 10.)), pattern );
    // Alternating between the two color schemes.
    col = mix(col, col2, smoothstep(.4, .6, sin(uTimeParameter*.25)*.5 + .5)); // 

    //col = mix(col.zxy, col, cos(uv.x*2.*3.14159)*cos(uv.y*5.*3.141595)*.25 + .75 );
    
    // Hilighting.
    //
    // Use a combination of the sample difference "c2-c" to add some really cheap, blueish highlighting.
    // It's a directional-derviative based lighting trick. Interesting, but no substitute for point-lit
    // bump mapping. Comment the following line out to see the regular, flat pattern.
    col += vec3(0.5, 0.8, 1.)*(c2*c2*c2-c*c*c)*5.;
       
    // Speckles.
    //
    // Adding subtle speckling to break things up and give it a less plastic feel.
    col += (length(hash22(uv + uTimeParameter))*0.06 - 0.03)*vec3(1., 0.5, 0.);
    

    // Vignette.
    //
    //col *= (1.15 - dot(uv, uv)*0.5);//*vec3(1., 0.97, 0.92); // Roundish.
    vec2 p = uv*vec2(uResolution.y/uResolution.x, 1.)+0.5; // Rectangular.
    col *= smoothstep(0., 0.5, pow( 16.*p.x*p.y*(1.0-p.x)*(1.0-p.y), 0.25))*vec3(1.1, 1.07, 1.01);
    
    
    // Even more color schemes.
    //col = col.xzy; // col.yzx, col.zyx, etc.
    
    // Approximate gamma correction.
	gl_FragColor = vec4(sqrt(clamp(col, 0., 1.)), 1.);


    /*
    bool checkX = fract(uv.x * 10.0) < .5;
    bool checkY = fract(uv.y * 10.0) < .5;
    if(checkX ? checkY : !checkY)
        gl_FragColor = vec4(0,0,0,1);
        */
	
}