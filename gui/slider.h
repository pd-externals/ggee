#ifndef __SLIDER_H__
#define __SLIDER_H__

/* changes for setting width by <dieter@rhiz.org> 
   set message by <dieter@klingt.org>
*/

#include "ggee.h"
#include "m_imp.h"
#include "g_canvas.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#define SPOSITIONS
#define te_xpos te_xpix
#define te_ypos te_ypix

typedef struct _slider
{
     t_object x_obj;
     t_atom a_pos;
     t_atom a_pos2;
     t_glist * x_glist;
     int x_width;
     int x_height;
     int x_offset; 
     int x_mpos;
     int x_num;
     int x_horizontal;
} t_slider;

#define x_pos a_pos.a_w.w_float
#define x_pos2 a_pos2.a_w.w_float


/* widget helper functions */

static void draw_inlets(t_slider *x, t_glist *glist, int firsttime, int nin, int nout)
{
     int n = nin;
     int nplus, i;
     nplus = (n == 1 ? 1 : n-1);
     for (i = 0; i < n; i++)
     {
	  int onset = x->x_obj.te_xpos + (x->x_width - IOWIDTH) * i / nplus;
	  if (firsttime)
	       sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xo%d\n",
			glist_getcanvas(glist),
			onset, x->x_obj.te_ypos + x->x_height - 1,
			onset + IOWIDTH, x->x_obj.te_ypos + x->x_height,
			x, i);
	  else
	       sys_vgui(".x%x.c coords %xo%d %d %d %d %d\n",
			glist_getcanvas(glist), x, i,
			onset, x->x_obj.te_ypos + x->x_height - 1,
			onset + IOWIDTH, x->x_obj.te_ypos + x->x_height);
     }
     n = nout; 
     nplus = (n == 1 ? 1 : n-1);
     for (i = 0; i < n; i++)
     {
	  int onset = x->x_obj.te_xpos + (x->x_width - IOWIDTH) * i / nplus;
	  if (firsttime)
	       sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xi%d\n",
			glist_getcanvas(glist),
			onset, x->x_obj.te_ypos,
			     onset + IOWIDTH, x->x_obj.te_ypos + 1,
			x, i);
	  else
	       sys_vgui(".x%x.c coords %xi%d %d %d %d %d\n",
			glist_getcanvas(glist), x, i,
			onset, x->x_obj.te_ypos,
			onset + IOWIDTH, x->x_obj.te_ypos + 1);
	  
     }
}



static void slider_update(t_slider *x, t_glist *glist)
{
     if (glist_isvisible(glist)) {
	  if (!x->x_horizontal) {
	       sys_vgui(".x%x.c coords %xP \
%d %d %d %d\n",glist_getcanvas(glist),x,
			x->x_obj.te_xpos,x->x_obj.te_ypos + x->x_height - (int)x->x_pos + x->x_offset,
			x->x_obj.te_xpos + x->x_width,x->x_obj.te_ypos + x->x_height - (int)x->x_pos + x->x_offset);
	       sys_vgui(".x%x.c coords %xP2 \
%d %d %d %d\n",glist_getcanvas(glist),x,
			x->x_obj.te_xpos,x->x_obj.te_ypos + x->x_height - (int)x->x_pos2 + x->x_offset,
			x->x_obj.te_xpos + x->x_width,x->x_obj.te_ypos + x->x_height - (int)x->x_pos2 + x->x_offset);
	  }
	  else {
	       sys_vgui(".x%x.c coords %xP \
%d %d %d %d\n",glist_getcanvas(glist),x,
			x->x_obj.te_xpos +(int)x->x_pos - x->x_offset ,x->x_obj.te_ypos,
			x->x_obj.te_xpos +(int)x->x_pos - x->x_offset ,x->x_obj.te_ypos + x->x_height);
	       sys_vgui(".x%x.c coords %xP2 \
%d %d %d %d\n",glist_getcanvas(glist),x,
			x->x_obj.te_xpos +(int)x->x_pos2 - x->x_offset ,x->x_obj.te_ypos,
			x->x_obj.te_xpos +(int)x->x_pos2 - x->x_offset ,x->x_obj.te_ypos + x->x_height);
	  }
     }
}


static void slider_drawme(t_slider *x, t_glist *glist, int firsttime)
{
     if (firsttime) {
	  sys_vgui(".x%x.c create rectangle \
%d %d %d %d -tags %xS "BACKGROUND"\n",
		   glist_getcanvas(glist),
		   x->x_obj.te_xpos, x->x_obj.te_ypos,
		   x->x_obj.te_xpos + x->x_width, x->x_obj.te_ypos + x->x_height,
		   x);
	  if (!x->x_horizontal) {
	       sys_vgui(".x%x.c create line \
%d %d %d %d -width 3 -tags %xP\n",glist_getcanvas(glist),
			x->x_obj.te_xpos,x->x_obj.te_ypos + x->x_height - (int)x->x_pos + x->x_offset,
			x->x_obj.te_xpos + x->x_width,
			x->x_obj.te_ypos + x->x_height - (int)x->x_pos + x->x_offset,x);

	       sys_vgui(".x%x.c create line \
%d %d %d %d -width 3 -tags %xP2\n",glist_getcanvas(glist),
			x->x_obj.te_xpos,x->x_obj.te_ypos + x->x_height - (int)x->x_pos2 + x->x_offset,
			x->x_obj.te_xpos + x->x_width,
			x->x_obj.te_ypos + x->x_height - (int)x->x_pos2 + x->x_offset,x);
	  }
	  else {
	       sys_vgui(".x%x.c create line \
%d %d %d %d -width 3 -tags %xP\n",glist_getcanvas(glist),
			x->x_obj.te_xpos + (int)x->x_pos + x->x_offset ,x->x_obj.te_ypos, 
			x->x_obj.te_xpos + (int)x->x_pos + x->x_offset,
			x->x_obj.te_ypos + x->x_height,x );
	       sys_vgui(".x%x.c create line \
%d %d %d %d -width 3 -tags %xP2\n",glist_getcanvas(glist),
			x->x_obj.te_xpos + (int)x->x_pos2 + x->x_offset ,x->x_obj.te_ypos, 
			x->x_obj.te_xpos + (int)x->x_pos2 + x->x_offset,
			x->x_obj.te_ypos + x->x_height,x );
	  }
     }     
     else {
	  sys_vgui(".x%x.c coords %xS \
%d %d %d %d\n",
		   glist_getcanvas(glist), x,
		   x->x_obj.te_xpos, x->x_obj.te_ypos,
		   x->x_obj.te_xpos + x->x_width, x->x_obj.te_ypos + x->x_height);
	  slider_update(x, glist);
     }

     draw_inlets(x, glist, firsttime, 1,1);

}




static void slider_erase(t_slider* x,t_glist* glist)
{
     int n;
     sys_vgui(".x%x.c delete %xS\n",
	      glist_getcanvas(glist), x);

     sys_vgui(".x%x.c delete %xP\n",
	      glist_getcanvas(glist), x);

    sys_vgui(".x%x.c delete %xP2\n",
             glist_getcanvas(glist), x);
    
    n = x->x_num;

     while (n--) {
	  sys_vgui(".x%x.c delete %xi%d\n",glist_getcanvas(glist),x,n);
	  sys_vgui(".x%x.c delete %xo%d\n",glist_getcanvas(glist),x,n);
     }
}
	


/* ------------------------ slider widgetbehaviour----------------------------- */


static void slider_getrect(t_gobj *z, t_glist *owner,
    int *xp1, int *yp1, int *xp2, int *yp2)
{
    int width, height;
    t_slider* s = (t_slider*)z;


    width = s->x_width;
    height = s->x_height;
    *xp1 = s->x_obj.te_xpos;
    *yp1 = s->x_obj.te_ypos;
    *xp2 = s->x_obj.te_xpos + width;
    *yp2 = s->x_obj.te_ypos + height;
}

static void slider_displace(t_gobj *z, t_glist *glist,
    int dx, int dy)
{
    t_slider *x = (t_slider *)z;
    x->x_obj.te_xpos += dx;
    x->x_obj.te_ypos += dy;
    slider_drawme(x, glist, 0);
    canvas_fixlinesfor(glist_getcanvas(glist),(t_text*) x);
}

static void slider_select(t_gobj *z, t_glist *glist, int state)
{
     t_slider *x = (t_slider *)z;
    sys_vgui(".x%x.c itemconfigure %xS -fill %s\n", glist, 
	     x, (state? "blue" : BACKGROUNDCOLOR));
}


static void slider_activate(t_gobj *z, t_glist *glist, int state)
{
/*    t_text *x = (t_text *)z;
    t_rtext *y = glist_findrtext(glist, x);
    if (z->g_pd != gatom_class) rtext_activate(y, state);*/
}

static void slider_delete(t_gobj *z, t_glist *glist)
{
    t_text *x = (t_text *)z;
    canvas_deletelinesfor(glist_getcanvas(glist), x);
}

       
static void slider_vis(t_gobj *z, t_glist *glist, int vis)
{
    t_slider* s = (t_slider*)z;
    if (vis)
	 slider_drawme(s, glist, 1);
    else
	 slider_erase(s,glist);
}

/* can we use the normal text save function ?? */

static void slider_save(t_gobj *z, t_binbuf *b)
{
    t_slider *x = (t_slider *)z;
    t_symbol* sname;
    int maxlen,maxwidth;

/*    if (!x->x_horizontal) {*/
    sname = gensym("slider");
    maxlen = x->x_height + x->x_offset;
    maxwidth = x->x_width;
/*    }*/   

    /* forget hsliders ...
    else {
	 sname = gensym("hslider");
	 maxlen = x->x_width + x->x_offset;
	 maxwidth = x->x_height;
    }
*/

    
    binbuf_addv(b, "ssiisiiii", gensym("#X"),gensym("obj"),
		(t_int)x->x_obj.te_xpos, (t_int)x->x_obj.te_ypos,  sname,maxlen,
		x->x_offset,maxwidth,x->x_num,x->x_horizontal);
/*        binbuf_addbinbuf(b, x->te_binbuf); */
    binbuf_addv(b, ";");
}


t_widgetbehavior slider_widgetbehavior;


static void slider_motion(t_slider *x, t_floatarg dx, t_floatarg dy)
{
     int max;

     if (!x->x_horizontal) {
	  x->x_mpos -= dy;
	  max = (x->x_offset + x->x_height);
     }
     else {
	  x->x_mpos += dx;
	  max = (x->x_offset + x->x_width);
     }

     if (x->x_mpos < x->x_offset)
	  x->x_pos = x->x_offset;
     else if (x->x_mpos > (max))
	  x->x_pos = (max);
     else
	  x->x_pos = x->x_mpos;

     
     slider_update(x, x->x_glist);
     outlet_float(x->x_obj.ob_outlet, x->x_pos);

}


static void slider_click(t_slider *x,
    t_floatarg xpos, t_floatarg ypos, t_floatarg shift, t_floatarg ctrl,
    t_floatarg alt)
{
     
     if (!x->x_horizontal)
	  x->x_mpos = x->x_pos = x->x_obj.te_ypos + x->x_height + x->x_offset - ypos;
     else
	  x->x_mpos = x->x_pos = xpos - x->x_obj.te_xpos + x->x_offset;

     slider_update(x, x->x_glist);
     outlet_float(x->x_obj.ob_outlet, x->x_pos);
#if (PD_VERSION_MINOR > 31)
     glist_grab(x->x_glist, &x->x_obj.te_g, slider_motion, 0,xpos, ypos);
#else
     glist_grab(x->x_glist, &x->x_obj.te_g, xpos, ypos);
#endif
}


#if (PD_VERSION_MINOR > 31) 
static int slider_newclick(t_gobj *z, struct _glist *glist,
    int xpix, int ypix, int shift, int alt, int dbl, int doit)
{
  t_slider* x = (t_slider *)z;
  if (doit)
    slider_click( x, (t_floatarg)xpix, (t_floatarg)ypix,
		 (t_floatarg)shift, 0, (t_floatarg)alt);
  return (1);
}
#endif

static void slider_setwidget()
{
    slider_widgetbehavior.w_getrectfn =     slider_getrect;
    slider_widgetbehavior.w_displacefn =    slider_displace;
    slider_widgetbehavior.w_selectfn =   slider_select;
    slider_widgetbehavior.w_activatefn =   slider_activate;
    slider_widgetbehavior.w_deletefn =   slider_delete;
    slider_widgetbehavior.w_visfn =   slider_vis;
#if (PD_VERSION_MINOR > 31) 
    slider_widgetbehavior.w_clickfn = slider_newclick;
    slider_widgetbehavior.w_propertiesfn = NULL;
#endif
    slider_widgetbehavior.w_savefn =   slider_save;
}



static void slider_float(t_slider *x,t_floatarg f)
{
     if (!x->x_horizontal) {
	  if (f >= x->x_offset && f <= (x->x_offset + x->x_height))
	       x->x_pos = (int)f;
     }
     else
	  if (f >= x->x_offset && f <= (x->x_offset + x->x_width))
	       x->x_pos = (int)f;
     

     slider_update(x, x->x_glist);
     outlet_float(x->x_obj.ob_outlet, x->x_pos);

}


static void slider_mark(t_slider *x,t_floatarg val)
{
     if (!x->x_horizontal) {
	  if (val >= x->x_offset && val <= (x->x_offset + x->x_height)) 
		    x->x_pos2 = (int)val;
     }
     else
	  if (val >= x->x_offset && val <= (x->x_offset + x->x_width))
		    x->x_pos2 = (int)val;

     slider_update(x, x->x_glist);
}


static void slider_type(t_slider *x,t_floatarg val)
{
  x->x_horizontal=val;
  slider_vis((struct _gobj*)x,x->x_glist,0);
  slider_vis((struct _gobj*)x,x->x_glist,1);
}

static void slider_list(t_slider *x,t_symbol* s,t_int argc, t_atom* argv)
{
     t_float num;
     t_float val;

     if (argc != 2) return;

     num = atom_getfloat(argv++);
     val = atom_getfloat(argv);
     if (!x->x_horizontal) {
	  if (val >= x->x_offset && val <= (x->x_offset + x->x_height)) 
	       if (num)
		    x->x_pos2 = (int)val;
	       else
		    x->x_pos = (int)val;
     }
     else
	  if (val >= x->x_offset && val <= (x->x_offset + x->x_width))
	       if (num)
		    x->x_pos2 = (int)val;
	       else
		    x->x_pos = (int)val;

     slider_update(x, x->x_glist);
     outlet_float(x->x_obj.ob_outlet, x->x_pos);

}
     


static void slider_bang(t_slider *x)
{
     outlet_float(x->x_obj.ob_outlet, x->x_pos);
}


static void slider_set(t_slider *x,t_floatarg f)
{
     if (!x->x_horizontal) {
          if (f >= x->x_offset && f <= (x->x_offset + x->x_height))
               x->x_pos = (int)f;
     }            
     else{                                                               
          if (f >= x->x_offset && f <= (x->x_offset + x->x_width))
               x->x_pos = (int)f;
     }            

     slider_update(x, x->x_glist);
}

#endif
