/* (C) Guenter Geiger <geiger@epy.co.at> */


#include <m_pd.h>
#include <math.h>
#ifdef _MSC_VER
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ------------------------ fofsynth~ ----------------------------- */

#ifndef _WIN32
void garray_usedindsp(t_garray *x);
#endif

#define DEBUG(a,b) if (x->debug) post(a,b);

#define MAXGRAINS 1000
#define PD_PI 3.14159


static t_word* fofcos_table;
static t_sample* halfcos_table;
static t_sample* exp_table;

static void cos_maketable(void)
{
     int i;
     t_word *fp;
     t_sample phase, phsinc = (2. * PD_PI) / COSTABSIZE;

     if (fofcos_table) return;
     fofcos_table = (t_word *)getbytes(sizeof(t_word) * (COSTABSIZE+1));

     for (i = COSTABSIZE + 1, fp = fofcos_table, phase = 0; i--;
	  fp++, phase += phsinc)
	  fp->w_float = cos(phase);

}

static void halfcos_maketable(void)
{
     int i;
     t_sample *fp, phase, phsinc = (PD_PI) / COSTABSIZE;

     if (halfcos_table) return;
     halfcos_table = (t_sample *)getbytes(sizeof(t_sample) * (COSTABSIZE+1));

     for (i = COSTABSIZE + 1, fp = halfcos_table, phase = PD_PI; i--;
	  fp++, phase += phsinc)
	  *fp = 0.5*(cos(phase) + 1.0);
}


static void exp_maketable(void)
{
     int i;
     t_sample *fp, phase, phsinc = (2 * PD_PI) / COSTABSIZE;

     if (exp_table) return;
     exp_table = (t_sample *)getbytes(sizeof(t_sample) * (COSTABSIZE+1));

     for (i = COSTABSIZE + 1, fp = exp_table, phase = 0; i--;
	  fp++, phase += phsinc)
	  *fp = exp(-phase);
}


static t_class *fofsynth_class;

typedef struct _grain
{
     struct _grain *next;
     t_float formph; /* 0 ... 1 */
     t_float formphinc;
     t_float envph;
     int falling;
} t_grain;


typedef struct _fofsynth
{
     t_object x_obj;

     /* it is possible to use 2 array, prob change this one
	int the future */

     t_symbol* x_arrayname;

     /* template */

     int x_npoints;
     t_word *x_vec;

     /* fof */
     int debug;

     int maxgrains;
     int numgrains;

     float* x_envelope;

     /* the queue of grains */

     t_grain* grainbase;
     t_grain* grainstart;
     t_grain* grainend;


     t_float fundph; /* 0 to 1; if 1 -> add a new grain */

     t_float fundfreq; /* input parameter 1 */
     t_float formfreq; /* input paramter 2 */
     t_float risedur; /* input parameter 3 ( in % of total duration )*/
     t_float falldur; /* input parameter 5 ( in % of total duration */

     /* other */
     int neednewgrain;
} t_fofsynth;




/* build a cyclic list */
static t_grain* grain_makecyclic(t_grain* base,int num)
{
     t_grain* cur = base;
     while (--num) {
	  cur->next = cur+1;
	  cur++;
     }
     cur->next = base;
	 return base;
}





static t_int *fofsynth_perform(t_int *w)
{
     t_fofsynth* x = (t_fofsynth*) (w[1]);
     t_sample *in = (t_sample *)(w[2]);
     t_sample *out = (t_sample *)(w[3]);
     int n = (int)(w[4]);

     t_float totaldur = (x->risedur+ x->falldur)*0.01/ *in;

     t_sample srate = 44100.0; /*((t_signal*)w[2])->s_sr;*/
     t_sample israte = 1.0/srate;

     t_float fundphase = x->fundph;
     t_float numperiods = totaldur*x->formfreq;
     t_float formphinc = (x->formfreq/srate);

     t_sample risinc;
     t_sample fallinc;

     t_grain* cur;

     risinc  = (x->risedur == 0.0) ? 1.0 : 1.0/ (srate*totaldur*0.01*x->risedur);
     fallinc = (x->falldur == 0.0) ? 1.0 : 1.0/ (srate*totaldur*0.01*x->falldur);

     DEBUG(" fundph %3.2f",x->fundph);
     DEBUG(" fundfreq %3.2f",x->fundfreq);
     DEBUG(" formfreq %3.2f",x->formfreq);
     DEBUG(" risedur %3.2f %",x->risedur);
     DEBUG(" falldur %3.2f %",x->falldur);
     DEBUG(" totaldur %3.2f s",totaldur);
     DEBUG(" risinc %0.6f",risinc);
     DEBUG(" fallinc %0.6f",fallinc);
     DEBUG(" formant increase %3.2f",formphinc);
     DEBUG(" numgrains %d",x->numgrains);

     while (n--)
     {
	  fundphase+=*++in*israte;
	  *out = 0.0;

	  if (x->neednewgrain) {  /* new grain, they are deleted separetely */
	       t_grain* newgrain = x->grainend;
/*	       DEBUG("new grain created",0); */
	       if (newgrain->next == x->grainstart) {
		    post("fof: grain overflow");
		    x->neednewgrain = 0;
	       }
	       else {
		    x->numgrains++;
		    x->grainend = newgrain->next;
		    newgrain->formphinc = formphinc;
		    newgrain->falling = 0;
		    newgrain->formph = newgrain->envph = 0.0;
		    x->neednewgrain = 0;
	       }
	  }

	  cur = x->grainstart;
	  while (cur != x->grainend) {
	       t_sample formphase = cur->formph;
	       t_sample envelope;

	       t_sample tph = (formphase - (t_sample)((int) formphase));
	       t_sample val = x->x_vec[(int) (tph * x->x_npoints)].w_float;

	       /* Apply the envelope */

	       if (!cur->falling && (cur->envph <= 1.0)) {
		    envelope = *(halfcos_table + (int) (cur->envph * COSTABSIZE));
		    cur->envph+=risinc;
		    val *= envelope;
	       }
	       else if (!cur->falling)
	       {
		    cur->falling = 1;
		    cur->envph = 0;
	       }


	       if (cur->falling) {
		    envelope = *(exp_table + (int) (cur->envph * COSTABSIZE));
		    cur->envph+=fallinc;
		    val *= envelope;
	       }

	       /* end of envelope code */


	       formphase+=cur->formphinc;
	       cur->formph = formphase;

	       if (formphase >= numperiods) { /* dead */
		    DEBUG("grain died",0);
		    x->grainstart = cur->next;
		    x->numgrains--;/* not needed */
	       }

	       cur = cur->next;
	       *out += val;
	  }


	  if (fundphase > 1.0) {
	       fundphase -=  1.0;
	       x->neednewgrain=1;
	  }
	  out++;
     }

     x->fundph=fundphase;
     x->debug = 0;


     return (w+5);
}

void fofsynth_usearray(t_symbol* s,int* points,t_word** vec)
{
     t_garray *a;
     if (!(a = (t_garray *)pd_findbyclass(s, garray_class)))
	  pd_error(0, "%s: no such array", s->s_name);
     else if (!garray_getfloatwords(a,points,vec))
	  pd_error(0, "%s: bad template for fof~", s->s_name);
     else
	  garray_usedindsp(a);
}

static void fofsynth_dsp(t_fofsynth *x, t_signal **sp)
{

     if (x->x_arrayname)
	  fofsynth_usearray(x->x_arrayname,&x->x_npoints, &x->x_vec);
     else {
	  x->x_npoints=COSTABSIZE;
	  x->x_vec = fofcos_table;
     }

     dsp_add(fofsynth_perform, 4, x,
	     sp[0]->s_vec,sp[1]->s_vec, sp[0]->s_n);
}


static void fofsynth_free(t_fofsynth *x)
{
     freebytes(x->grainbase,sizeof(t_grain)*x->maxgrains);
}


static void fofsynth_debug(t_fofsynth* x)
{
     x->debug = 1;
}


static void fofsynth_float(t_fofsynth* x,t_float f)
{
	x->fundfreq = f > 0.0 ? f : -f;
}


static void *fofsynth_new(t_symbol* s,t_float a,t_float b,t_float c,t_float d)
{
     int maxgrains = MAXGRAINS;
     t_fofsynth *x = (t_fofsynth *)pd_new(fofsynth_class);

     x->debug = 0;
     x->x_arrayname = s;

     if (s == &s_)
	  x->x_arrayname = NULL;

     /* setup the grain queue */

     x->grainbase = getbytes(sizeof(t_grain)*maxgrains);
     x->maxgrains = maxgrains;
     grain_makecyclic(x->grainbase,maxgrains);
     x->grainstart = x->grainbase;
     x->grainend = x->grainbase;
     x->numgrains = 0;

     /* some of them could be signals too */

     floatinlet_new(&x->x_obj, &x->formfreq);
     floatinlet_new(&x->x_obj, &x->risedur);
     floatinlet_new(&x->x_obj, &x->falldur);

     x->fundph = 0.0;
     x->fundfreq = 200.0;
     x->formfreq = 600.0;
     x->risedur = 5.0;
     x->falldur = 140.0;

     if (a) x->fundfreq = a;
     if (b) x->formfreq = b;
     if (c) x->risedur = c;
     if (d) x->falldur = d;

     outlet_new(&x->x_obj, &s_signal);
     return (x);
}

void fofsynth_tilde_setup(void)
{
     fofcos_table = NULL;
     halfcos_table = NULL;
     fofsynth_class = class_new(gensym("fof~"), (t_newmethod) fofsynth_new,(t_method) fofsynth_free,
				sizeof(t_fofsynth), 0,A_DEFSYM, A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,0);
     class_addcreator((t_newmethod)fofsynth_new,gensym("fofsynth~"),A_DEFSYM, A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,0);
     class_addmethod(fofsynth_class, nullfn, gensym("signal"), 0);
     class_addmethod(fofsynth_class, (t_method) fofsynth_dsp, gensym("dsp"), 0);
     class_addfloat(fofsynth_class, (t_method) fofsynth_float);
     class_addmethod(fofsynth_class,(t_method) fofsynth_debug, gensym("debug"),0);
     cos_maketable();
     halfcos_maketable();
     exp_maketable();
}
