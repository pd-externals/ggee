/* (C) Guenter Geiger <geiger@epy.co.at> */


#include "math.h"
#include <m_pd.h>

/* ----------------------------- apply ----------------------------- */
static t_class *apply_class;


typedef double (*t_apply_onearg)(double);
typedef double (*t_apply_twoarg)(double);

static double nop(double f) {
  return f;
}

typedef struct _funlist {
  char* name;
  t_int* fun;
  int numarg;
  char* desc;
} t_funlist;

#define MFUN1(x,d) {#x,(t_int*)x,1,d}

static t_funlist funlist[] = 
  {
  MFUN1(nop,"does nothing"),
  MFUN1(sin,"calculate sine"),
  MFUN1(asin,"calculate arcus sine"),
  MFUN1(cos,"calculate cosine"),
  MFUN1(acos,"calculate arcus cosine"),
  MFUN1(tan,"calculate tangent"),
  MFUN1(atan,"calculate arcus tangent"),

  MFUN1(sinh,""),
  MFUN1(asinh,""),
  MFUN1(cosh,""),
  MFUN1(acosh,""),
  MFUN1(tanh,""),
  MFUN1(atanh,""),

  MFUN1(exp,""),
  MFUN1(expm1,""),
  //  MFUN1(exp10),
  //  MFUN1(pow10),
  MFUN1(log,""),
  MFUN1(log1p,""),
  MFUN1(log10,""),
  //  MFUN1(log2),
  MFUN1(sqrt,""),
  MFUN1(cbrt,""),


  MFUN1(rint,""),
  //  MFUN1(round),
  MFUN1(ceil,""),
  MFUN1(floor,""),
  //  MFUN1(trunc),

  MFUN1(erf,""),
  MFUN1(erfc,""),
  MFUN1(gamma,""),
  MFUN1(lgamma,""),
  //  MFUN1(tgamma),

  MFUN1(j0,""),
  MFUN1(j1,""),
  MFUN1(y0,""),
  MFUN1(j1,"")
  };



typedef struct _apply
{
    t_object x_obj;
  t_int* x_fun;
} t_apply;

static void *apply_new(t_symbol *s, int argc, t_atom *argv)
{
  t_symbol* fname;
  int i;
  int numfun = sizeof(funlist)/sizeof(t_funlist);
  t_apply *x = (t_apply *)pd_new(apply_class);
  outlet_new(&x->x_obj, &s_signal);

  x->x_fun = (t_int*)nop;
  if (argc < 1) {
    post("nop operation requested");
  }
  else {
    if (argv[0].a_type != A_SYMBOL)
      goto nofun;
    fname = atom_getsymbol(argv);
    for (i=0;i<numfun;i++)
      if (!strcmp(fname->s_name,funlist[i].name))
	x->x_fun = funlist[i].fun;



  }
  

  return (x);
  nofun:
      post("apply first argument has to be a function");
      return (x);
}


static void apply_what(t_apply* x)
{
  int i;
  int numfun = sizeof(funlist)/sizeof(t_funlist);
  for (i=0;i<numfun;i++)
    post("function: %s:  %s",funlist[i].name,funlist[i].desc);
}

t_int *apply_perform(t_int *w)
{
  t_apply* x = (t_apply*)(w[1]);
    t_float *in1 = (t_float *)(w[2]);
    t_float *out = (t_float *)(w[3]);

    int n = (int)(w[4]);
    while (n--) *out++ = (t_float) ((t_apply_onearg)x->x_fun)(*in1++); 
    return (w+5);
}


void dsp_add_apply(t_apply* x,t_sample *in1, t_sample *out, int n)
{
    	dsp_add(apply_perform, 4,x, in1, out, n);
}

static void apply_dsp(t_apply *x, t_signal **sp)
{
    dsp_add_apply(x,sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

void apply_tilde_setup(void)
{
    apply_class = class_new(gensym("apply~"), (t_newmethod)apply_new, 0,
    	sizeof(t_apply), 0, A_GIMME, 0);
    class_addmethod(apply_class, nullfn, gensym("signal"), 0);
    class_addmethod(apply_class, (t_method)apply_dsp, gensym("dsp"), 0);
    class_addmethod(apply_class, (t_method)apply_what, gensym("what"), 0);
}
