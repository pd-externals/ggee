/* (C) Guenter Geiger <geiger@epy.co.at> */


#include <m_pd.h>
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ------------------------ prepend ----------------------------- */

static t_class *prepend_class;


typedef struct _prepend
{
     t_object x_obj;
     t_symbol* x_s;
} t_prepend;


void prepend_anything(t_prepend *x,t_symbol* s,t_int argc,t_atom* argv)
{
     int i = argc;
     t_symbol* cur;
     t_atom a_out[256];
     int    c_out = 0;
     t_atom* a = a_out;

     if (argv->a_type == A_SYMBOL) {
       SETSYMBOL(a,s);
       a++;
       c_out++;
     }

     while (i--) {
	  switch( argv->a_type) {
	  case A_FLOAT:
	       SETFLOAT(a,atom_getfloat(argv));
	       a++;
	       c_out++;
	       break;
	  case A_SYMBOL:
	       SETSYMBOL(a,atom_getsymbol(argv));
	       a++;
	       c_out++;
	       break;
	  default:
	       post("unknown type");
	  }
	  argv++;
     }
     outlet_anything(x->x_obj.ob_outlet,x->x_s,c_out,(t_atom*)&a_out);
}

static void *prepend_new(t_symbol* s)
{
    t_prepend *x = (t_prepend *)pd_new(prepend_class);
    outlet_new(&x->x_obj, &s_float);
    if (s != &s_)
	 x->x_s = s;
    else {
	 x->x_s = gensym("prepend");
	 error("prepend needs symbol argument");
    }
    return (x);
}

void prepend_setup(void)
{
    prepend_class = class_new(gensym("prepend"), (t_newmethod)prepend_new, 0,
				sizeof(t_prepend), 0,A_DEFSYM,NULL);
    class_addanything(prepend_class,prepend_anything);
}


