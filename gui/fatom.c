#include <m_pd.h>
#include "g_canvas.h"
#include <ggee.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ------------------------ fatom ----------------------------- */

#define x_val a_pos.a_w.w_float

static t_class *fatom_class;

typedef struct _fatom
{
     t_object x_obj;
     t_atom a_pos;

     t_glist * x_glist;
     int x_width;
     int x_height;
     t_symbol*  x_sym;
     t_symbol*  x_type;
} t_fatom;

/* widget helper functions */



static void draw_inlets(t_fatom *x, t_glist *glist, int firsttime, int nin, int nout)
{
     int n = nin;
     int nplus, i;
     nplus = (n == 1 ? 1 : n-1);
     for (i = 0; i < n; i++)
     {
	  int onset = text_xpix(&x->x_obj, glist) + (x->x_width - IOWIDTH) * i / nplus;
	  if (firsttime)
	       sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xo%d\n",
			glist_getcanvas(glist),
			onset, text_ypix(&x->x_obj, glist) + x->x_height - 1,
			onset + IOWIDTH, text_ypix(&x->x_obj, glist) + x->x_height,
			x, i);
	  else
	       sys_vgui(".x%x.c coords %xo%d %d %d %d %d\n",
			glist_getcanvas(glist), x, i,
			onset, text_ypix(&x->x_obj, glist) + x->x_height - 1,
			onset + IOWIDTH, text_ypix(&x->x_obj, glist) + x->x_height);
     }
     n = nout; 
     nplus = (n == 1 ? 1 : n-1);
     for (i = 0; i < n; i++)
     {
	  int onset = text_xpix(&x->x_obj, glist) + (x->x_width - IOWIDTH) * i / nplus;
	  if (firsttime)
	       sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xi%d\n",
			glist_getcanvas(glist),
			onset, text_ypix(&x->x_obj, glist),
			     onset + IOWIDTH, text_ypix(&x->x_obj, glist) + 1,
			x, i);
	  else
	       sys_vgui(".x%x.c coords %xi%d %d %d %d %d\n",
			glist_getcanvas(glist), x, i,
			onset, text_ypix(&x->x_obj, glist),
			onset + IOWIDTH, text_ypix(&x->x_obj, glist) + 1);
	  
     }
}


void fatom_drawme(t_fatom *x, t_glist *glist, int firsttime)
{
     if (firsttime) {

	  if (!strcmp(x->x_type->s_name,"vslider")) {
	       x->x_width = 32;
	       x->x_height = 140;

	       sys_vgui("scale .x%x.c.s%x \
                    -sliderlength 10 \
                    -showvalue 0 \
                    -length 131 \
                    -from 127 -to 0 \
                    -command fatom_cb%x\n",glist_getcanvas(glist),x,x);
	       sys_vgui(".x%x.c create window %d %d -anchor nw -window .x%x.c.s%x -tags %xS\n", 
			glist_getcanvas(glist),text_xpix(&x->x_obj, glist), text_ypix(&x->x_obj, glist)+2,glist_getcanvas(glist),x,x);
	  } else  if (!strcmp(x->x_type->s_name,"hslider")) {
	       x->x_width = 150;
	       x->x_height = 28;
	       sys_vgui("scale .x%x.c.s%x \
                    -sliderlength 10 \
                    -showvalue 0 \
                    -length 131 \
                    -orient horizontal \
                    -from 127 -to 0 \
                    -command fatom_cb%x\n",glist_getcanvas(glist),x,x);
	       sys_vgui(".x%x.c create window %d %d -anchor nw -window .x%x.c.s%x -tags %xS\n", 
			glist_getcanvas(glist),text_xpix(&x->x_obj, glist), text_ypix(&x->x_obj, glist)+2,glist_getcanvas(glist),x,x);
	  } else if (!strcmp(x->x_type->s_name,"checkbutton")) {
	       x->x_width = 40;
	       x->x_height = 25;
	       sys_vgui("checkbutton .x%x.c.s%x \
                    -command { fatom_cb%x $fatom_val%x} -variable fatom_val%x\n",glist_getcanvas(glist),x,x,x,x);
	       sys_vgui(".x%x.c create window %d %d -anchor nw -window .x%x.c.s%x -tags %xS\n", 
			x->x_glist,text_xpix(&x->x_obj, glist), text_ypix(&x->x_obj, glist)+2,glist_getcanvas(glist),x,x);
	  } else if (!strcmp(x->x_type->s_name,"hradio")) {
	       int i;
	       x->x_width = 8*20;
	       x->x_height = 25;
	       for (i=0;i<8;i++) {
		    sys_vgui("radiobutton .x%x.c.s%x%d \
                    -command { fatom_cb%x $fatom_val%x} -variable fatom_val%x -value %d\n",glist_getcanvas(glist),x,i,x,x,x,i);
		    sys_vgui(".x%x.c create window %d %d -anchor nw -window .x%x.c.s%x%d -tags %x%xS\n", 
			     x->x_glist,text_xpix(&x->x_obj, glist)+i*20, text_ypix(&x->x_obj, glist)+2,x->x_glist,x,i,x,i);
	       }
	  } else if (!strcmp(x->x_type->s_name,"vradio")) {
	       int i;
	       x->x_width = 30;
	       x->x_height = 20*8+5;
	       for (i=0;i<8;i++) {
		    sys_vgui("radiobutton .x%x.c.s%x%d \
                    -command { fatom_cb%x $fatom_val%x} -variable fatom_val%x -value %d\n",glist_getcanvas(glist),x,i,x,x,x,i);
		    sys_vgui(".x%x.c create window %d %d -anchor nw -window .x%x.c.s%x%d -tags %x%xS\n", 
			     x->x_glist,text_xpix(&x->x_obj, glist), text_ypix(&x->x_obj, glist)+2+i*20,x->x_glist,x,i,x,i);
	       }
	  } else {
	       x->x_width = 32;
	       x->x_height = 140;
	       sys_vgui("scale .x%x.c.s%x \
                    -sliderlength 10 \
                    -showvalue 0 \
                    -length 131 \
                    -from 127 -to 0 \
                    -command fatom_cb%x\n",glist_getcanvas(glist),x,x);
	       sys_vgui(".x%x.c create window %d %d -anchor nw -window .x%x.c.s%x -tags %xS\n", 
			glist_getcanvas(glist),text_xpix(&x->x_obj, glist), text_ypix(&x->x_obj, glist)+2,glist_getcanvas(glist),x,x);
	  }
	       


     }     
     else {
	  if (!strcmp(x->x_type->s_name,"hradio")) {
	       int i;
	       for (i=0;i<8;i++) {
		    sys_vgui(".x%x.c coords %x%xS \
%d %d\n",
			     glist_getcanvas(glist), x,i,
			     text_xpix(&x->x_obj, glist) + 20*i, text_ypix(&x->x_obj, glist)+2);
	       }
	  }
	  else if (!strcmp(x->x_type->s_name,"vradio")) {
	       int i;
	       for (i=0;i<8;i++) {
		    sys_vgui(".x%x.c coords %x%xS \
%d %d\n",
			     glist_getcanvas(glist), x,i,
			     text_xpix(&x->x_obj, glist), text_ypix(&x->x_obj, glist)+2+20*i);
	       }
	       
	  } else {
	       sys_vgui(".x%x.c coords %xS \
%d %d\n",
			glist_getcanvas(glist), x,
			text_xpix(&x->x_obj, glist), text_ypix(&x->x_obj, glist)+2);
	  }
     }

     draw_inlets(x, glist, firsttime, 1,1);
}


void fatom_erase(t_fatom* x,t_glist* glist)
{
     int n;

//     sys_vgui(".x%x.c.s%x delete\n",glist_getcanvas(glist),x);

     sys_vgui(".x%x.c delete %xS\n",glist_getcanvas(glist), x);

     /* inlets and outlets */
     
     sys_vgui(".x%x.c delete %xi%d\n",x->x_glist,x,0);
     sys_vgui(".x%x.c delete %xo%d\n",x->x_glist,x,0);
}
	


/* ------------------------ fatom widgetbehaviour----------------------------- */


static void fatom_getrect(t_gobj *z, t_glist *owner,
    int *xp1, int *yp1, int *xp2, int *yp2)
{
    int width, height;
    t_fatom* s = (t_fatom*)z;


    width = s->x_width;
    height = s->x_height;
    *xp1 = s->x_obj.te_xpix;
    *yp1 = s->x_obj.te_ypix;
    *xp2 = s->x_obj.te_xpix + width;
    *yp2 = s->x_obj.te_ypix + height;
}

static void fatom_displace(t_gobj *z, t_glist *glist,
    int dx, int dy)
{
    t_fatom *x = (t_fatom *)z;
    x->x_obj.te_xpix += dx;
    x->x_obj.te_ypix += dy;
    sys_vgui(".x%x.c coords %xSEL %d %d %d %d\n",
		   glist_getcanvas(glist), x,
		   text_xpix(&x->x_obj, glist), text_ypix(&x->x_obj, glist),
       	           text_xpix(&x->x_obj, glist) + x->x_width, text_ypix(&x->x_obj, glist) + x->x_height);

    fatom_drawme(x, glist, 0);
    canvas_fixlinesfor(glist_getcanvas(glist),(t_text*) x);
}

static void fatom_select(t_gobj *z, t_glist *glist, int state)
{
     t_fatom *x = (t_fatom *)z;
     if (state) {
	  sys_vgui(".x%x.c create rectangle \
%d %d %d %d -tags %xSEL -outline blue\n",
		   glist_getcanvas(glist),
		   text_xpix(&x->x_obj, glist), text_ypix(&x->x_obj, glist),
		   text_xpix(&x->x_obj, glist) + x->x_width, text_ypix(&x->x_obj, glist) + x->x_height,
		   x);
     }
     else {
	  sys_vgui(".x%x.c delete %xSEL\n",
		   glist_getcanvas(glist), x);
     }



}


static void fatom_activate(t_gobj *z, t_glist *glist, int state)
{
/*    t_text *x = (t_text *)z;
    t_rtext *y = glist_findrtext(glist, x);
    if (z->g_pd != gatom_class) rtext_activate(y, state);*/
}

static void fatom_delete(t_gobj *z, t_glist *glist)
{
    t_text *x = (t_text *)z;
    canvas_deletelinesfor(glist_getcanvas(glist), x);
}

       
static void fatom_vis(t_gobj *z, t_glist *glist, int vis)
{
    t_fatom* s = (t_fatom*)z;
    if (vis)
	 fatom_drawme(s, glist, 1);
    else
	 fatom_erase(s,glist);
}

/* can we use the normal text save function ?? */

static void fatom_save(t_gobj *z, t_binbuf *b)
{

    t_fatom *x = (t_fatom *)z;

    binbuf_addv(b, "ssiiss", gensym("#X"),gensym("obj"),
		x->x_obj.te_xpix, x->x_obj.te_ypix ,  
		gensym("fatom"),x->x_type);
    binbuf_addv(b, ";");
}


t_widgetbehavior   fatom_widgetbehavior;

void fatom_size(t_fatom* x,t_floatarg w,t_floatarg h) {
     x->x_width = w;
     x->x_height = h;
}

void fatom_color(t_fatom* x,t_symbol* col)
{
/*     outlet_bang(x->x_obj.ob_outlet); only bang if there was a bang .. 
       so color black does the same as bang, but doesn't forward the bang 
*/
}

static void fatom_setwidget()
{
    fatom_widgetbehavior.w_getrectfn =     fatom_getrect;
    fatom_widgetbehavior.w_displacefn =    fatom_displace;
    fatom_widgetbehavior.w_selectfn =   fatom_select;
    fatom_widgetbehavior.w_activatefn =   fatom_activate;
    fatom_widgetbehavior.w_deletefn =   fatom_delete;
    fatom_widgetbehavior.w_visfn =   fatom_vis;
#if (PD_VERSION_MINOR > 31) 
    fatom_widgetbehavior.w_clickfn = NULL;
    fatom_widgetbehavior.w_propertiesfn = NULL; 
#endif
    fatom_widgetbehavior.w_savefn =   fatom_save;
}



static void fatom_float(t_fatom* x,t_floatarg f) 
{
     x->x_val = f;
     sys_vgui(".x%x.c.s%x set %f\n",x->x_glist,x,f);
     outlet_float(x->x_obj.ob_outlet,f);
}

static void fatom_f(t_fatom* x,t_floatarg f) 
{
     x->x_val = f;
     outlet_float(x->x_obj.ob_outlet,f);
}

static void *fatom_new(t_symbol* type)
{
    t_fatom *x = (t_fatom *)pd_new(fatom_class);
    char buf[256];

    x->x_type = type;

    x->x_glist = (t_glist*) canvas_getcurrent();
/*
    if (h) x->x_width = h;
    else
*/
/*
    if (o) x->x_height = o;
    else
*/

    /* bind to a symbol for fatom callback (later make this based on the
       filepath ??) */

    sprintf(buf,"fatom%x",x);
    x->x_sym = gensym(buf);
    pd_bind(&x->x_obj.ob_pd, x->x_sym);

/* pipe startup code to tk */

    sys_vgui("proc fatom_cb%x {val} {\n
       pd [concat fatom%x f $val \\;]\n
       }\n",x,x);

    outlet_new(&x->x_obj, &s_float);
    return (x);
}

void fatom_setup(void)
{
    fatom_class = class_new(gensym("fatom"), (t_newmethod)fatom_new, 0,
				sizeof(t_fatom),0,A_DEFSYM,0);

    class_addfloat(fatom_class, (t_method)fatom_float);
    class_addmethod(fatom_class, (t_method)fatom_f, gensym("f"),
    	A_FLOAT, 0);

/*
    class_addmethod(fatom_class, (t_method)fatom_size, gensym("size"),
    	A_FLOAT, A_FLOAT, 0);

    class_addmethod(fatom_class, (t_method)fatom_color, gensym("color"),
    	A_SYMBOL, 0);
*/
/*
    class_addmethod(fatom_class, (t_method)fatom_open, gensym("open"),
    	A_SYMBOL, 0);
*/
    fatom_setwidget();
    class_setwidget(fatom_class,&fatom_widgetbehavior);

}


