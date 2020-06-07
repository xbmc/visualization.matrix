#define RNDSEED1 170.12
#define RNDSEED2 7572.1

#define INTENSITY 1.0
#define MININTENSITY 0.075

#define DISTORTTHRESHOLD 0.4
#define DISTORTFACTORX 0.6
#define DISTORTFACTORY 0.4

#define VIGNETTEINTENSITY 0.05

#ifdef lowpower
float h11(float p)
{
    return fract(fract(p * .1031) * (p + 33.33));
}
#else
float h11(float p)
{
    return fract(20.12345+sin(p*RNDSEED1)*RNDSEED2);
}
#endif

void main(void)
{
    //general stuff
    vec2 uv = (gl_FragCoord.xy-0.5*iResolution.xy)/iResolution.y;
    
    //rain
    vec2 gv = floor(uv*cColumns);
    float rnd = h11(gv.x) + 0.1;
    float bw = 1. - fract((gv.y*.0024)+iTime*rnd);
    
    //VHS-like distortions
    float wav = texture( iChannel0, vec2((uv.y +1.)*.5,1.0) ).x-.5;
	float distort = sign(wav) * max(abs(wav)-DISTORTTHRESHOLD,.0);
    
    //KODI texture
    float tex = texture(iChannel1, uv+.5-vec2(distort*DISTORTFACTORX,distort*DISTORTFACTORY)).x;
    //float tex = texture(iChannel1, uv+.5-vec2(distort,distort)).x;
    //float tex = texture(iChannel1, uv+.5).x;
    tex *= .9 - wav*.2;
    //Shadow effect around the KODI texture. Needs a prepared texture to work.
    float shadow = (wav+.5)*.25;
    tex = (max(shadow,tex)-shadow)/(1.-shadow);
        
    
    //"interlaced" logo distortion effect
    float line = mod(gv.y*sign(wav),2.);
    tex *= 1. - (line*10.*abs(distort) + 5.*abs(distort));
    
    bw = bw*max(tex,MININTENSITY);
    
    //brightens lines where distortion are occuring
	bw += min(abs(distort)*.7,.0105);
    
    //FFT stuff (visualization)
    //might need some scaling
    float fft = texture( iChannel0, vec2((1.-abs(uv.x-distort*.2))*.7,0.0) ).x;
    fft *= (3.2 -abs(0.-uv.x*1.3))*0.75;
    fft *= 1.8;
    
    bw=bw+bw*fft*0.4;
    bw += bw*clamp((pow(fft*1.3,2.)-12.),.0,.6);
    bw += bw*clamp((pow(fft*1.0,3.)-23.),.0,.7);
    bw = min(bw,1.99);
    
    //noise texture
	bw *= texture(iChannel2, vec2(gl_FragCoord.xy/(256.*iDotSize))).x;

	//vignette effect
	float vignette = length(uv)*VIGNETTEINTENSITY;
	bw -= vignette;
	
    //pseudo pixels (dots)
    float d = length(fract(uv*cColumns)-.5);
    float peakcolor = smoothstep(.35,.00,d)*bw;
    float basecolor = smoothstep(.85,.00,d)*bw;

    //vec3 col = vec3(basecolor*RED+peakcolor,basecolor*GREEN+peakcolor,basecolor*BLUE+peakcolor);
    vec3 col = basecolor*cColor+peakcolor;

    col *= INTENSITY;    
    
    FragColor = vec4(col,1.0);
}
