/* (C) Guenter Geiger <geiger@epy.co.at> */


/* ------------------------ slider ----------------------------- */

#include "slider.h"

static t_class *slider_class;


static void *slider_new(t_floatarg h,t_floatarg o,t_floatarg w,t_floatarg n,t_floatarg horiz)
{
    t_slider *x = (t_slider *)pd_new(slider_class);
    x->x_glist = (t_glist*) canvas_getcurrent();
    
    if (w) x->x_width = w;
    else x->x_width = 15;
    if (h) x->x_height = h - o;
    else
	 x->x_height = 127;
    x->x_offset = o;
    x->x_pos = o;
    x->x_pos2 = o;
    x->a_pos.a_type = A_FLOAT;
    if (n) x->x_num = n;
    else x->x_num = 1;
    outlet_new(&x->x_obj, &s_float);

/* make us think we're an atom .. we are one ... this doesn work, 
 because  the object resets it .. so we will have to create our own menu entry later */
/*    x->x_obj.te_type = T_ATOM; */
    x->x_horizontal = horiz;

    return (x);
}



void slider_setup(void)
{
    slider_class = class_new(gensym("slider"), (t_newmethod)slider_new, 0,
				sizeof(t_slider), 0,A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,A_DEFFLOAT,0);
    class_addbang(slider_class,slider_bang);
    class_addfloat(slider_class,slider_float);

    class_addmethod(slider_class, (t_method)slider_mark, gensym("mark"),
    	A_FLOAT, 0);


    class_addmethod(slider_class, (t_method)slider_click, gensym("click"),
    	A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(slider_class, (t_method)slider_motion, gensym("motion"),
    	A_FLOAT, A_FLOAT, 0);
    class_addmethod(slider_class, (t_method)slider_set, gensym("set"),
A_FLOAT, 0);
    class_addmethod(slider_class, (t_method)slider_type, gensym("type"),A_FLOAT,0);

    
    slider_setwidget();
    class_setwidget(slider_class,&slider_widgetbehavior);
}

