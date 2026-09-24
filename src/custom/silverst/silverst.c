/* SilverSt -- original true-stereo clean amp + fixed open-back 2x12 cab.
 * Reference algorithm: tools/audio_preview/renderers/silverst.py.
 * No audio is crossed or summed: only the sag detector is shared. */
#include <stdint.h>
#include "../../airwindows/common/zoom_params.h"
#include "silverst_params.h"
#ifndef SILVERST_AUDIO_FUNC
#define SILVERST_AUDIO_FUNC Fx_DLY_SilverSt
#endif
#define PRAGMA_(x) _Pragma(#x)
#define CODESEC(f) PRAGMA_(CODE_SECTION(f, ".audio"))
#define INLINE(f) PRAGMA_(FUNC_ALWAYS_INLINE(f))
CODESEC(SILVERST_AUDIO_FUNC)
#define PTR(t,x) ((t)(uintptr_t)(x))
#define SL_MAGIC 0x534C5652u
#define SL_VERSION 1u
typedef struct {
 uint32_t magic,version,initialized,paramsLive;
 float dc[2],inter[2],lo5[2],lo2[2],r75[2],r180[2],pre[2];
 float voiceZ1[2][3],voiceZ2[2][3],cabZ1[2][6],cabZ2[2][6];
 float env,smooth[9];
} SilverStState;
INLINE(sl_clamp) static inline float sl_clamp(float x,float a,float b){if(x<a)return a;if(x>b)return b;return x;}
INLINE(sl_flush) static inline float sl_flush(float x){if(x!=x||x>1e12f||x<-1e12f)return 0.0f;if(x>-1e-25f&&x<1e-25f)return 0.0f;return x;}
INLINE(sl_abs) static inline float sl_abs(float x){return x<0?-x:x;}
INLINE(sl_soft) static inline float sl_soft(float x){x=sl_clamp(x,-1.0f,1.0f);return 1.5f*x-0.5f*x*x*x;}
INLINE(sl_limit) static inline float sl_limit(float x){float a=sl_abs(x);float y,t;if(a<=.8f)return x;if(a>=1.2f)return x<0?-1.0f:1.0f;t=(a-.8f)*2.5f;y=.8f+.266666667f*(t-.25f*t*t);return x<0?-y:y;}
INLINE(sl_param) static inline float sl_param(float raw,float fallback,uint32_t live){if(!live)return fallback;if(raw<=0.0f)return 0.0f;if(raw<=1.0f)return raw;return sl_clamp(raw*.01f,0.0f,1.0f);}
INLINE(sl_mix) static inline float sl_mix(float c,float e,float t){return c+(e-c)*t;}
INLINE(sl_bq) static inline float sl_bq(float x,float *z1,float *z2,float b0,float b1,float b2,float a1,float a2){float y=b0*x+*z1;*z1=b1*x-a1*y+*z2;*z2=b2*x-a2*y;return y;}
void SILVERST_AUDIO_FUNC(unsigned int *ctx){
 float *params=PTR(float*,ctx[1]); float *buf=PTR(float*,ctx[5]);
 unsigned int *src=PTR(unsigned int*,ctx[12]); unsigned int *dst=PTR(unsigned int*,*(unsigned int*)PTR(unsigned int*,ctx[11])); *dst=*src;
 if(params[0]<.5f)return;
 volatile unsigned int *desc=PTR(volatile unsigned int*,ctx[3]); uintptr_t base,end,span,stateBase;
 if(!desc)return; base=(uintptr_t)desc[0];end=(uintptr_t)desc[1];span=(uintptr_t)desc[2];stateBase=(base+3u)&~(uintptr_t)3u;
 if(!base||end<=base||(base&3u)||(end&3u)||(span&3u)||end-base<sizeof(SilverStState)||span<end-base||stateBase+sizeof(SilverStState)>end)return;
 SilverStState *st=(SilverStState*)stateBase;
 if(st->magic!=SL_MAGIC||st->version!=SL_VERSION){uint32_t *w=(uint32_t*)st;uint32_t i;for(i=0;i<(uint32_t)(sizeof(SilverStState)>>2);i++)w[i]=0u;st->magic=SL_MAGIC;st->version=SL_VERSION;st->initialized=1u;return;}
 /* Recursive states are bounded by stable one-poles and the output limiter.
  * The expensive denormal/NaN scrub is therefore a block guard, not paid
  * twenty times per sample. */
 if(st->env!=st->env||st->env>1e6f||st->env<-1e6f){uint32_t *w=(uint32_t*)st;uint32_t i;for(i=4u;i<(uint32_t)(sizeof(SilverStState)>>2);i++)w[i]=0u;}
 if(!st->paramsLive){if(params[5]>.0001f||params[6]>.0001f||params[7]>.0001f||params[8]>.0001f||params[9]>.0001f||params[10]>.0001f||params[11]>.0001f||params[12]>.0001f||params[13]>.0001f)st->paramsLive=1u;}
 {uint32_t live=st->paramsLive;int f;
  /* Scalar updates intentionally avoid a block-local parameter array. */
  st->smooth[0]=sl_flush(st->smooth[0]+.025f*(sl_param(params[5],.25f,live)-st->smooth[0]));
  st->smooth[1]=sl_flush(st->smooth[1]+.025f*(sl_param(params[6],.30f,live)-st->smooth[1]));
  st->smooth[2]=sl_flush(st->smooth[2]+.025f*(sl_param(params[7],.60f,live)-st->smooth[2]));
  st->smooth[3]=sl_flush(st->smooth[3]+.025f*(sl_param(params[8],.50f,live)-st->smooth[3]));
  st->smooth[4]=sl_flush(st->smooth[4]+.025f*(sl_param(params[9],.50f,live)-st->smooth[4]));
  st->smooth[5]=sl_flush(st->smooth[5]+.025f*(sl_param(params[10],.50f,live)-st->smooth[5]));
  st->smooth[6]=sl_flush(st->smooth[6]+.025f*(sl_param(params[11],.45f,live)-st->smooth[6]));
  st->smooth[7]=sl_flush(st->smooth[7]+.025f*(sl_param(params[12],.50f,live)-st->smooth[7]));
  st->smooth[8]=sl_flush(st->smooth[8]+.025f*(sl_param(params[13],.35f,live)-st->smooth[8]));
  float g=st->smooth[0],preg=1.0f+9.0f*g*g,power=1.0f+2.5f*g*g*g;
  float gb=1.0f+(st->smooth[3]-.5f)*1.8f,gm=1.0f+(st->smooth[4]-.5f)*1.8f,gt=1.0f+(st->smooth[5]-.5f)*1.8f;
  float reson=-.20f+st->smooth[6]*.75f,pres=-.25f+st->smooth[7]*.70f,baseMic=st->smooth[8],spread=.22f*st->smooth[1],micL=sl_clamp(baseMic-spread,0,1),micR=sl_clamp(baseMic+spread,0,1),level=.25f+1.25f*st->smooth[2];
  for(f=0;f<8;f++){
   float shapedL,shapedR,x,det,sag;int c;
   for(c=0;c<2;c++){x=buf[f+c*8];st->dc[c]+=.004265f*(x-st->dc[c]);x-=st->dc[c];x=sl_soft(.62f*(x*preg+.09f))-sl_soft(.0558f);st->inter[c]+=.68f*(x-st->inter[c]);x=sl_soft(st->inter[c]*.72f);st->lo5[c]+=.06876f*(x-st->lo5[c]);st->lo2[c]+=.24795f*(x-st->lo2[c]);x=gb*st->lo5[c]+gm*(st->lo2[c]-st->lo5[c])+gt*(x-st->lo2[c]);
    x=sl_bq(x,&st->voiceZ1[c][0],&st->voiceZ2[c][0],1.001165396f,-1.974142276f,.973291473f,-1.974167224f,.974431921f);x=sl_bq(x,&st->voiceZ1[c][1],&st->voiceZ2[c][1],.979209230f,-1.848912489f,.878414669f,-1.848912489f,.857623898f);x=sl_bq(x,&st->voiceZ1[c][2],&st->voiceZ2[c][2],1.046695227f,-1.506015496f,.592619998f,-1.506015496f,.639315225f);if(c==0)shapedL=x;else shapedR=x;}
   det=sl_abs(shapedL);if(sl_abs(shapedR)>det)det=sl_abs(shapedR);st->env+=((det>st->env)?.004525f:.000189f)*(det-st->env);sag=1.0f-.18f*g*sl_clamp((st->env-.125f)*4.0f,0,1);
   for(c=0;c<2;c++){float t=(c==0?micL:micR);x=(c==0?shapedL:shapedR);x=sl_soft(x*power*sag);st->r75[c]+=.01063f*(x-st->r75[c]);st->r180[c]+=.02532f*(x-st->r180[c]);x+=reson*(st->r180[c]-st->r75[c]);st->pre[c]+=.347f*(x-st->pre[c]);x+=pres*(x-st->pre[c]);
    x=sl_bq(x,&st->cabZ1[c][0],&st->cabZ2[c][0],sl_mix(.992972613f,.992372568f*1.183f,t),sl_mix(-1.985945226f,-1.984745136f*1.183f,t),sl_mix(.992972613f,.992372568f*1.183f,t),sl_mix(-1.985895841f,-1.984686958f,t),sl_mix(.985994611f,.984803315f,t));
    x=sl_bq(x,&st->cabZ1[c][1],&st->cabZ2[c][1],sl_mix(1.002336864f,1.001619634f*1.183f,t),sl_mix(-1.975178772f,-1.973433550f*1.183f,t),sl_mix(.973185056f,.972165118f*1.183f,t),sl_mix(-1.975228881f,-1.973469666f,t),sl_mix(.975471811f,.973748636f,t));
    x=sl_bq(x,&st->cabZ1[c][2],&st->cabZ2[c][2],sl_mix(.976641521f,.965036720f*1.183f,t),sl_mix(-1.830367058f,-1.799915303f*1.183f,t),sl_mix(.863398598f,.845482309f*1.183f,t),sl_mix(-1.830367058f,-1.799915303f,t),sl_mix(.840040120f,.810519028f,t));
    x=sl_bq(x,&st->cabZ1[c][3],&st->cabZ2[c][3],sl_mix(1.048980473f,1.007741381f*1.183f,t),sl_mix(-1.525158222f,-1.558100276f*1.183f,t),sl_mix(.611171830f,.663736676f*1.183f,t),sl_mix(-1.525158222f,-1.558100276f,t),sl_mix(.660152303f,.671478057f,t));
    x=sl_bq(x,&st->cabZ1[c][4],&st->cabZ2[c][4],sl_mix(.100174055f,.075040791f*1.183f,t),sl_mix(.200348110f,.150081581f*1.183f,t),sl_mix(.100174055f,.075040791f*1.183f,t),sl_mix(-.927153709f,-1.089785638f,t),sl_mix(.327849929f,.389948800f,t));
    x=sl_bq(x,&st->cabZ1[c][5],&st->cabZ2[c][5],sl_mix(.605507871f,.496134962f*1.183f,t),sl_mix(-.454449615f,-.423880831f*1.183f,t),sl_mix(.137750153f,.128993947f*1.183f,t),sl_mix(-1.050466253f,-1.216872628f,t),sl_mix(.339274662f,.418120705f,t));buf[f+c*8]=sl_limit(x*level*(1.0f-.6f*t));}
  }
 }
}
