#define DOTSIZE 4.
#define RNDSEED1 170.12
#define RNDSEED2 7572.1
#define FALLSPEED 4. //depends on height - I think
#define PIXELSHIFT iTime * .0001

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

void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
	float zoom = abs(sin(iTime*.25));
	zoom = 1.;
    //general stuff
    vec2 uv = (fragCoord-0.5*iResolution.xy)/iResolution.y*zoom;
    //shifts the whole coord system to avoid burn-in
    vec2 shift = floor(abs(mod((vec2(iTime*.01,iTime*.003)), DOTSIZE*2.)-DOTSIZE));
    uv += shift/iResolution.y;
    highp float columns = floor(iResolution.y/DOTSIZE);
    
    
    //rain
    highp vec2 gv = floor(uv*columns);
    //highp float rnd = fract(20.12345+sin(gv.x*RNDSEED1)*RNDSEED2)+distort*.14;
    highp float rnd = fract(20.12345+sin(gv.x*RNDSEED1)*RNDSEED2);
    highp float bw = 1.-(fract((gv.y*.01)/FALLSPEED+(time+20.)*0.25*rnd)*1.);
    
    bw *= .1;
    
    
    //FFT stuff (visualization)
    float fft = texture( iChannel0, vec2((1.-abs(uv.x))*.7,0.0) ).x;
    fft *= (3.2 -abs(0.-uv.x*1.3))*0.75;
    fft *= 1.8;
    
    bw=bw+bw*fft*0.4;
    bw += bw*clamp((pow(fft*1.3,2.)-12.),.0,.6);
    bw += bw*clamp((pow(fft*1.0,3.)-23.),.0,.7);
    bw = min(bw,1.99);
    
    
    //noise texture
    //bw *= texture(iChannel2, vec2((fragCoord)/(256.*DOTSIZE))+PIXELSHIFT+shift*1.).x;
    //bw *= texture(iChannel2, vec2(((fragCoord+shift))/(256.*DOTSIZE)-vec2(distort,0.)*.1)+PIXELSHIFT).x;
	bw *= texture(iChannel2, vec2(((fragCoord+shift))/(256.*DOTSIZE))+PIXELSHIFT).x;

    //bw *= .5;


	//vignette effect
	float vignette = length(uv)*VIGNETTEINTENSITY;
	//bw -= vignette;
	

    //pseudo pixels (dots)
    float d = length(fract(uv*columns)-.5);
    float peakcolor = smoothstep(.35,.00,d)*bw;
    float basecolor = smoothstep(.85,.00,d)*bw;

    vec3 col = vec3(peakcolor+basecolor*RED,peakcolor+basecolor*GREEN,peakcolor+basecolor*BLUE);
    col *= INTENSITY;
    vec3 tmp4 = col;
    
    
    fragColor = vec4(col,1.0);
}
