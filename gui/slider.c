#include <m_pd.h>
#include "g_canvas.h"

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include <stdio.h>
#include "fatom.h"

/* can we use the normal text save function ?? */

static t_class *slider_class;

static void slider_save(t_gobj *z, t_binbuf *b)
{

    t_fatom *x = (t_fatom *)z;

    binbuf_addv(b, "ssiisiii", gensym("#X"),gensym("obj"),
		x->x_obj.te_xpix, x->x_obj.te_ypix ,  
		gensym("slider"),x->x_max,x->x_min,x->x_width);
    binbuf_addv(b, ";");
}


static void *slider_new(t_floatarg max, t_floatarg min, t_floatarg h)
{
    t_fatom *x = (t_fatom *)pd_new(slider_class);
    x->x_type = gensym("vslider");
    return fatom_new(x,max,min,h,&s_);
}


t_widgetbehavior   slider_widgetbehavior = {
  w_getrectfn:  fatom_getrect,
  w_displacefn: fatom_displace,
  w_selectfn:   fatom_select,
  w_activatefn: fatom_activate,
  w_deletefn:   fatom_delete,
  w_visfn:      fatom_vis,
#if PD_MINOR_VERSION < 37
  w_savefn:     slider_save,
  w_propertiesfn: NULL,
#endif
  w_clickfn:    NULL,
}; 

void slider_setup() {
    slider_class = class_new(gensym("slider"), (t_newmethod)slider_new, 0,
				sizeof(t_fatom),0,A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,0);

  fatom_setup_common(slider_class);
    class_setwidget(slider_class,&slider_widgetbehavior);
#if PD_MINOR_VERSION >= 37
    class_setsavefn(slider_class,&slider_save);
#endif
    class_setpropertiesfn(slider_class,&fatom_properties);
}
