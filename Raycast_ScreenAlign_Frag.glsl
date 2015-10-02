#version 120
#extension GL_EXT_gpu_shader4 : enable //access to integer and bit operation
uniform mat4 rotMat4;
uniform mat4 light_rotate;
uniform vec3 translateVec3;

uniform sampler3D blockPool;
uniform sampler3D shadowVolume;
uniform sampler1D tranf;
uniform sampler1D CDFtranf;
uniform sampler2D preInt2D;

uniform float renderMode;
uniform float showVol;
uniform float sizeX;
uniform float sizeY;
uniform float sizeZ;

uniform float isPreIntegration;

uniform float aspectRatio;

uniform float stepsize;
uniform float isoValue;
uniform float var;
uniform float integralMethod;

uniform vec3 cameraO;
uniform vec3 cameraDir;
uniform vec3 cameraUp;

vec3 eyeO;
//==view
uniform float insideView;
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


//////////Phong
vec3 PhongLighting(vec3 lightDir, vec3 viewDir, vec3 normal,
							vec3 diffuseColor, vec3 ambientColor, vec3 specularColor, float shininess)
{
	vec3 finalColor = ambientColor;

	vec3 N = normalize( normal );		
	vec3 L =  normalize( lightDir ); 

	float lambertTerm = dot(N, L); //position or zero
	if(lambertTerm > 0.0)
	{
		finalColor += diffuseColor*lambertTerm;

		vec3 E = normalize(-viewDir);
		vec3 R = reflect(-L,N);
		//vec3 R = 2 * dot( N,L) * N - L;

		float specularTerm = pow( max(dot(R,E),0.0), shininess);
		finalColor += specularTerm*specularColor;
	}

	return finalColor;
}

////////////////////////

vec4 DirectVolumeRendering(vec3 front, vec3 back, float upperThreshold, float threshold, float offset)
{
	vec3 dir;
	dir = back - front;
	float len = length(dir);
	vec3 norm_dir = normalize(dir);
	//float iso_value = 0.1;

	vec3 rayIncrement = stepsize*norm_dir;

	vec3 pos = front;
	vec4 value=vec4(0,0,0,0);
	float valuef = 0;
	// accumlate value
	vec4 result = vec4(0,0,0,0);//RGBA
	float raylength = 0;

  float scale = 1.0/(upperThreshold - threshold);
  float res = 0;
  // float count = 0;
  vec4 lastCDF = vec4(0);
  vec4 currentCDF = vec4(0);	
  float lastValuef = 0;
  
  while(result.a < 1.0 && raylength < len )
	{
		//count++;
		float factor = 0.5;

		vec3 pos = front+raylength*norm_dir;
		vec4 texValue = vec4(0);
		float dx, dy, dz;
		//float ratio = 100.0f;
    float shadowDensity = 1.0;


    if(renderMode > 0.5)
    {
      shadowDensity =(1.0 - texture3D(shadowVolume, pos).a)+0.15;
      valuef = texture3D(blockPool, pos).a;
    }
    else
    {
      valuef = texture3D(blockPool, pos).a;
    }
    
    //regular transfer-function
    texValue = texture1D(tranf, valuef);

    if(isPreIntegration == 1.0)
    {
      //1D pre-integration
      currentCDF = texture1D(CDFtranf, valuef);
      float dv = abs(lastValuef - valuef) * 255.0;
      float da = abs(lastCDF.a - currentCDF.a);
      if (dv == 0.0 || da == 0.0)
        texValue = texture1D(tranf, valuef);
      else
      {
        texValue.a = da/dv;
        texValue.rgb = abs(lastCDF.rgb - currentCDF.rgb)/da;
      }
    } 
    //pre-integration
    else if(isPreIntegration == 2.0)
    {
      texValue = texture2D(preInt2D, vec2(valuef,lastValuef));
    }
    
    //this is needed for both pre-integration
    lastCDF = currentCDF;
    lastValuef = valuef;

    texValue.a = texValue.a *stepsize *sizeX;

	float ratio = 2.0;
  /*
  dx = texture3D(blockPool, pos + vec3(1.0/sizeX*ratio,0,0)).a - texture3D(blockPool, pos - vec3(1.0/sizeX*ratio,0,0) ).a;
	dy = texture3D(blockPool, pos + vec3(0,1.0/sizeY*ratio,0)).a - texture3D(blockPool, pos - vec3(0,1.0/sizeY*ratio,0) ).a;
	dz = texture3D(blockPool, pos + vec3(0,0,1.0/sizeZ*ratio)).a - texture3D(blockPool, pos - vec3(0,0,1.0/sizeZ*ratio) ).a;
	//phong shading
	vec3 normal = vec3(dx, dy, dz);
	vec3 lightDir = vec3(0,0,-3);
	//return vec4(normalize(normal),1);
	texValue.rgb = PhongLighting( norm_dir, -norm_dir, normal, texValue.rgb,	vec3(0.2), vec3(1.0),5);
	*/	
    //stepsize invarient color
//    result.rgb += (1.0-result.a)*texValue.a*texValue.rgb*shadowDensity;
    result.rgb += (1.0-result.a)*texValue.a*texValue.rgb;
    result.a += (1.0-result.a)*texValue.a;
    raylength +=  stepsize;
    }
    return result;

}
void main()
{
	float maxSize =	max(sizeZ,max(sizeX, sizeY));
	float nsizeX = sizeX/maxSize;
	float nsizeY = sizeY/maxSize;
	float nsizeZ = sizeZ/maxSize;
	vec3 boxMin = vec3(-nsizeX, -nsizeY, -nsizeZ);
  vec3 boxMax = vec3(nsizeX, nsizeY, nsizeZ);

	//4=>3
	mat3 rotateMat = mat3(vec3(rotMat4[0]),vec3(rotMat4[1]),vec3(rotMat4[2]));
	vec4 O;
	vec3 eyeD;
	vec3 du, dv, imagePlane;

	vec2 uv = (gl_TexCoord[0].xy/gl_TexCoord[0].w);
	uv[0] *= aspectRatio;
	
	if(insideView<0.5)
	{
	//path view
		vec3 rotTranslate = rotateMat*translateVec3;
		O = (vec4(-rotTranslate[0],-rotTranslate[1],4-rotTranslate[2],1));
		//eyeO = O.xyz/O.w;
		//eyeD = rotateMat*normalize(vec3(uv,-2.0));
		eyeO = cameraO;//cameraO;
		du = cross(cameraDir, cameraUp);
		du = normalize(du);
		dv = cross(cameraDir, du);
		dv = normalize(dv);
		//imagePlane = eyeO
		
		eyeD = normalize((uv.x*du + uv.y*dv) + 2.0*cameraDir);

	}
	else
	{
	//normal view
		O = rotMat4*(vec4(-translateVec3[0],-translateVec3[1],4-translateVec3[2],1));
		eyeO = O.xyz/O.w;
		eyeD = rotateMat*normalize(vec3(uv,-2.0));

	}

	
	range t = intersectBox(eyeO, eyeD, boxMin, boxMax);

	vec3 start = vec3(0);
	vec3 end = vec3(0);
	
	if(t.tNear<0)
		t.tNear = 0;
	
	if(t.tNear>t.tFar)
		gl_FragColor = vec4(0,0,0,1);
	else
	{
		//gl_FragColor = vec4(pos*0.5+0.5,t.tNear);
		vec3 scale = vec3(1.0/(nsizeX*2.0),1.0/(nsizeY*2.0),1.0/(nsizeZ*2.0));
		vec3 offset = vec3(nsizeX, nsizeY, nsizeZ);
		
		start = ((eyeO + eyeD*t.tNear)+offset)*scale;
		end = ((eyeO + eyeD*t.tFar)+offset)*scale;

		gl_FragColor = DirectVolumeRendering(start, end, 0, 0, 0);
		
	}
	//gl_FragColor = vec4(eyeO,1);
}
