#define DOTSIZE 4.
const float cFallspeed = .25;


#define INTENSITY 1.0
#define MININTENSITY 0.075

#define DISTORTTHRESHOLD 0.4
#define DISTORTFACTORX 0.6
#define DISTORTFACTORY 0.4

#define VIGNETTEINTENSITY 0.05

//blueish
#define RED .2
#define GREEN .8
#define BLUE 1.

/*
//greenish
#define RED .1
#define GREEN .99
#define BLUE .40
*/

//TODO: remove:
#define time iTime


//precision highp float;
precision mediump float;

float h11(float p)
{
    return fract(pow(fract(p * .1031) * (p + 33.33),2.));
}

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    //general stuff
    vec2 uv = (fragCoord-0.5*iResolution.xy)/iResolution.y;
    //shifts the whole coord system to avoid burn-in
    vec2 shift = floor(abs(mod((vec2(iTime*.01,iTime*.003)), DOTSIZE*2.)-DOTSIZE));
    uv += shift/iResolution.y;
    mediump float columns = floor(iResolution.y/DOTSIZE);
    
    
    //rain
    mediump vec2 gv = floor(uv*columns);
    mediump float rnd = h11(gv.x) + 0.1;
    mediump float bw = 1. -(fract((gv.y*.0024)+(time+20.)*cFallspeed*rnd));
    
    bw *= .025;
    
    
    //FFT stuff (visualization)
    float fft = texture( iChannel0, vec2((1.-abs(uv.x))*.7,0.0) ).x;
    fft *= (3.2 -abs(0.-uv.x*1.3))*0.75;
    fft *= 1.8;
    
    bw=bw+bw*fft*0.4;
    bw += bw*clamp((pow(fft*1.3,2.)-12.),.0,.6);
    bw += bw*clamp((pow(fft*1.0,3.)-23.),.0,.7);
    bw = min(bw,1.99);
    


    //pseudo pixels (dots)
    //float d = length(fract(uv*columns)-.5);
    float d = length(fract(uv*columns));
    float peakcolor = smoothstep(.35,.00,d)*bw;
    float basecolor = smoothstep(.85,.00,d)*bw;

    vec3 col = vec3(peakcolor+basecolor*RED,peakcolor+basecolor*GREEN,peakcolor+basecolor*BLUE);
    col *= INTENSITY *8.;
    
    fragColor = vec4(col,1.0);
}