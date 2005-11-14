/* (C) Guenter Geiger <geiger@epy.co.at> */


#include <m_pd.h>
#include <string.h>

/* ------------------------ concat ----------------------------- */

#define MAX_ELEMENTS 256

static t_class *concat_class;


typedef struct _concat
{
     t_object x_obj;
     t_symbol* x_sym;
} t_concat;

static char tsym[2048];

void concat_float(t_concat *x,t_float f)
{
  sprintf(tsym,"%g",f);
  strcat(tsym,x->x_sym->s_name);

  outlet_symbol(x->x_obj.ob_outlet,gensym(tsym));
}

void concat_symbol(t_concat *x,t_symbol* s)
{
  strcpy(tsym,s->s_name);
  strcat(tsym,x->x_sym->s_name);

  outlet_symbol(x->x_obj.ob_outlet,gensym(tsym));
}


static void *concat_new(t_symbol* s)
{
    t_concat *x = (t_concat *)pd_new(concat_class);
    outlet_new(&x->x_obj,&s_float);
    symbolinlet_new(&x->x_obj, &x->x_sym);
    x->x_sym = s;
    *tsym = 0;
    return (x);
}



void concat_setup(void)
{
    concat_class = class_new(gensym("concat"), (t_newmethod)concat_new, 0,
				sizeof(t_concat),0, A_DEFSYM,0);
    class_addsymbol(concat_class,concat_symbol);
    class_addfloat(concat_class,concat_float);
}


