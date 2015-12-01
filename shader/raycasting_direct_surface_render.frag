#version 330

in vec3 EntryPoint;
in vec4 ExitPointCoord;

uniform sampler2D ExitPoints;
uniform sampler3D VolumeTex;
uniform sampler1D TransferFunc;
uniform float     StepSize;
uniform vec2      ScreenSize;
uniform float	  tmin;
uniform float     tmax;

uniform float	  visual;
uniform float     distrans;
uniform vec3      cutplane;
uniform float     cutstate;
uniform float     cutplanepos;

out vec4 FragColor;

uniform float     intensityMin, intensityMax;
uniform float     redCenter, redWidth;

vec3      lightcontrol = vec3(0.5,0.5,2.0);
float     surfacealpha = 1.0;
vec3      matcolor = vec3(1.0,0.0,0.0);

// Clip Plane
uniform float     azimuth, elevation, clipPlaneDepth;
uniform int clip;
const int numSamples = 1600;


// Convert from Polar to Cartesian Coordinates
vec3 p2cart(float azimuth, float elevation)
{
    float pi = 3.1415926;
    float x, y, z, k;
    float ele = -elevation * pi / 180.0;
    float azi = (azimuth + 90.0) * pi / 180.0;

    k = cos( ele );
    z = sin( ele );
    y = sin( azi ) * k;
    x = cos( azi ) * k;

    return vec3( x, y, z );
}

void main()
{
    vec3 clipPlane = p2cart(azimuth, elevation);

    // ExitPointCoord
    vec3 exitPoint = texture(ExitPoints, gl_FragCoord.st/ScreenSize).xyz;
    // that will actually give you clip-space coordinates rather than
    // normalised device coordinates, since you're not performing the perspective
    // division which happens during the rasterisation process (between the vertex
    // shader and fragment shader
    // vec2 exitFragCoord = (ExitPointCoord.xy / ExitPointCoord.w + 1.0)/2.0;
    // vec3 exitPoint  = texture(ExitPoints, exitFragCoord).xyz;

    if (EntryPoint == exitPoint)
        //background need no raycasting
        discard;

    vec3 rayStart = EntryPoint;
    vec3 rayStop = exitPoint;
    vec3 dir = rayStop - rayStart;

    // the length from front to back is calculated and used to terminate the ray
    float len = length(dir);
    dir = normalize(dir);

    vec3  deltaDir = dir * StepSize; // -- Step
    float deltaDirLen = length(deltaDir);
    float travel = distance(rayStop, rayStart);

    vec3 pos = rayStart;
    vec3 voxelCoord = EntryPoint;

    vec4 colorAcum = vec4(0.0); // The dest color
    float alphaAcum = 0.0;      // The  dest alpha for blending
    float intensity;
    float lengthAcum = 0.0;
    float stepLength = 0.0;
    vec4 colorSample;           // The src color
    float alphaSample;          // The src alpha

    // backgroundColor
    //alpha = 1 means fully opaque while alpha = 0 means fully transparent
    vec4 bgColor = vec4(0.0, 0.0, 0.0, 0.0);

    vec4 colorBoundary = vec4(0.0);

    float delta = 0.001;
    vec3 lightPosition = lightcontrol;
    vec3 eyePosition = lightcontrol;

    vec3 voxelBoundary;

    vec3 Ka = vec3(0.1,0.1,0.1);
    vec3 Kd = vec3(0.6,0.6,0.6);
    vec3 Ks = vec3(0.7,0.7,0.7);

    float shininess = 100.0;
    vec3 lightColor = vec3(1.0,1.0,1.0);
    vec3 ambientLight = vec3(0.5,0.5,0.5);



    // ===================================================================
    // Clip Plane
    // ===================================================================
    if (clip == 1)
    {
        //render the clipped surface invisible
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);

        // see if clip plane faces viewer
        bool frontface = (dot(dir , clipPlane) > 0.0);

        // Dot product between 2 normalized vectors will give you a number that ranges from -1 to 1.
        // 1 means that they are pointed the same way
        // -1 means they are pointed opposite ways.
        // 0 means they are perpendicular
        // distance from ray origin to clip plane
        float dis = dot(dir, clipPlane);

        if (dis != 0.0) {
            dis = (-clipPlaneDepth - dot(clipPlane, rayStart.xyz-0.5)) / dis;
        }

        if ((!frontface) && (dis < 0.0)) {
            return;
        }

        if ((frontface) && (dis > len)) {
            return;
        }

        if ((dis > 0.0) && (dis < len)) {
            if (frontface) {
                rayStart = rayStart + dir * dis;
            }
            else {
                rayStop =  rayStart + dir * dis;
            }
        }

        pos = rayStart;
        deltaDir = normalize(rayStop - rayStart) * StepSize;
        travel = distance(rayStop, rayStart);
        deltaDirLen = length(deltaDir);
    } // End IF

    // ===================================================================
    // Accumulatin Loop
    // ===================================================================
    for (int i = 0; i < numSamples && travel > 0.0; ++i, pos += deltaDir, travel -= StepSize, lengthAcum += deltaDirLen)
    {
        intensity =  texture(VolumeTex, pos).x;

        float rgba = ((intensity - redCenter + (redWidth / 2)) / redWidth);
        colorSample.rgba = vec4(rgba, rgba, rgba, rgba);


        if (colorSample.a > intensityMin && colorSample.a < intensityMax)
        {
            vec3 sample1, sample2;
            sample1.x = texture(VolumeTex, pos - vec3(delta, 0.0,   0.0)).x;
            sample2.x = texture(VolumeTex, pos + vec3(delta, 0.0,   0.0)).x;
            sample1.y = texture(VolumeTex, pos - vec3(0.0,   delta, 0.0)).x;
            sample2.y = texture(VolumeTex, pos + vec3(0.0,   delta, 0.0)).x;
            sample1.z = texture(VolumeTex, pos - vec3(0.0,   0.0,   delta)).x;
            sample2.z = texture(VolumeTex, pos + vec3(0.0,   0.0,   delta)).x;

            vec3 Ntemp = (sample2-sample1);
            vec3 N = normalize(Ntemp);
            vec3 L = normalize(lightPosition-pos);
            vec3 V = normalize(eyePosition-pos);
            vec3 H = normalize(L+V);

            vec3 ambient = Ka * ambientLight;
            float diffuseLight = max(dot(L,N),0);
            vec3 diffuse = Kd * lightColor * diffuseLight;

            float specularLight = pow(max(dot(H,N),0),shininess);
            vec3 specular = Ks*lightColor*specularLight;

            if (abs(Ntemp.x) <= 0.001 && abs(Ntemp.y) <= 0.001 && abs(Ntemp.z) <= 0.001)
            {
                colorAcum.rgb = colorSample.rgb * matcolor;
            }
            else
            {
                colorAcum.rgb += ambient + diffuse * matcolor + specular;
            }
            colorAcum.a += surfacealpha+0.01;


            voxelBoundary = pos;
        } // end if (colorSample.a > intensityMin && colorSample.a < intensityMax)


//        voxelCoord += deltaDir;
//        lengthAcum += deltaDirLen;

        if (lengthAcum >= len )
        {
            vec3 sample1, sample2;
            sample1.x = texture(VolumeTex, voxelBoundary - vec3(delta,0.0,0.0)).x;
            sample2.x = texture(VolumeTex, voxelBoundary + vec3(delta,0.0,0.0)).x;
            sample1.y = texture(VolumeTex, voxelBoundary - vec3(0.0,delta,0.0)).x;
            sample2.y = texture(VolumeTex, voxelBoundary + vec3(0.0,delta,0.0)).x;
            sample1.z = texture(VolumeTex, voxelBoundary - vec3(0.0,0.0,delta)).x;
            sample2.z = texture(VolumeTex, voxelBoundary + vec3(0.0,0.0,delta)).x;

            vec3 N = normalize(sample2-sample1);
            vec3 L = normalize(lightPosition-voxelBoundary);
            vec3 V = normalize(eyePosition-voxelBoundary);

            vec3 H = normalize(L+V);
            vec3 ambient = Ka * ambientLight;
            float diffuseLight = max(dot(L,N),0);
            vec3 diffuse = Kd * lightColor * diffuseLight;

            float specularLight = pow(max(dot(H,N),0),shininess);
            vec3 specular = Ks*lightColor*specularLight;
            colorBoundary.rgb = ambient + diffuse*matcolor + specular;

            colorAcum.rgb = colorAcum.rgb + colorBoundary.rgb;

            colorAcum.rgb = colorAcum.rgb*colorAcum.a  + (1.0 - colorAcum.a)*bgColor.rgb;
            break;
        }
        else if (colorAcum.a > 1.0)
        {
            colorAcum.a = 1.0;
            break;
        }
    }

    FragColor = colorAcum;
}
