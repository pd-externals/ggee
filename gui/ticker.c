/* (C) Guenter Geiger <geiger@epy.co.at> */


#include <ggee.h>
#include <m_pd.h>
#include "g_canvas.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ------------------------ ticker ----------------------------- */

#define DEFAULTSIZE 15
#define DEFAULTCOLOR "black"

static t_class *ticker_class;

typedef struct _ticker
{
     t_object x_obj;
     t_glist * x_glist;
     int x_width;
     int x_height;
     t_float x_on;
} t_ticker;

/* widget helper functions */


static void draw_inlets(t_ticker *x, t_glist *glist, int firsttime, int nin, int nout)
{
     int n = nin;
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
     n = nout; 
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



void ticker_draw(t_ticker *x,t_glist *glist)
{
     if (x->x_on) {
	  sys_vgui(".x%x.c create line \
%d %d %d %d -tags %xB\n",glist_getcanvas(glist),
		   x->x_obj.te_xpix+1,x->x_obj.te_ypix+1,
		   x->x_obj.te_xpix + x->x_width -1,
		   x->x_obj.te_ypix + x->x_height -1,x);
	  sys_vgui(".x%x.c create line \
%d %d %d %d -tags %xC\n",glist_getcanvas(glist),
		   x->x_obj.te_xpix+1,x->x_obj.te_ypix + x->x_height -1,
		   x->x_obj.te_xpix + x->x_width -1,x->x_obj.te_ypix+1,
		   x);
	  
     }	  
     else {
	  sys_vgui(".x%x.c delete %xB\n",
		   glist_getcanvas(glist), x);
	  sys_vgui(".x%x.c delete %xC\n",
		   glist_getcanvas(glist), x);
     }	       
}


void ticker_update(t_ticker *x,t_glist *glist)
{
     if (glist_isvisible(glist)) {
	  if (x->x_on) {
	       sys_vgui(".x%x.c coords %xB \
%d %d %d %d\n",glist_getcanvas(glist),
			x,x->x_obj.te_xpix+1,x->x_obj.te_ypix+1,
			x->x_obj.te_xpix + x->x_width -1,
			x->x_obj.te_ypix + x->x_height -1);
	       sys_vgui(".x%x.c coords %xC \
%d %d %d %d\n",glist_getcanvas(glist),
			x,x->x_obj.te_xpix+1,x->x_obj.te_ypix + x->x_height -1,
			x->x_obj.te_xpix + x->x_width -1,x->x_obj.te_ypix+1);
	       
	  }	  
     }
}



void ticker_drawme(t_ticker *x, t_glist *glist, int firsttime)
{
     if (firsttime) {
	  sys_vgui(".x%x.c create rectangle \
%d %d %d %d -tags %xS "BACKGROUND"\n",
		   glist_getcanvas(glist),
		   x->x_obj.te_xpix, x->x_obj.te_ypix,
		   x->x_obj.te_xpix + x->x_width, x->x_obj.te_ypix + x->x_height,
		   x);
	  ticker_draw(x,glist);
     }     
     else {
	  sys_vgui(".x%x.c coords %xS \
%d %d %d %d\n",
		   glist_getcanvas(glist), x,
		   x->x_obj.te_xpix, x->x_obj.te_ypix,
		   x->x_obj.te_xpix + x->x_width, x->x_obj.te_ypix + x->x_height);
	  ticker_update(x,glist);
     }

     draw_inlets(x, glist, firsttime, 1,1);

}




void ticker_erase(t_ticker* x,t_glist* glist)
{
     int n;
     sys_vgui(".x%x.c delete %xS\n",
	      glist_getcanvas(glist), x);

     sys_vgui(".x%x.c delete %xP\n",
	      glist_getcanvas(glist), x);

     n = 1;
     sys_vgui(".x%x.c delete %xB\n",
	      glist_getcanvas(glist), x);
     sys_vgui(".x%x.c delete %xC\n",
	      glist_getcanvas(glist), x);

     while (n--) {
	  sys_vgui(".x%x.c delete %xi%d\n",glist_getcanvas(glist),x,n);
	  sys_vgui(".x%x.c delete %xo%d\n",glist_getcanvas(glist),x,n);
     }
}
	


/* ------------------------ ticker widgetbehaviour----------------------------- */


static void ticker_getrect(t_gobj *z, t_glist *owner,
    int *xp1, int *yp1, int *xp2, int *yp2)
{
    int width, height;
    t_ticker* s = (t_ticker*)z;


    width = s->x_width;
    height = s->x_height;
    *xp1 = s->x_obj.te_xpix;
    *yp1 = s->x_obj.te_ypix;
    *xp2 = s->x_obj.te_xpix + width;
    *yp2 = s->x_obj.te_ypix + height;
}

static void ticker_displace(t_gobj *z, t_glist *glist,
    int dx, int dy)
{
    t_ticker *x = (t_ticker *)z;
    x->x_obj.te_xpix += dx;
    x->x_obj.te_ypix += dy;
    ticker_drawme(x, glist, 0);
    canvas_fixlinesfor(glist_getcanvas(glist),(t_text*) x);
}

static void ticker_select(t_gobj *z, t_glist *glist, int state)
{
     t_ticker *x = (t_ticker *)z;
    sys_vgui(".x%x.c itemconfigure %xS -fill %s\n", glist, 
	     x, (state? "blue" : BACKGROUNDCOLOR));
}


static void ticker_activate(t_gobj *z, t_glist *glist, int state)
{
/*    t_text *x = (t_text *)z;
    t_rtext *y = glist_findrtext(glist, x);
    if (z->g_pd != gatom_class) rtext_activate(y, state);*/
}

static void ticker_delete(t_gobj *z, t_glist *glist)
{
    t_text *x = (t_text *)z;
    canvas_deletelinesfor(glist_getcanvas(glist), x);
}

       
static void ticker_vis(t_gobj *z, t_glist *glist, int vis)
{
    t_ticker* s = (t_ticker*)z;
    if (vis)
	 ticker_drawme(s, glist, 1);
    else
	 ticker_erase(s,glist);
}

/* can we use the normal text save function ?? */

static void ticker_save(t_gobj *z, t_binbuf *b)
{
    t_ticker *x = (t_ticker *)z;
    binbuf_addv(b, "ssiisii", gensym("#X"),gensym("obj"),
		(t_int)x->x_obj.te_xpix, (t_int)x->x_obj.te_ypix,  
		gensym("ticker"),x->x_width,x->x_height);
    binbuf_addv(b, ";");
}


static void ticker_click(t_ticker *x,
    t_floatarg xpos, t_floatarg ypos, t_floatarg shift, t_floatarg ctrl,
    t_floatarg alt)
{
     x->x_on = x->x_on == 0.0 ? 1.0:0.0;
     ticker_draw(x,x->x_glist);
     outlet_float(x->x_obj.ob_outlet,x->x_on);
}

#if (PD_VERSION_MINOR > 31) 
static int ticker_newclick(t_gobj *z, struct _glist *glist,
    int xpix, int ypix, int shift, int alt, int dbl, int doit)
{
    	if (doit)
	    ticker_click((t_ticker *)z, (t_floatarg)xpix, (t_floatarg)ypix,
	    	(t_floatarg)shift, 0, (t_floatarg)alt);
	return (1);
}
#endif

t_widgetbehavior ticker_widgetbehavior;


void ticker_size(t_ticker* x,t_floatarg w,t_floatarg h) {
     x->x_width = w;
     x->x_height = h;
     ticker_drawme(x, x->x_glist, 0);
}


static void ticker_bang(t_ticker* x)
{
     x->x_on = x->x_on == 0.0 ? 1.0:0.0;
     if (glist_isvisible(x->x_glist))
	 ticker_draw(x,x->x_glist);
     outlet_float(x->x_obj.ob_outlet,x->x_on);     
}

static void ticker_float(t_ticker* x,t_floatarg f)
{
     x->x_on = f;
     if (glist_isvisible(x->x_glist))
	  ticker_draw(x,x->x_glist);
     outlet_float(x->x_obj.ob_outlet,x->x_on);     
}

static void ticker_set(t_ticker* x,t_floatarg f)
{
     x->x_on = f;
}

static void *ticker_new(t_floatarg h,t_floatarg o)
{
    t_ticker *x = (t_ticker *)pd_new(ticker_class);

    x->x_glist = (t_glist*) canvas_getcurrent();
    if (h) x->x_width = h;
    else
	 x->x_width = DEFAULTSIZE;

    if (o) x->x_height = o;
    else
	 x->x_height = DEFAULTSIZE;

    x->x_on = 0;

    outlet_new(&x->x_obj, &s_float);
    return (x);
}

void ticker_setup(void)
{
    ticker_class = class_new(gensym("ticker"), (t_newmethod)ticker_new, 0,
				sizeof(t_ticker),0, A_DEFFLOAT,A_DEFFLOAT,0);


    class_addmethod(ticker_class, (t_method)ticker_click, gensym("click"),
    	A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(ticker_class, (t_method)ticker_size, gensym("size"),
    	A_FLOAT, A_FLOAT, 0);

    class_addfloat(ticker_class, ticker_float);
    class_addbang(ticker_class,ticker_bang);
    class_addmethod(ticker_class,(t_method) ticker_set,gensym("set"),A_FLOAT,0);
    class_setwidget(ticker_class,&ticker_widgetbehavior);

    ticker_widgetbehavior.w_getrectfn =     ticker_getrect;
    ticker_widgetbehavior.w_displacefn =    ticker_displace;
    ticker_widgetbehavior.w_selectfn =   ticker_select;
    ticker_widgetbehavior.w_activatefn =   ticker_activate;
    ticker_widgetbehavior.w_deletefn =   ticker_delete;
    ticker_widgetbehavior.w_visfn =   ticker_vis;
#if (PD_VERSION_MINOR > 31) 
    ticker_widgetbehavior.w_clickfn = ticker_newclick;
    ticker_widgetbehavior.w_propertiesfn = NULL;
#endif
    ticker_widgetbehavior.w_savefn =   ticker_save;
}


