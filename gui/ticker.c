#include <m_pd.h>
#include "g_canvas.h"

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include <stdio.h>
#include "fatom.h"

/* can we use the normal text save function ?? */

static t_class *ticker_class;

static void ticker_save(t_gobj *z, t_binbuf *b)
{

    t_fatom *x = (t_fatom *)z;

    binbuf_addv(b, "ssiiss", gensym("#X"),gensym("obj"),
		x->x_obj.te_xpix, x->x_obj.te_ypix ,  
		gensym("ticker"),x->x_text);
    binbuf_addv(b, ";");
}

static void ticker_bang(t_fatom* x)
{
  x->x_val = !x->x_val;
  fatom_float(x,x->x_val);
}

static void *ticker_new(t_symbol* t)
{
    t_fatom *x = (t_fatom *)pd_new(ticker_class);
    x->x_type = gensym("checkbutton");
    return fatom_new(x,10,0,0,t);
}


t_widgetbehavior   ticker_widgetbehavior = {
  w_getrectfn:  fatom_getrect,
  w_displacefn: fatom_displace,
  w_selectfn:   fatom_select,
  w_activatefn: fatom_activate,
  w_deletefn:   fatom_delete,
  w_visfn:      fatom_vis,
#if PD_MINOR_VERSION < 37
  w_savefn:     ticker_save,
  w_propertiesfn: NULL,
#endif
  w_clickfn:    NULL,
}; 

void ticker_setup() {
    ticker_class = class_new(gensym("ticker"), (t_newmethod)ticker_new, 0,
				sizeof(t_fatom),0,A_DEFSYMBOL,0);

    class_addbang(ticker_class,ticker_bang);
  fatom_setup_common(ticker_class);
    class_addbang(ticker_class, (t_method)ticker_bang);
    class_setwidget(ticker_class,&ticker_widgetbehavior);
#if PD_MINOR_VERSION >= 37
    class_setsavefn(ticker_class,&ticker_save);
#endif
}

