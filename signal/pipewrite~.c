/* (C) Guenter Geiger <geiger@epy.co.at> */


#include <m_pd.h>
#include "g_canvas.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

/*
 * -------------------------- pipewrite~ -------------------------------
 */

#define MAX_CHANS 4
#define BLOCKTIME 0.001
#define uint32 unsigned int
#define uint16 unsigned short

static t_class *pipewrite_class;

typedef struct _pipewrite
{
     t_object x_obj;
     t_symbol* filename;
     int x_file;
     t_int x_channels;
     t_int  size;
     t_glist * x_glist;
     t_int x_blocked;
     t_int x_blockwarn;
     short maxval;
} t_pipewrite;

static void pipewrite_close(t_pipewrite *x)
{
     if (x->x_file > 0) {
	  close(x->x_file);
     }
     x->x_file = -1;
     x->size=0;
}


static void pipewrite_open(t_pipewrite *x,t_symbol *filename)
{
     char fname[MAXPDSTRING];

     if (filename == &s_) {
	  post("pipewrite: open without filename");
	  return;
     }

     canvas_makefilename(glist_getcanvas(x->x_glist), filename->s_name,
			 fname, MAXPDSTRING);
     x->x_blocked = 0;
     x->filename = filename;
     x->maxval=0;
     x->size=0;
     post("pipewrite: filename = %s",x->filename->s_name);

     pipewrite_close(x);
     
     if ((x->x_file = open(fname,O_WRONLY | O_NONBLOCK)) < 0)
     {

	  error("can't open pipe %s: ",fname);
	  perror("system says");
       return;
     }
}

static void pipewrite_block(t_pipewrite *x, t_floatarg f)
{
     x->x_blockwarn = f;
}


static short out[4*64];

static t_int *pipewrite_perform(t_int *w)
{
     t_pipewrite* x = (t_pipewrite*)(w[1]);
     t_float * in[4];
     int c = x->x_channels;
     int i,num,n;
     short* tout = out;
     int ret;
     double timebefore,timeafter;
     double late;
     struct sigaction action;     
     struct sigaction oldaction;
     

     if (x->x_file < 0) {
	 pipewrite_open(x,x->filename);
     }	  

     action.sa_handler = SIG_IGN;
     sigaction(SIGPIPE, &action, &oldaction);

     for (i=0;i < c;i++) {
	  in[i] = (t_float *)(w[2+i]);     
     }

     n = num = (int)(w[2+c]);

     /* loop */

     if (x->x_file >= 0) {

	  while (n--) {
	       for (i=0;i<c;i++)  {
                    if (*(in[i]) > 1. ) { *(in[i]) = 1. ; }
                    if (*(in[i]) < -1. ) { *(in[i]) = -1. ; }
		    *tout++ =  (*(in[i])++ * 32768.);
	       }
	  }
	  
	  timebefore = sys_getrealtime();
	  if ((ret = write(x->x_file,out,sizeof(short)*num*c)) < sizeof(short)*num*c) { 
	       post("pipewrite: short write %d",ret);
	       
	  }

	  timeafter = sys_getrealtime();
	  late = timeafter - timebefore;
          x->size +=ret;
	  /* OK, we let only 10 ms block here */
	  if (late > BLOCKTIME && x->x_blockwarn) { 
	       x->x_blocked++;
	       if (x->x_blocked > x->x_blockwarn) {
		    x->x_blocked=0;
/*		    post("maximum blockcount %d reached",x->x_blockwarn); */
	       }
	  }
     }

     sigaction(SIGPIPE, &oldaction, NULL);

     return (w+3+c);
}



static void pipewrite_dsp(t_pipewrite *x, t_signal **sp)
{
     switch (x->x_channels) {
     case 1:
	  dsp_add(pipewrite_perform, 3, x, sp[0]->s_vec, 
		   sp[0]->s_n);
	  break;
     case 2:
	  dsp_add(pipewrite_perform, 4, x, sp[0]->s_vec, 
		  sp[1]->s_vec, sp[0]->s_n);
	  break;
     case 4:
	  dsp_add(pipewrite_perform, 6, x, sp[0]->s_vec, 
		  sp[1]->s_vec,
		  sp[2]->s_vec,
		  sp[3]->s_vec,
		  sp[0]->s_n);
	  break;
     }
}

static void pipewrite_free(t_pipewrite* x)
{
     pipewrite_close(x);
}


static void *pipewrite_new(t_symbol* name,t_floatarg chan)
{
    t_pipewrite *x = (t_pipewrite *)pd_new(pipewrite_class);
    t_int c = chan;

    if (c<1 || c > MAX_CHANS) c = 1;

    x->x_glist = (t_glist*) canvas_getcurrent();
    x->x_channels = c--;
    post("channels:%d",x->x_channels);
    x->x_file=0;
    x->x_blocked = 0;
    x->x_blockwarn = 10;
    while (c--) {
	 inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    }

    if (name && name != &s_) { /* start the pipe */
	 pipewrite_open(x,name);
    }

    return (x);
}

void pipewrite_tilde_setup(void)
{
     pipewrite_class = class_new(gensym("pipewrite~"), (t_newmethod)pipewrite_new, (t_method)pipewrite_free,
    	sizeof(t_pipewrite), 0,A_DEFSYM,A_DEFFLOAT,A_NULL);
     class_addmethod(pipewrite_class,nullfn,gensym("signal"), 0);
     class_addmethod(pipewrite_class, (t_method) pipewrite_dsp, gensym("dsp"), 0);
     class_addmethod(pipewrite_class, (t_method) pipewrite_open, gensym("open"), A_SYMBOL,A_NULL);
     class_addmethod(pipewrite_class, (t_method) pipewrite_close, gensym("close"), 0);
    class_addmethod(pipewrite_class, (t_method)pipewrite_block,gensym("block"),A_DEFFLOAT,0);
     
}
