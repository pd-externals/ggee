/* (C) Guenter Geiger <geiger@xdv.org> */


#include <m_pd.h>
#include "g_canvas.h"

/* ------------------------ gcanvas ----------------------------- */


#define BACKGROUNDCOLOR "grey"

#define DEFAULTSIZE 80

static t_class *gcanvas_class;

typedef struct _gcanvas
{
     t_object x_obj;
     t_glist * x_glist;
     t_outlet* out2;
     int x_width;
     int x_height;
     int x;
     int y;
} t_gcanvas;

/* widget helper functions */

void gcanvas_drawme(t_gcanvas *x, t_glist *glist, int firsttime)
{
     if (firsttime) {
	  sys_vgui(".x%x.c create rectangle \
%d %d %d %d -tags %xS -fill %s\n",
		   glist_getcanvas(glist),
		   x->x_obj.te_xpix, x->x_obj.te_ypix,
		   x->x_obj.te_xpix + x->x_width, x->x_obj.te_ypix + x->x_height,
		   x,BACKGROUNDCOLOR);
     }     
     else {
	  sys_vgui(".x%x.c coords %xS \
%d %d %d %d\n",
		   glist_getcanvas(glist), x,
		   x->x_obj.te_xpix, x->x_obj.te_ypix,
		   x->x_obj.te_xpix + x->x_width, x->x_obj.te_ypix + x->x_height);
     }

     {
       /* outlets */
	  int n = 2;
	  int nplus, i;
	  nplus = (n == 1 ? 1 : n-1);
	  for (i = 0; i < n; i++)
	  {
	       int onset = x->x_obj.te_xpix + (x->x_width - IOWIDTH) * i / nplus;
	       if (firsttime)
		    sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xo%d\n",
			     glist_getcanvas(glist),
			     onset, x->x_obj.te_ypix + x->x_height - 1,
			     onset + IOWIDTH, x->x_obj.te_ypix + x->x_height,
			     x, i);
	       else
		    sys_vgui(".x%x.c coords %xo%d %d %d %d %d\n",
			     glist_getcanvas(glist), x, i,
			     onset, x->x_obj.te_ypix + x->x_height - 1,
			     onset + IOWIDTH, x->x_obj.te_ypix + x->x_height);
	  }
	  /* inlets */
	  n = 0; 
	  nplus = (n == 1 ? 1 : n-1);
	  for (i = 0; i < n; i++)
	  {
	       int onset = x->x_obj.te_xpix + (x->x_width - IOWIDTH) * i / nplus;
	       if (firsttime)
		    sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xi%d\n",
			     glist_getcanvas(glist),
			     onset, x->x_obj.te_ypix,
			     onset + IOWIDTH, x->x_obj.te_ypix + 1,
			     x, i);
	       else
		    sys_vgui(".x%x.c coords %xi%d %d %d %d %d\n",
			     glist_getcanvas(glist), x, i,
			     onset, x->x_obj.te_ypix,
			     onset + IOWIDTH, x->x_obj.te_ypix + 1);
	       
	  }
     }

}




void gcanvas_erase(t_gcanvas* x,t_glist* glist)
{
     int n;
     sys_vgui(".x%x.c delete %xS\n",
	      glist_getcanvas(glist), x);
     n = 2;
     while (n--) {
	  sys_vgui(".x%x.c delete %xo%d\n",glist_getcanvas(glist),x,n);
     }
}
	


/* ------------------------ gcanvas widgetbehaviour----------------------------- */


static void gcanvas_getrect(t_gobj *z, t_glist *owner,
    int *xp1, int *yp1, int *xp2, int *yp2)
{
    int width, height;
    t_gcanvas* s = (t_gcanvas*)z;


    width = s->x_width;
    height = s->x_height;
    *xp1 = s->x_obj.te_xpix;
    *yp1 = s->x_obj.te_ypix;
    *xp2 = s->x_obj.te_xpix + width;
    *yp2 = s->x_obj.te_ypix + height;
}

static void gcanvas_displace(t_gobj *z, t_glist *glist,
    int dx, int dy)
{
    t_gcanvas *x = (t_gcanvas *)z;
    x->x_obj.te_xpix += dx;
    x->x_obj.te_ypix += dy;
    gcanvas_drawme(x, glist, 0);
    canvas_fixlinesfor(glist_getcanvas(glist),(t_text*) x);
}

static void gcanvas_select(t_gobj *z, t_glist *glist, int state)
{
     t_gcanvas *x = (t_gcanvas *)z;
     sys_vgui(".x%x.c itemconfigure %xS -fill %s\n", glist, 
	     x, (state? "blue" : BACKGROUNDCOLOR));
}


static void gcanvas_activate(t_gobj *z, t_glist *glist, int state)
{
/*    t_text *x = (t_text *)z;
    t_rtext *y = glist_findrtext(glist, x);
    if (z->g_pd != gatom_class) rtext_activate(y, state);*/
}

static void gcanvas_delete(t_gobj *z, t_glist *glist)
{
    t_text *x = (t_text *)z;
    canvas_deletelinesfor(glist_getcanvas(glist), x);
}

       
static void gcanvas_vis(t_gobj *z, t_glist *glist, int vis)
{
    t_gcanvas* s = (t_gcanvas*)z;
    if (vis)
	 gcanvas_drawme(s, glist, 1);
    else
	 gcanvas_erase(s,glist);
}

/* can we use the normal text save function ?? */

static void gcanvas_save(t_gobj *z, t_binbuf *b)
{
    t_gcanvas *x = (t_gcanvas *)z;
    binbuf_addv(b, "ssiisii", gensym("#X"),gensym("obj"),
		(t_int)x->x_obj.te_xpix, (t_int)x->x_obj.te_ypix,  
		gensym("gcanvas"),x->x_width,x->x_height);
    binbuf_addv(b, ";");
}


t_widgetbehavior   gcanvas_widgetbehavior;

static void gcanvas_motion(t_gcanvas *x, t_floatarg dx, t_floatarg dy)
{
  x->x += dx;
  x->y += dy;
  outlet_float(x->out2,x->y);
  outlet_float(x->x_obj.ob_outlet,x->x);
}

void gcanvas_key(t_gcanvas *x, t_floatarg f)
{
  post("key");
}


static void gcanvas_click(t_gcanvas *x,
    t_floatarg xpos, t_floatarg ypos, t_floatarg shift, t_floatarg ctrl,
    t_floatarg alt)
{
    glist_grab(x->x_glist, &x->x_obj.te_g, (t_glistmotionfn) gcanvas_motion,
		(t_glistkeyfn) NULL, xpos, ypos);

    x->x = xpos - x->x_obj.te_xpix;
    x->y = ypos - x->x_obj.te_ypix;
    outlet_float(x->out2,x->y);
    outlet_float(x->x_obj.ob_outlet,x->x);
}

static int gcanvas_newclick(t_gobj *z, struct _glist *glist,
    int xpix, int ypix, int shift, int alt, int dbl, int doit)
{
    	if (doit)
	    gcanvas_click((t_gcanvas *)z, (t_floatarg)xpix, (t_floatarg)ypix,
	    	(t_floatarg)shift, 0, (t_floatarg)alt);
	return (1);
}

void gcanvas_size(t_gcanvas* x,t_floatarg w,t_floatarg h) {
     x->x_width = w;
     x->x_height = h;
     gcanvas_drawme(x, x->x_glist, 0);
}

static void gcanvas_setwidget(void)
{
    gcanvas_widgetbehavior.w_getrectfn =     gcanvas_getrect;
    gcanvas_widgetbehavior.w_displacefn =    gcanvas_displace;
    gcanvas_widgetbehavior.w_selectfn =   gcanvas_select;
    gcanvas_widgetbehavior.w_activatefn =   gcanvas_activate;
    gcanvas_widgetbehavior.w_deletefn =   gcanvas_delete;
    gcanvas_widgetbehavior.w_visfn =   gcanvas_vis;
    gcanvas_widgetbehavior.w_clickfn = gcanvas_newclick;
#if PD_MINOR_VERSION < 37
    gcanvas_widgetbehavior.w_propertiesfn = NULL; 
    gcanvas_widgetbehavior.w_savefn =   gcanvas_save;
#endif
}


static void *gcanvas_new(t_floatarg h,t_floatarg o)
{
    t_gcanvas *x = (t_gcanvas *)pd_new(gcanvas_class);

    x->x_glist = (t_glist*) canvas_getcurrent();
    if (h) x->x_width = h;
    else
	 x->x_width = DEFAULTSIZE;

    if (o) x->x_height = o;
    else
	 x->x_height = DEFAULTSIZE;

    outlet_new(&x->x_obj, &s_float);
    x->out2 = outlet_new(&x->x_obj, &s_float);
    return (x);
}

void gcanvas_setup(void)
{
    gcanvas_class = class_new(gensym("gcanvas"), (t_newmethod)gcanvas_new, 0,
				sizeof(t_gcanvas),0, A_DEFFLOAT,A_DEFFLOAT,0);

    class_addcreator((t_newmethod)gcanvas_new,gensym("bng"),A_DEFSYM,A_DEFFLOAT,A_DEFFLOAT,A_GIMME,0);

    class_addmethod(gcanvas_class, (t_method)gcanvas_click, gensym("click"),
    	A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(gcanvas_class, (t_method)gcanvas_size, gensym("size"),
    	A_FLOAT, A_FLOAT, 0);

    gcanvas_setwidget();
    class_setwidget(gcanvas_class,&gcanvas_widgetbehavior);
#if PD_MINOR_VERSION >= 37
    class_setsavefn(gcanvas_class,&gcanvas_save);
#endif
}


