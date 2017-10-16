#version 130

precision mediump float;

varying vec2 vPosition;
varying vec2 vTexCoord;
varying vec2 vScenePosition;

uniform vec3 uLookVector;
uniform sampler2D uVeinsTexture;


const float pi = 3.1415926535;
const vec3 lightNormal =  normalize(vec3(4,-4,-1));

void main() {
    vec2 xy = 2 * vTexCoord - 1.0;
    float lengthSqrd = dot(xy, xy);
    if(lengthSqrd < 1.0) {
        float dist = sqrt(lengthSqrd);
        float z = sqrt(1 - dist * dist);
        vec3 normal = vec3(xy, z);
        normal = normalize(normal);
        gl_FragColor = vec4(.5 + .5 * normal.xy, normal.z, 1);

        vec3 lightdir = normalize(vec3(500,-500,500) - vec3(vScenePosition, 20 * normal.z));
        float b = dot(lightdir, normal);
        b = pow(b, 2.0);
        vec3 darkColor = vec3(.15, .3, .3);
        vec3 midColor = vec3(.4, .6, .5);
        vec3 lightColor = vec3(0.4, 0.8, .65);
        if(b < .5)
            gl_FragColor.rgb = mix(darkColor, midColor, b / .5);
        else
            gl_FragColor.rgb = mix(midColor, lightColor, (b - .5) / .5);

        float baseProj = dot(uLookVector, normal);
        if(baseProj > .85) {
            vec3 u = normalize(cross(vec3(0,1,0), uLookVector));
            vec3 v = normalize(cross(uLookVector, u));
            vec2 uv = vec2(dot(normal, u), dot(normal,v)) * 3.0;

            gl_FragColor = vec4(.3 + b / 1.0, .1 + b / 2.0, b / 3.0, 1);
            float l = length(uv);
            if(l< 1.0)
                gl_FragColor.rgb = vec3(0);
            else if(l > 1.35)
                gl_FragColor.rgb *= .5;
        }
        else {
            float texY = .5 + .5 * baseProj;

            vec3 u = normalize(cross(vec3(0,1,0), uLookVector));
            vec3 v = normalize(cross(uLookVector, u));
            vec2 uv = vec2(dot(normal, u), dot(normal,v));
            float texX = fract(.25 * atan(uv.y, uv.x) / pi);
            float sample = 1 - texture2D(uVeinsTexture, vec2(texX , asin(texY) * .15)).r;

            if(baseProj > .5)
                sample *= max(0, 1 - (baseProj - .5) / .3);

            gl_FragColor.rgb = mix(gl_FragColor.rgb, .5 * darkColor, .2 * sample);

        }

        if(b > .7) {
            float highlight = pow((b - .7) / .3, 3.0);
            gl_FragColor.rgb = mix(gl_FragColor.rgb, vec3(1), .5 * highlight);
            //alpha = mix(alpha, 1.0, highlight);
        }

        if(lengthSqrd > .8)
            gl_FragColor *= .7 + .3 * (1 - (lengthSqrd - .8) / .2);

        if(dist > .95)
            gl_FragColor.rgb = darkColor;
    }
    else
        gl_FragColor = vec4(0);
}
