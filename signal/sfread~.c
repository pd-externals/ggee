/* (C) Guenter Geiger <geiger@epy.co.at> */


#include <m_imp.h>
/*#include <m_pd.h>*/
#include "g_canvas.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include <stdio.h>
#include <string.h>
#ifndef NT
#include <unistd.h>
#include <sys/mman.h>
#else
#include <io.h>
#endif


#include <fcntl.h>
#include <sys/stat.h>

/* ------------------------ sfread~ ----------------------------- */

#ifdef NT
#define BINREADMODE "rb"
#else
#define BINREADMODE "r"
#endif

static t_class *sfread_class;


typedef struct _sfread
{
     t_object x_obj;
     void*     x_mapaddr;
     int       x_fd;

     t_int   x_play;
     t_int   x_channels;
     t_int   x_size;
     t_int   x_loop;
     t_float x_offset;
     t_float x_skip;
     t_float x_speed;

     t_glist * x_glist;
     t_outlet *x_bangout;
} t_sfread;


void sfread_open(t_sfread *x,t_symbol *filename)
{
     struct stat  fstate;
     char fname[MAXPDSTRING];

     if (filename == &s_) {
	  post("sfread: open without filename");
	  return;
     }

     canvas_makefilename(glist_getcanvas(x->x_glist), filename->s_name,
			 fname, MAXPDSTRING);


     /* close the old file */

     if (x->x_mapaddr) munmap(x->x_mapaddr,x->x_size);
     if (x->x_fd >= 0) close(x->x_fd);

     if ((x->x_fd = open(fname,O_RDONLY)) < 0)
     {
	  error("can't open %s",fname);
	  x->x_play = 0;
	  x->x_mapaddr = NULL;
	  return;
     }

     /* get the size */

     fstat(x->x_fd,&fstate);
     x->x_size = fstate.st_size;

     /* map the file into memory */

     if (!(x->x_mapaddr = mmap(NULL,x->x_size,PROT_READ,MAP_PRIVATE,x->x_fd,0)))
     {
	  error("can't mmap %s",fname);
	  return;
     }
}

#define MAX_CHANS 4

static t_int *sfread_perform(t_int *w)
{
     t_sfread* x = (t_sfread*)(w[1]);
     short* buf = x->x_mapaddr;
/*     t_float *in = (t_float *)(w[2]); will we need this (indexing) ?*/
     int c = x->x_channels;
     t_float offset = x->x_offset*c;
     t_float speed = x->x_speed;
     int i,n;
     int end =  x->x_size/sizeof(short);
     t_float* out[MAX_CHANS];
     
     for (i=0;i<c;i++)  
	  out[i] = (t_float *)(w[3+i]);
     n = (int)(w[3+c]);
     
     /* loop */

     if (offset >  end)
	  offset = end;

     if (offset + n*c*speed > end) { // playing forward end
	  if (!x->x_loop) {
	       x->x_play=0;
	       offset = x->x_skip*c;
	  }
     }

     if (offset + n*c*speed < 0) {  // playing backwards end
	  if (!x->x_loop) {
	       x->x_play=0;
	       offset = end;
	  }

     }


     if (x->x_play && x->x_mapaddr) {

	  if (speed != 1) { /* different speed */
	       float aoff = (((int)offset)>>1)<<1;
	       float frac = offset - (int)offset;
	       while (n--) {
		    for (i=0;i<c;i++)  {
	                 t_sample as = *(buf+(int)aoff+i)*3.052689e-05;
	                 t_sample bs = *(buf+(int)aoff+i+c)*3.052689e-05;

			 *out[i]++ = as + frac*(bs-as);
		    }
		    offset+=speed*c;
		    aoff = (((int)offset)>>1)<<1;
		    if (aoff > end) { 
			 if (x->x_loop) aoff = x->x_skip;
			 else break;
		    }
		    if (aoff < 0) {
			 if (x->x_loop) aoff = end;
			 else break;
		    }
	       }
	       /* Fill with zero in case of end */ 
	       n++;
	       while (n--) 
		    for (i=0;i<c;i++)  
			 *out[i]++ = 0;
	       offset = aoff;
	  }
	  else { /* speed == 1 */
	       int aoff = (((int)offset)>>1)<<1;
	       while (n--) {
		    for (i=0;i<c;i++)  {
			 *out[i]++ = *(buf+aoff+i)*3.052689e-05;
		    }
		    aoff+=c;
		    if (aoff > end) {
			 if (x->x_loop) aoff = x->x_skip;
			 else break;
		    }
	       }

	       /* Fill with zero in case of end */ 
	       n++;
	       while (n--) 
		    for (i=0;i<c;i++)  
			 *out[i]++ = 0.;
	       offset = aoff;	       
	  }

     }
     else {
	  while (n--) {
	       for (i=0;i<c;i++)
		    *out[i]++ = 0.;
	  }
     }
     x->x_offset = offset/c; /* this should always be integer !! */
     return (w+c+4);
}


static void sfread_float(t_sfread *x, t_floatarg f)
{
     int t = f;
     if (t && x->x_mapaddr) {
	  x->x_play=1;
     }
     else {
	  x->x_play=0;
     }

}

static void sfread_loop(t_sfread *x, t_floatarg f)
{
     x->x_loop = f;
}



static void sfread_size(t_sfread* x)
{
     t_atom a;

     SETFLOAT(&a,x->x_size*0.5/x->x_channels);
     outlet_list(x->x_bangout, gensym("size"),1,&a);
}

static void sfread_state(t_sfread* x)
{
     t_atom a;

     SETFLOAT(&a,x->x_play);
     outlet_list(x->x_bangout, gensym("state"),1,&a);
}




static void sfread_bang(t_sfread* x)
{
     x->x_offset = x->x_skip*x->x_channels;
     sfread_float(x,1.0);
}


static void sfread_dsp(t_sfread *x, t_signal **sp)
{
/*     post("sfread: dsp"); */
     switch (x->x_channels) {
     case 1:
	  dsp_add(sfread_perform, 4, x, sp[0]->s_vec, 
		  sp[1]->s_vec, sp[0]->s_n);
	  break;
     case 2:
	  dsp_add(sfread_perform, 5, x, sp[0]->s_vec, 
		  sp[1]->s_vec,sp[2]->s_vec, sp[0]->s_n);
	  break;
     case 4:
	  dsp_add(sfread_perform, 6, x, sp[0]->s_vec, 
		  sp[1]->s_vec,sp[2]->s_vec,
		  sp[3]->s_vec,sp[4]->s_vec,
		  sp[0]->s_n);
	  break;
     }
}


static void *sfread_new(t_floatarg chan,t_floatarg skip)
{
    t_sfread *x = (t_sfread *)pd_new(sfread_class);
    t_int c = chan;

    x->x_glist = (t_glist*) canvas_getcurrent();

    if (c<1 || c > MAX_CHANS) c = 1;
    floatinlet_new(&x->x_obj, &x->x_offset);
    floatinlet_new(&x->x_obj, &x->x_speed);


    x->x_fd = -1;
    x->x_mapaddr = NULL;

    x->x_size = 0;
    x->x_loop = 0;
    x->x_channels = c;
    x->x_mapaddr=NULL;
    x->x_offset = skip;
    x->x_skip = skip;
    x->x_speed = 1.0;
    x->x_play = 0;

    while (c--) {
	 outlet_new(&x->x_obj, gensym("signal"));
    }

     x->x_bangout = outlet_new(&x->x_obj, &s_float);

/*  post("sfread: x_channels = %d, x_speed = %f",x->x_channels,x->x_speed);*/

    return (x);
}

void sfread_tilde_setup(void)
{
     /* sfread */

    sfread_class = class_new(gensym("sfread~"), (t_newmethod)sfread_new, 0,
    	sizeof(t_sfread), 0,A_DEFFLOAT,A_DEFFLOAT,0);
    class_addmethod(sfread_class, nullfn, gensym("signal"), 0);
    class_addmethod(sfread_class, (t_method) sfread_dsp, gensym("dsp"), 0);
    class_addmethod(sfread_class, (t_method) sfread_open, gensym("open"), A_SYMBOL,A_NULL);
    class_addmethod(sfread_class, (t_method) sfread_size, gensym("size"), 0);
    class_addmethod(sfread_class, (t_method) sfread_state, gensym("state"), 0);
    class_addfloat(sfread_class, sfread_float);
    class_addbang(sfread_class,sfread_bang);
    class_addmethod(sfread_class,(t_method)sfread_loop,gensym("loop"),A_FLOAT,A_NULL);

}






