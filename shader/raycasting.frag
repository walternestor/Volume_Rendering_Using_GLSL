#version 330
// for raycasting

in vec3 EntryPoint;
in vec4 ExitPointCoord;

uniform sampler2D ExitPoints;
uniform sampler3D VolumeTex;
uniform sampler1D TransferFunc;  
uniform float     StepSize;
uniform vec2      ScreenSize;
layout (location = 0) out vec4 FragColor;
//out vec4 FragColor;

const int numSamples = 1600;
uniform float azimuth, elevation, clipPlaneDepth; //clipping plane variables
uniform int clip;

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

    //    return vec3( x, z, y );
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

    //    vec3 rayStart = 0.1 * (EntryPoint + 0.5);
    //    vec3 rayStop = 0.1 * (exitPoint + 0.5);

    vec3 rayStart = EntryPoint;
    vec3 rayStop = exitPoint;
    vec3 dir = rayStop - rayStart;

    // the length from front to back is calculated and used to terminate the ray
    float len = length(dir);
    dir = normalize(dir);

    vec3 deltaDir = dir * StepSize; // -- Step
    //    vec3 step = dir * StepSize;
    float deltaDirLen = length(deltaDir);
    vec3 voxelCoord = EntryPoint;
    float travel = distance(rayStop, rayStart);
    vec3 pos = rayStart;

    vec4 colorAcum = vec4(0.0); // The dest color
    float alphaAcum = 0.0;                // The  dest alpha for blending
    float intensity;
    float lengthAcum = 0.0;
    float stepLength = 0.0;
    vec4 colorSample; // The src color
    float alphaSample; // The src alpha
    // backgroundColor
    //alpha = 1 means fully opaque while alpha = 0 means fully transparent
    vec4 bgColor = vec4(0.0, 0.0, 0.0, 0.0);

    if (clip == 1)
    {
        //render the clipped surface invisible
        FragColor = vec4(0.0, 0.0, 0.0, 0.0);
        //    FragColor.a = 1.0;
        // see if clip plane faces viewer
        bool frontface = (dot(dir , clipPlane) > 0.0);

        // Dot product between 2 normalized vectors will give you a number that ranges from -1 to 1.
        // 1 means that they are pointed the same way
        // -1 means they are pointed opposite ways.
        // 0 means they are perpendicular
        // distance from ray origin to clip plane
        float dis = dot(dir, clipPlane);

        if (dis != 0.0)
        {
            dis = (-clipPlaneDepth - dot(clipPlane, rayStart.xyz-0.5)) / dis;
        }

        if ((!frontface) && (dis < 0.0))
        {
            return;
        }

        if ((frontface) && (dis > len))
        {
            return;
        }

        if ((dis > 0.0) && (dis < len))
        {
            if (frontface)
            {
                rayStart = rayStart + dir * dis;
            }
            else
            {
                rayStop =  rayStart + dir * dis;
            }
        }

        pos = rayStart;
        deltaDir = normalize(rayStop - rayStart) * StepSize;
        travel = distance(rayStop, rayStart);
        deltaDirLen = length(deltaDir);
    } // End IF

    for (int i = 0; i < numSamples && travel > 0.0; ++i, pos += deltaDir, travel -= StepSize)
    {
        //float intensity
        // -- tf_pos
        intensity =  texture(VolumeTex, pos).x;

        // vec4 colorSample; // The src color
        // -- value
        colorSample = texture(TransferFunc, intensity);

        // modulate the value of colorSample.a
        // front-to-back integration
        if (colorSample.a > 0.0)
        {
            // accomodate for variable sampling rates (base interval defined by mod_compositing.frag)
            // StepSize = 0.001f -- 1 - (1 - colorSample.a ^0,2)
            colorSample.a = 1.0 - pow(1.0 - colorSample.a, StepSize*200.0f);
            colorAcum.rgb += (1.0 - colorAcum.a) * colorSample.rgb * colorSample.a;
            colorAcum.a += (1.0 - colorAcum.a) * colorSample.a;
        }

        //         voxelCoord += deltaDir;
        //        voxelCoord += pos;
        lengthAcum += deltaDirLen;

        // terminate if opacity > 1 or the ray is outside the volume
        if (lengthAcum >= len )
        {
            colorAcum.rgb = colorAcum.rgb*colorAcum.a + (1 - colorAcum.a)*bgColor.rgb;
            break;
        }
        if (colorAcum.a > 1.0)
        {
            colorAcum.a = 1.0;
            break;
        }
    }

    FragColor = colorAcum;
}

