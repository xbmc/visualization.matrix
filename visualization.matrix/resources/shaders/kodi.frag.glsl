#define DOTSIZE 5.
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
    
    //VHS-like distortions
    float wav = texture( iChannel0, vec2((uv.y +1.)*.5,1.0) ).x-.5;
    
    
    //old
	float distort = sign(wav) * max(abs(wav)-DISTORTTHRESHOLD,.0);
	//float distort = sign(wav) * smoothstep(.3,.95,abs(wav));
    
    
    
    
    
    
    //KODI texture
    //float textureDisto
    float tex = texture(iChannel1, uv+.5-vec2(distort*DISTORTFACTORX,distort*DISTORTFACTORY)).x;
    //float tex = texture(iChannel1, uv+.5-vec2(distort,distort)).x;
    //float tex = texture(iChannel1, uv+.5).x;
    tex *= .9 - wav*.2;
    //Shadow effect around the KODI texture. Needs a prepared texture to work.
    float shadow = (wav+.5)*.25;
    tex = (max(shadow,tex)-shadow)/(1.-shadow);
    //tex += abs(wav)*.125;
        
        
    //hack for non-clamped texture
    //tex = (abs(uv.x) < .5) ? tex : 0.0;
    
    
    //"interlaced" logo distortion effect
    //float line = mod(gv.y,2.);
    // flips affected "field", comment out for performance
    //float line = mod(gv.y+floor(iTime*8.),2.);
    float line = mod(gv.y*sign(wav),2.);
    tex *= 1. - (line*10.*abs(distort) + 5.*abs(distort));
    //float line = mod(gv.y+floor(iTime*8.),2.);
    //tex -= line*abs(distort);
    
    
    //limit overall intensity
    bw = bw*max(tex,MININTENSITY);
    
    
    //brightens lines where distortion are occuring
    //bw += abs(distort)*.45;
    //bw = smoothstep(0.,1.
    //bw = smoothstep(0.,1.-abs(distort*.5),bw);*.5),bw);
	//bw += abs(distort)*.3;
	bw += min(abs(distort)*.7,.0105);
	//bw -= bw*abs(distort)*5.;
    
    
    //FFT stuff (visualization)
    //might need some scaling
    //float fft = texture( iChannel0, vec2((1.-abs(uv.x))*.7+wav*.05,0.0) ).x;
    //float fft = texture( iChannel0, vec2((1.-abs(uv.x))*.7+distort*.2,0.0) ).x;
    float fft = texture( iChannel0, vec2((1.-abs(uv.x-distort*.2))*.7,0.0) ).x;
    //float fft = texture( iChannel0, vec2((1.-abs(uv.x))*.7,0.0) ).x;
    fft *= (3.2 -abs(0.-uv.x*1.3))*0.75;
    fft *= 2.;
    //fft *= 3.;
    
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

    //basecolor = bw;
    /*
    if (uv.x < 0) {
		basecolor += .2*bw;
		basecolor *= .75;
	};*/
    //output
    vec3 col = vec3(peakcolor+basecolor*RED,peakcolor+basecolor*GREEN,peakcolor+basecolor*BLUE);
    col *= INTENSITY;
    vec3 tmp4 = col;
    
    
    fragColor = vec4(col,1.0);
    //fragColor = vec4(((fragCoord)/(256.*DOTSIZE+shift*256.*DOTSIZE)),0.,1.0);
    //fragColor = vec4(tmp3,0.,0.,1.0);
    //fragColor = vec4(mod(gv.y,2.)*8.*abs(distort),.0,.0,1.0);
    //fragColor = vec4((floor(fract(uv*columns*.5)*2.)*0.5+0.5),distort,1.0);
    //fragColor = vec4(line2,line2,line2,1.0);
    //fragColor = vec4(columns*.1-1.5,.0,.0,1.0);
    //fragColor = vec4(0.,0.,d,1.0);
    //fragColor = vec4(1.-abs(uv.x),uv.y,0.,1.0);
   
    
}
