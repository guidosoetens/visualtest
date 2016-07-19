#version 130

precision mediump float;

uniform vec2 uResolution;
uniform float uTimeParameter;
uniform int uTriangulateOffset;

varying vec2 vPosition;
varying vec3 vNormal;
varying vec2 vFlowCoord;

const float sqrt_third = 0.57735026919;
const vec3 lightNormal = vec3(sqrt_third, -sqrt_third, sqrt_third);
const float pi = 3.1415926535;


//TRIANGLE FLOW!


const float triangleHeight = .5 * tan(pi / 3.0);

const float baseSize = sqrt(3.0);
const float halfBaseSize = .5 * baseSize;
const vec2 source = vec2(0, 1);
const vec2 sink1 = vec2(-halfBaseSize, -.5);
const vec2 sink2 = vec2(halfBaseSize, -.5);
const vec2 center = (source + sink1 + sink2) / 3.0;
const float bezierOffset = 0.5 * baseSize;
const float maxInternalOffset = length(.25 * source - .5 * center + .25 * sink1);
const float centerStretchFactor = (maxInternalOffset + bezierOffset) / bezierOffset;

const vec2 cornerTL = source + (sink1 - sink2);
const vec2 cornerTR = source + (sink2 - sink1);
const vec2 cornerB = sink1 + (sink2 - source);

float cross2(vec2 v1, vec2 v2) {
    return v1.x * v2.y - v1.y * v2.x;
}

vec2 getTriangulatedOffset() {
    vec2 xy = vFlowCoord;
 
  //gl_FragColor = vec4(xy.x + 2.5,0,0,1);
 
 
  vec2 vecSource = normalize(center - source);
  vec2 vecSink1 = normalize(sink1 - center);
  vec2 vecSink2 = normalize(sink2 - center);
 
  float traversalDistance = 2. * length(center - source);
 
 
  float projSource = dot(xy - source, vecSource);
  float projSink1 = dot(xy - sink1, vecSink1);
  float projSink2 = dot(xy - sink2, vecSink2);
 
  float orSource = cross2(xy - source, vecSource);
  float orSink1 = cross2(xy - sink1, vecSink1);
  float orSink2 = cross2(xy - sink2, vecSink2);
 
  float val1 = projSource / traversalDistance;
  float val2 = 1.0 + projSink1 / traversalDistance;
  float val3 = 1.0 + projSink2 / traversalDistance;
 
  float power = 2.0;
  float weight1 = pow(1.0 / abs(projSource), power);
  float weight2 = pow(1.0 / abs(projSink1), power);
  float weight3 = pow(1.0 / abs(projSink2), power);
  float sumWeight = weight1 + weight2 + weight3;
 
  float offsetX = (weight1 / sumWeight) * val1
                + (weight2 / sumWeight) * val2
                + (weight3 / sumWeight) * val3;
 
  //calc offset Y:
  vec2 focusPt;
  if(orSource <= 0.0 && orSink2 <= 0.0)
    focusPt = cornerTR;
  else if(orSink2 >= 0.0 && orSink1 <= 0.0)
      focusPt = cornerB;
  else
      focusPt = cornerTL;
 
 
  vec2 to = xy - focusPt;
  float toDist = length(to);
  to /= toDist;
 
  float dist = toDist - bezierOffset;
 
  float maxAng = pi / 6.;
 
  float angle = acos(dot(to, normalize(center - focusPt)));
  float maxOffset = 1.0 / cos(pi / 6.0 - angle) - bezierOffset;
 
  float circDistFrac = dist / (1. - bezierOffset);
  float projDistFrac = dist / maxOffset;
 
 
  float angleFrac = 1. - angle / maxAng;
 
 
  float offFactor = pow(projDistFrac, 2.0 + abs(angleFrac) * projDistFrac);
  float offsetY = (1. - offFactor) * circDistFrac + offFactor * projDistFrac;
  offsetY = 1. - offsetY;

  return vec2(offsetX, offsetY);
}

//END TRIANGLE FLOW

void main() {

    gl_FragColor = vec4(.5 + .5 * vNormal.xy, vNormal.z, 1);

    float r = 0.0;
    if(uTriangulateOffset == 1) {
        //gl_FragColor = vec4(clamp(getTriangulatedOffset(), 0., 1.), 0.5, 1);
        r = getTriangulatedOffset().y;

        r = vFlowCoord.x / (2.0 * sqrt(3.0)) + 1.0;
    }
    else {
        //gl_FragColor = vec4(clamp(vFlowCoord.xy, 0., 1.), 0.5, 1);
        r = vFlowCoord.x;
    }

    gl_FragColor = vec4(r, 0., 0.5, 1);
}
