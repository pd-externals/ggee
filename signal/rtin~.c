/* (C) Guenter Geiger <geiger@epy.co.at> */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>

#ifdef NT
#include <sys/timeb.h>
void gettimeofday(struct timeval* t,void* timezone)
{       struct timeb timebuffer;
        ftime( &timebuffer );
        t->tv_sec=timebuffer.time;
        t->tv_usec=1000*timebuffer.millitm;
}
#endif

#include "math.h"
#include <m_pd.h>


/* ----------------------------- rtin ----------------------------- */
static t_class *rtin_class;


#define INVTWOPI 0.15915494f

typedef struct _rtin
{
     t_object x_obj;
     t_int  fd;
     t_int usec;
} t_rtin;

static void *rtin_new(t_symbol *s, int argc, t_atom *argv)
{
     t_rtin *x = (t_rtin *)pd_new(rtin_class);
     /*	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);*/
     outlet_new(&x->x_obj, &s_float);

     x->usec=0;
     x->fd = open("/dev/midi00",O_RDWR|O_NDELAY);

     return (x);

}

t_int *rtin_perform(t_int *w)
{
     struct timeval tv;
     long diff;
    t_rtin* x = (t_rtin *)(w[1]);
//    int n = (int)(w[2]);
    char c;


    if (read(x->fd,&c,1) == 1) {
	 gettimeofday(&tv, NULL);
	 diff = tv.tv_usec - x->usec;
	 if (diff < 0) diff+=1000000;
	 if (diff > 10000) outlet_float(x->x_obj.ob_outlet,diff*0.001);
	 x->usec=tv.tv_usec;
    }


    return (w+2);
}

static void rtin_dsp(t_rtin *x, t_signal **sp)
{
     dsp_add(rtin_perform, 1, x);
}

void rtin_tilde_setup(void)
{
    rtin_class = class_new(gensym("rtin~"), (t_newmethod)rtin_new, 0,
    	sizeof(t_rtin), 0, A_GIMME, 0);
    class_addmethod(rtin_class, nullfn, gensym("signal"), 0);
    class_addmethod(rtin_class, (t_method)rtin_dsp, gensym("dsp"), 0);
}
