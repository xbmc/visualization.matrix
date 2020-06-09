#define RNDSEED1 170.12
#define RNDSEED2 7572.1

#ifdef lowpower
float h11(float p)
{
    return fract(fract(p * .1031) * (p + 33.33));
}

vec3 bw2col(float bw, float d)
{
    float peakcolor = .6-d;
    float basecolor = .8-d;
    return (basecolor*cColor+peakcolor)*bw;
}
#else
float h11(float p)
{
    return fract(20.12345+sin(p*RNDSEED1)*RNDSEED2);
}

vec3 bw2col(float bw, float d)
{
    float peakcolor = smoothstep(.35,.0,d)*bw;
    float basecolor = smoothstep(.85,.0,d)*bw;
    return basecolor*cColor+peakcolor;
}
#endif

void main(void)
{
    //general stuff
    vec2 uv = (gl_FragCoord.xy-0.5*iResolution.xy)/iResolution.y;
    
    //rain
    vec2 gv = floor(uv*cColumns);
    float rnd = h11(gv.x) + 0.1;
    float bw = .65 - fract((gv.y*.0024)+iTime*rnd);
    
    //FFT
    float fft = texture(iChannel0, vec2((1.-abs(uv.x))*.7,0.0)).x;
    fft -= abs(uv.x)*.25;

    bw *= 1. + fft*0.4;
    bw += bw*clamp((pow(fft*1.3,2.)-12.),.0,.6);
    bw += bw*clamp((pow(fft*1.0,3.)-23.),.0,.7);
    bw = min(bw,1.99);
    
    //waveform
    float wave = texture(iChannel0,vec2(uv.x*.15+.5,0.75)).x*.5 + uv.y;
    bw -= abs(smoothstep(.225,.275,wave) -.5);

    //pseudo pixels (dots)
    float d = length(fract(uv*cColumns)-.5);

    vec3 col = bw2col(bw,d);
    
    //col *= INTENSITY;
    
    FragColor = vec4(col,1.0);
}