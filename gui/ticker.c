#include <m_pd.h>
#include "g_canvas.h"
#include <ggee.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include "fatom.h"

/* can we use the normal text save function ?? */

static t_class *ticker_class;

static void ticker_save(t_gobj *z, t_binbuf *b)
{

    t_fatom *x = (t_fatom *)z;

    binbuf_addv(b, "ssiis", gensym("#X"),gensym("obj"),
		x->x_obj.te_xpix, x->x_obj.te_ypix ,  
		gensym("ticker"));
    binbuf_addv(b, ";");
}

static void ticker_bang(t_fatom* x)
{
  x->x_val = !x->x_val;
  fatom_float(x,x->x_val);
}

static void *ticker_new()
{
    t_fatom *x = (t_fatom *)pd_new(ticker_class);
    char buf[256];

    x->x_type = gensym("checkbutton");
    x->x_glist = (t_glist*)NULL;
/*
    if (h) x->x_width = h;
    else
*/
/*
    if (o) x->x_height = o;
    else
*/

    /* bind to a symbol for ticker callback (later make this based on the
       filepath ??) */

    sprintf(buf,"ticker%x",x);
    x->x_sym = gensym(buf);
    pd_bind(&x->x_obj.ob_pd, x->x_sym);

/* pipe startup code to slitk */

    sys_vgui("proc fatom_cb%x {val} {\n
       pd [concat ticker%x f $val \\;]\n
       }\n",x,x);

    outlet_new(&x->x_obj, &s_float);
    return (x);
}


t_widgetbehavior   ticker_widgetbehavior = {
  w_getrectfn:  fatom_getrect,
  w_displacefn: fatom_displace,
  w_selectfn:   fatom_select,
  w_activatefn: fatom_activate,
  w_deletefn:   fatom_delete,
  w_visfn:      fatom_vis,
  w_savefn:     ticker_save,
  w_clickfn:    NULL,
  w_propertiesfn: NULL,
}; 

void ticker_setup() {
    ticker_class = class_new(gensym("ticker"), (t_newmethod)ticker_new, 0,
				sizeof(t_fatom),0,0);

    class_addbang(ticker_class,ticker_bang);
  fatom_setup_common(ticker_class);
    class_setwidget(ticker_class,&ticker_widgetbehavior);
}
