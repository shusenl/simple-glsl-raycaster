#version 120
//#extension GL_EXT_gpu_shader4 : enable //access to integer and bit operation
uniform sampler3D volume;
uniform sampler1D tranf;
/*uniform*/ vec3 lightPos;
float dim=256;

struct range
{
	float tNear;
	float tFar;
};
range intersectBox(vec3 entryPoint , vec3 rayDir, vec3 boxmin, vec3 boxmax)//, float *tnear, float *tfar)
{
    // compute intersection of ray with all six bbox planes
    vec3 invR = vec3(1.0) / rayDir;
    vec3 tbot = invR * (boxmin - entryPoint);
    vec3 ttop = invR * (boxmax - entryPoint);

    // re-order intersections to find smallest and largest on each axis
    vec3 tmin = min(ttop, tbot);
    vec3 tmax = max(ttop, tbot);

    // find the largest tmin and the smallest tmax
    float largest_tmin = max(max(tmin.x, tmin.y), tmin.z);
    float smallest_tmax = min(min(tmax.x, tmax.y), tmax.z);

	range tMinMax;
	tMinMax.tNear = largest_tmin;
	tMinMax.tFar = smallest_tmax;
	return tMinMax;
}

void main(void)
{
  lightPos = vec3(-3,-3,-2);
  //current voxel location
  vec3 pos = vec3(gl_TexCoord[0].x, gl_TexCoord[0].y, gl_TexCoord[0].z);
  
  vec3 dir = normalize(pos - lightPos);
  float len = length(pos - lightPos);
  float stepsize = 0.002;
  vec3 O = lightPos;

  range r = intersectBox(O , dir, vec3(0,0,0), vec3(1,1,1) );

  float t = r.tNear;
  r.tFar = len;
  float A = 0.0;

  if(r.tNear>r.tFar)
    discard; 

  while( t<r.tFar)
  {
    float s = texture3D(volume, O+t*dir).r;
    float a = texture1D(tranf, s).a;

    //if(a>A)
    //  A = a;
    A += a*(1-A)*stepsize*dim*0.5;
    if(A>0.99)
      break;
    t += stepsize;
  }

  gl_FragColor = vec4(A,0,0,1);
 //gl_FragColor = vec4(r.tFar - r.tNear,0,0,1);
  //gl_FragColor = vec4(texture1D(tranf,texture3D(volume,pos).a));
   //gl_FragColor = 0.5*texture3D(volume,pos);

}