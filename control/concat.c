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


void concat_float(t_concat *x,t_float f)
{
  char* nsym;
  int len = (12+strlen(x->x_sym));
  nsym = getbytes(len);

  sprintf(nsym,"%g",f);
  strcat(nsym,x->x_sym->s_name);

  outlet_symbol(x->x_obj.ob_outlet,gensym(nsym));
  freebytes(nsym,len);
}

void concat_symbol(t_concat *x,t_symbol* s)
{
  char* nsym;
  int len = (strlen(s->s_name)+strlen(x->x_sym));
  nsym = getbytes(len);

  strcpy(nsym,s->s_name);
  strcat(nsym,x->x_sym->s_name);

  outlet_symbol(x->x_obj.ob_outlet,gensym(nsym));
  freebytes(nsym,len);
}


static void *concat_new(t_symbol* s)
{
    t_concat *x = (t_concat *)pd_new(concat_class);
    outlet_new(&x->x_obj,&s_float);
    symbolinlet_new(&x->x_obj, &x->x_sym);
    x->x_sym = s;
    return (x);
}



void concat_setup(void)
{
    concat_class = class_new(gensym("concat"), (t_newmethod)concat_new, 0,
				sizeof(t_concat),0, A_DEFSYM,0);
    class_addsymbol(concat_class,concat_symbol);
    class_addfloat(concat_class,concat_float);
}


