/* (C) Guenter Geiger <geiger@epy.co.at> */


#ifndef VERSION
#define VERSION "unknown"
#endif

#include <m_pd.h>

#define EXPERIMENTAL

#ifndef __DATE__ 
#define __DATE__ "without using a gnu compiler"
#endif

typedef struct _ggext
{
     t_object x_obj;
} t_ggext;

static t_class* ggext_class;

void sfplay_setup();
void streamin_tilde_setup();     
void streamout_tilde_setup();
void fofsynth_setup();
void sfread_tilde_setup();
void sfwrite_tilde_setup();
void state_setup();
void slider_setup();
void hslider_setup();
void toddle_setup();
void envgen_setup();
void ticker_setup();
void unwonk_setup();
void atan2_tilde_setup(void);
void log_tilde_setup(void);
void exp_tilde_setup(void);
void sinh_setup(void);
void inv_setup();
void scalarinv_setup();
void rtout_setup(void);
void fasor_tilde_setup(void);
void sl_setup();
void rl_setup();
void trsync_tilde_setup();
void serialize_setup();
void vbap_setup();
void hlshelf_setup();
void lowpass_setup();
void     highpass_setup();
void     bandpass_setup();
void     notch_setup();
void     lowshelf_setup();
void     highshelf_setup();
void equalizer_setup();
void constant_setup(void );
void     mixer_tilde_setup();
void     stripdir_setup();
void     qread_setup();
void joystick_setup();

#ifdef HAVE_LIBSTK
void stk_setup();
#endif
void serialctl_setup();
void serial_ms_setup();
void serial_bird_setup();
void shell_setup();

static void* ggext_new(t_symbol* s) {
    t_ggext *x = (t_ggext *)pd_new(ggext_class);
    return (x);
}

void ggext_setup(void) 
{
    ggext_class = class_new(gensym("ggext"), (t_newmethod)ggext_new, 0,
    	sizeof(t_ggext), 0,0);

     streamin_tilde_setup();     
     streamout_tilde_setup();
     fofsynth_tilde_setup();
#ifdef unix
     sfread_tilde_setup();
     sfwrite_tilde_setup();
     serialctl_setup();
     serial_ms_setup();
     serial_bird_setup();
     rtout_setup();
     shell_setup();
#endif
     serialize_setup();
     sfplay_tilde_setup();
     state_setup();
     slider_setup();
     hslider_setup();
     toddle_setup();
     envgen_setup();
     ticker_setup();
     unwonk_setup();
     atan2_tilde_setup();
     log_tilde_setup();
     exp_tilde_setup();
     sinh_setup();
     inv_setup();
     scalarinv_setup();
     fasor_tilde_setup();
     sl_setup();
     rl_setup();
#ifdef HAVE_LIBSTK
     stk_setup();
#endif
     vbap_setup();
     hlshelf_setup();
     lowpass_setup();
     highpass_setup();
     bandpass_setup();
     notch_setup();
     lowshelf_setup();
     highshelf_setup();
     equalizer_setup();
     qread_setup();
     joystick_setup();
#ifdef EXPERIMENTAL
     constant_setup();
     mixer_tilde_setup();
     stripdir_setup();
#endif

     post("GGEXT: Guenter Geiger");
     post("GGEXT: ver: "VERSION);
     post("GGEXT: compiled: "__DATE__);
}
