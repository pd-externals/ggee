/* (C) Guenter Geiger <geiger@epy.co.at> */


#include <m_pd.h>
#include "math.h"

#define UNITBIT32 1572864.  /* 3*2^19; bit 32 has place value 1 */

    /* machine-dependent definitions.  These ifdefs really
    should have been by CPU type and not by operating system! */
#ifdef IRIX
    /* big-endian.  Most significant byte is at low address in memory */
#define HIOFFSET 0    /* word offset to find MSB */
#define LOWOFFSET 1    /* word offset to find LSB */
#define int32 long  /* a data type that has 32 bits */
#else
#ifdef NT
    /* little-endian; most significant byte is at highest address */
#define HIOFFSET 1
#define LOWOFFSET 0
#define int32 long
#else
#ifdef  __linux__

#include <endian.h>

#if !defined(__BYTE_ORDER) || !defined(__LITTLE_ENDIAN)                         
#error No byte order defined                                                    
#endif                                                                          
                                                                                
#if __BYTE_ORDER == __LITTLE_ENDIAN                                             
#define HIOFFSET 1                                                              
#define LOWOFFSET 0                                                             
#else                                                                           
#define HIOFFSET 0    /* word offset to find MSB */                             
#define LOWOFFSET 1    /* word offset to find LSB */                            
#endif /* __BYTE_ORDER */                                                       

#include <sys/types.h>
#define int32 int32_t

#endif /* __linux__ */
#endif /* NT */
#endif /* SGI */

union tabfudge
{
    double tf_d;
    int32 tf_i[2];
};

typedef struct fasor
{
     t_object x_obj;
     double x_phase;
     float x_conv;
     float x_f;	    /* used for scalar only */
     float x_fnew;	    /* used for scalar only */
     double x_flast;	    /* used for scalar only */
} t_fasor;

static t_class *fasor_class;

static void *fasor_new(t_symbol *s, int argc, t_atom *argv)
{
    t_fasor *x;
    x = (t_fasor *)pd_new(fasor_class);

    if (argc)
	 x->x_f =  atom_getfloatarg(0, argc, argv);
    else
	 x->x_f = 0;


    x->x_flast = 1.0;
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("ft1"));
    x->x_phase = 0;
    x->x_conv = 0;
    outlet_new(&x->x_obj, gensym("signal"));
    return (x);
}

static t_int *fasor_perform(t_int *w)
{
    t_fasor *x = (t_fasor *)(w[1]);
    t_float *out = (t_float *)(w[2]);
    int n = (int)(w[3]);

    double dphase = x->x_phase + UNITBIT32;
    union tabfudge tf;
    int normhipart;
    float conv = x->x_conv;
    double freq = x->x_f;
    double flast = x->x_flast;
    double fout;

    tf.tf_d = UNITBIT32;
    normhipart = tf.tf_i[HIOFFSET];
    tf.tf_d = dphase;
    fout = tf.tf_d;

    while (n--)
    {
    	tf.tf_i[HIOFFSET] = normhipart;
    	dphase += freq * conv;
	*out++ = (t_float) (fout = tf.tf_d - UNITBIT32);
	tf.tf_d = dphase;

	if (fout <= flast) {
	     freq = x->x_f = x->x_fnew; /*!! performance if freq = 0 ?? */ 
	}

	flast = fout;
    }

    x->x_flast = flast;
    tf.tf_i[HIOFFSET] = normhipart;
    x->x_phase = tf.tf_d - UNITBIT32;
    return (w+4);
}

static void fasor_dsp(t_fasor *x, t_signal **sp)
{
     
    x->x_conv = 1.f/sp[0]->s_sr;
    dsp_add(fasor_perform, 3, x, sp[0]->s_vec, sp[0]->s_n);
}

static void fasor_ft1(t_fasor *x, t_float f)
{
    x->x_phase = f;
}

static void fasor_float(t_fasor *x, t_float f)
{
    x->x_fnew = (float) fabs(f);
}


void fasor_tilde_setup(void)
{
    fasor_class = class_new(gensym("fasor~"), (t_newmethod)fasor_new, 0,
    	sizeof(t_fasor), 0, A_GIMME, 0);
    class_addmethod(fasor_class, nullfn, gensym("signal"), 0);
    class_addmethod(fasor_class, (t_method)fasor_dsp, gensym("dsp"), 0);
    class_addmethod(fasor_class, (t_method)fasor_ft1,
    	gensym("ft1"), A_FLOAT, 0);
    class_addfloat(fasor_class,fasor_float);
}
