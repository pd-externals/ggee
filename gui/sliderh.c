#include <m_pd.h>
#include "g_canvas.h"
#include <ggee.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include "fatom.h"

/* can we use the normal text save function ?? */

static t_class *sliderh_class;

static void sliderh_save(t_gobj *z, t_binbuf *b)
{

    t_fatom *x = (t_fatom *)z;

    binbuf_addv(b, "ssiisiii", gensym("#X"),gensym("obj"),
		x->x_obj.te_xpix, x->x_obj.te_ypix ,  
		gensym("sliderh"),x->x_max,x->x_min,x->x_width);
    binbuf_addv(b, ";");
}


static void *sliderh_new(t_floatarg max, t_floatarg min,t_floatarg h)
{
    t_fatom *x = (t_fatom *)pd_new(sliderh_class);
    x->x_type = gensym("hslider");
    return fatom_new(x,max,min,h);
}


t_widgetbehavior   sliderh_widgetbehavior = {
  w_getrectfn:  fatom_getrect,
  w_displacefn: fatom_displace,
  w_selectfn:   fatom_select,
  w_activatefn: fatom_activate,
  w_deletefn:   fatom_delete,
  w_visfn:      fatom_vis,
  w_savefn:    sliderh_save,
  w_clickfn:    NULL,
  w_propertiesfn: NULL,
}; 


void sliderh_setup() {
    sliderh_class = class_new(gensym("sliderh"), (t_newmethod)sliderh_new, 0,
				sizeof(t_fatom),0,A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,0);

    fatom_setup_common(sliderh_class);
    class_setwidget(sliderh_class,&sliderh_widgetbehavior);
}
