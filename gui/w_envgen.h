#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#define abs fabs
#endif

#include "ggee.h"
#define BORDER 2

#if 0 /* backwards compatiblity */
#define text_xpix(a,b) a.te_xpos
#define text_ypix(a,b) a.te_ypos
#endif

static void draw_inlets(t_envgen *x, t_glist *glist, int firsttime, int nin, int nout)
{
     int n = nin;
     int nplus, i;
     nplus = (n == 1 ? 1 : n-1);
     for (i = 0; i < n; i++)
     {
	  int onset = x->x_obj.te_xpix + (x->w.width - IOWIDTH) * i / nplus - BORDER;
	  if (firsttime)
	       sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xo%d\n",
			glist_getcanvas(glist),
			onset, x->x_obj.te_ypix + x->w.height - 1 + 2*BORDER,
			onset + IOWIDTH, x->x_obj.te_ypix + x->w.height + 2*BORDER,
			x, i);
	  else
	       sys_vgui(".x%x.c coords %xo%d %d %d %d %d\n",
			glist_getcanvas(glist), x, i,
			onset, x->x_obj.te_ypix + x->w.height - 1 + 2*BORDER,
			onset + IOWIDTH, x->x_obj.te_ypix + x->w.height + 2*BORDER);
     }
     n = nout; 
     nplus = (n == 1 ? 1 : n-1);
     for (i = 0; i < n; i++)
     {
	  int onset = x->x_obj.te_xpix + (x->w.width - IOWIDTH) * i / nplus - BORDER;
	  if (firsttime)
	       sys_vgui(".x%x.c create rectangle %d %d %d %d -tags %xi%d\n",
			glist_getcanvas(glist),
			onset, x->x_obj.te_ypix - BORDER,
			     onset + IOWIDTH, x->x_obj.te_ypix + 1 - BORDER,
			x, i);
	  else
	       sys_vgui(".x%x.c coords %xi%d %d %d %d %d\n",
			glist_getcanvas(glist), x, i,
			onset, x->x_obj.te_ypix - BORDER,
			onset + IOWIDTH, x->x_obj.te_ypix + 1 - BORDER);
	  
     }
}



static int envgen_next_doodle(t_envgen *x, int xpos,int ypos)
{
     int ret = -1;
     float xscale,yscale;
     int dxpos,dypos;
     float minval = 1000000.0;
     float tval;
     int i;
     int insertpos = -1;


     xscale = x->w.width/x->duration[x->last_state];
     yscale = x->w.height;
     
     dxpos = x->x_obj.te_xpix;/* + BORDER */;
     dypos = x->x_obj.te_ypix + BORDER;

     for (i=0;i<=x->last_state;i++) {
	  float dx2 = (dxpos + (x->duration[i] * xscale)) - xpos;
	  float dy2 = (dypos + yscale - (x->finalvalues[i] * yscale)) - ypos;

	  dx2*=dx2;
	  dy2*=dy2;
	  tval = dx2+dy2;
/*	  post("%i: dist = %f dx=%f dy=%f",i,tval,dx2,dy2);*/
	  if ((dxpos + (x->duration[i] * xscale)) < xpos) 
	    insertpos = i;
	  if (abs(tval) < minval) {
	       minval = abs(tval);
	  }
     }

     /* decide if we want to make a new one */
/*          post("insertpos %d",insertpos); */
     if (minval > /*5*/ 16 && insertpos >= 0 && !x->x_freeze) {

	  if (((dxpos + (x->duration[insertpos] * xscale)) - xpos) < 0)
	       insertpos++;
/*	  post("minval = %f, insertpos = %d",minval,insertpos);*/

	  if (x->last_state+1 >= x->args)
	       envgen_resize(x,x->args+1);

	  for (i=x->last_state;i>=insertpos;i--) {
	       x->duration[i+1] = x->duration[i];
	       x->finalvalues[i+1] = x->finalvalues[i];
	  }

	  x->duration[insertpos] = (float)(xpos-dxpos)/x->w.width*x->duration[x->last_state++];

	  x->w.grabbed = insertpos;
     }

     return insertpos;
}

static void envgen_create_doodles(t_envgen *x, t_glist *glist)
{
     float xscale,yscale;
     int xpos,ypos;
     int i;
     char guistr[255];
     
     xscale = x->w.width/x->duration[x->last_state];
     yscale = x->w.height;
     
     xpos = x->x_obj.te_xpix;
     ypos = (int) (x->x_obj.te_ypix + x->w.height);
     for (i=0;i<=x->last_state;i++) {
	  sprintf(guistr,".x%x.c create oval %d %d %d %d -tags %xD%d",glist_getcanvas(glist),
		   (int) (xpos+(x->duration[i] * xscale) - 2),
		   (int) (ypos - x->finalvalues[i]*yscale - 2),
		   (int) (xpos+(x->duration[i] * xscale)+2),
		   (int) (ypos - x->finalvalues[i]*yscale + 2),
		   x,i);

	  if (i == x->w.grabbed) strcat(guistr," -fill red\n");
	  else strcat(guistr,"\n");
	  sys_vgui("%s",guistr);
     }
     x->w.numdoodles = i;
}


static void envgen_delete_doodles(t_envgen *x, t_glist *glist)
{
     int i;
     for (i=0;i<=x->w.numdoodles;i++) 
	  sys_vgui(".x%x.c delete %xD%d\n",glist_getcanvas(glist),x,i);
}

static void envgen_update_doodles(t_envgen *x, t_glist *glist)
{

     envgen_delete_doodles(x,glist);
/* LATER only create new doodles if necessary */
     envgen_create_doodles(x, glist);
}


static void envgen_create(t_envgen *x, t_glist *glist)
{
     int i;
     static char  buf[1024];
     float xscale,yscale;
     int xpos,ypos;
     char num[40];
     
     sys_vgui(".x%x.c create rectangle \
%d %d %d %d -tags %xS "BACKGROUND"\n",
	      glist_getcanvas(glist),
	      x->x_obj.te_xpix-BORDER, x->x_obj.te_ypix-BORDER,
	      x->x_obj.te_xpix + x->w.width+2*BORDER, x->x_obj.te_ypix + x->w.height+2*BORDER,
	      x);
     
     xscale = x->w.width/x->duration[x->last_state];
     yscale = x->w.height;
     
     sprintf(buf,".x%x.c create line",glist_getcanvas(glist));
     xpos = x->x_obj.te_xpix;
     ypos = (int) (x->x_obj.te_ypix + x->w.height);
     for (i=0;i<=x->last_state;i++) {
	  sprintf(num," %d %d ",(int)(xpos + x->duration[i]*xscale),
		                (int)(ypos - x->finalvalues[i]*yscale));
	  strcat(buf,num);
     }
     
     sprintf(num,"-tags %xP\n",x);
     strcat(buf,num);
/*     post("sending %s",buf); */
     sys_vgui("%s",buf);
     envgen_create_doodles(x,glist);
}


static void envgen_update(t_envgen *x, t_glist *glist)
{
int i;
     static char  buf[1024];
     float xscale,yscale;
     int xpos,ypos;
     char num[40];

     sys_vgui(".x%x.c coords %xS \
%d %d %d %d\n",
	      glist_getcanvas(glist), x,
	      x->x_obj.te_xpix - BORDER, x->x_obj.te_ypix -BORDER,
	      x->x_obj.te_xpix + x->w.width+2*BORDER, x->x_obj.te_ypix + x->w.height+2*BORDER);
     
     
     xscale = x->w.width/x->duration[x->last_state];
     yscale = x->w.height;
     
     sprintf(buf,".x%x.c coords %xP",glist_getcanvas(glist),x);
     xpos = x->x_obj.te_xpix;
     ypos = (int) (x->x_obj.te_ypix + x->w.height);
     for (i=0;i<=x->last_state;i++) {
	  sprintf(num," %d %d ",(int)(xpos + x->duration[i]*xscale),
		                (int) (ypos - x->finalvalues[i]*yscale));
	  strcat(buf,num);
     }
     strcat(buf,"\n");
/*     post("sending %s",buf); */
     sys_vgui("%s",buf);
     envgen_update_doodles(x,glist);
}



void envgen_drawme(t_envgen *x, t_glist *glist, int firsttime)
{

     if (firsttime) envgen_create(x,glist);
     else envgen_update(x,glist);

     draw_inlets(x, glist, firsttime, 1,1);
}




void envgen_erase(t_envgen* x,t_glist* glist)
{
     int n;
     sys_vgui(".x%x.c delete %xS\n",
	      glist_getcanvas(glist), x);

     sys_vgui(".x%x.c delete %xP\n",
	      glist_getcanvas(glist), x);

     n = 1;

     while (n--) {
	  sys_vgui(".x%x.c delete %xi%d\n",glist_getcanvas(glist),x,n);
	  sys_vgui(".x%x.c delete %xo%d\n",glist_getcanvas(glist),x,n);
     }
     
     envgen_delete_doodles(x,glist);
}
	


/* ------------------------ envgen widgetbehaviour----------------------------- */


static void envgen_getrect(t_gobj *z, t_glist *owner,
    int *xp1, int *yp1, int *xp2, int *yp2)
{
    int width, height;
    t_envgen* s = (t_envgen*)z;


    width = s->w.width + 2*BORDER;
    height = s->w.height + 2*BORDER;
    *xp1 = s->x_obj.te_xpix-BORDER;
    *yp1 = s->x_obj.te_ypix-BORDER;
    *xp2 = s->x_obj.te_xpix + width;
    *yp2 = s->x_obj.te_ypix + height;
}

static void envgen_displace(t_gobj *z, t_glist *glist,
    int dx, int dy)
{
    t_envgen *x = (t_envgen *)z;
    x->x_obj.te_xpix += dx;
    x->x_obj.te_ypix += dy;

    envgen_drawme(x, glist, 0);
    canvas_fixlinesfor(glist_getcanvas(glist),(t_text*) x);
}

static void envgen_select(t_gobj *z, t_glist *glist, int state)
{
     t_envgen *x = (t_envgen *)z;
    sys_vgui(".x%x.c itemconfigure %xS -fill %s\n", glist, 
	     x, (state? "blue" : BACKGROUNDCOLOR));
}


static void envgen_activate(t_gobj *z, t_glist *glist, int state)
{
/*    t_text *x = (t_text *)z;
    t_rtext *y = glist_findrtext(glist, x);
    if (z->g_pd != gatom_class) rtext_activate(y, state);*/
}

static void envgen_delete(t_gobj *z, t_glist *glist)
{
    t_text *x = (t_text *)z;
    canvas_deletelinesfor(glist_getcanvas(glist), x);
}

       
static void envgen_vis(t_gobj *z, t_glist *glist, int vis)
{
    t_envgen* s = (t_envgen*)z;
    if (vis)
	 envgen_drawme(s, glist, 1);
    else
	 envgen_erase(s,glist);
}

/* can we use the normal text save function ?? */

static void envgen_save(t_gobj *z, t_binbuf *b)
{
    t_envgen *x = (t_envgen *)z;
    binbuf_addv(b, "ssiisii", gensym("#X"),gensym("obj"),
		(t_int)x->x_obj.te_xpix, (t_int)x->x_obj.te_ypix,  
		gensym("envgen"),x->w.width,x->w.height);
    binbuf_addv(b, ";");
}



static void envgen_followpointer(t_envgen* x)
{
     float dur;

     float xscale = x->duration[x->last_state]/x->w.width;
     
     if  ((x->w.grabbed > 0) && (x->w.grabbed < x->last_state)) {
	  
	  dur = (x->w.pointerx - x->x_obj.te_xpix)*xscale;
	  if (dur < x->duration[x->w.grabbed-1])
	       dur = x->duration[x->w.grabbed-1] + 1.0;
	  if (dur > x->duration[x->w.grabbed+1])	  
	       dur = x->duration[x->w.grabbed+1] - 0.2;

	  x->duration[x->w.grabbed] = dur;
     }
     

     x->finalvalues[x->w.grabbed] = 1.0f - (float)(x->w.pointery - x->x_obj.te_ypix)/(float)x->w.height;
     if (x->finalvalues[x->w.grabbed] < 0.0) 
	  x->finalvalues[x->w.grabbed]= 0.0;
     else if (x->finalvalues[x->w.grabbed] > 1.0)
	  x->finalvalues[x->w.grabbed]= 1.0;

}


void envgen_motion(t_envgen *x, t_floatarg dx, t_floatarg dy)
{
     x->w.pointerx+=dx;
     x->w.pointery+=dy;

     if (!x->resizing)
	  envgen_followpointer(x);
     else {
	  x->w.width+=dx;
	  x->w.height+=dy;
     }
     envgen_update(x,x->w.glist);
}

void envgen_key(t_envgen *x, t_floatarg f)
{
     if (f == 8.0 && x->w.grabbed < x->last_state &&  x->w.grabbed > 0) {
	  int i;

	  for (i=x->w.grabbed;i<=x->last_state;i++) {
	       x->duration[i] = x->duration[i+1];
	       x->finalvalues[i] = x->finalvalues[i+1];
	  }

	  x->last_state--;
	  x->w.grabbed--;
	  envgen_update(x,x->w.glist);
     }
}



void envgen_click(t_envgen *x,
    t_floatarg xpos, t_floatarg ypos, t_floatarg shift, t_floatarg ctrl,
    t_floatarg alt)
{
    /* check if user wants to resize */
     float wxpos = x->x_obj.te_xpix;
     float wypos = (int) (x->x_obj.te_ypix + x->w.height);

     x->w.grabbed = envgen_next_doodle(x,xpos,ypos);
#if (PD_VERSION_MINOR > 31)
     glist_grab(x->w.glist, &x->x_obj.te_g, envgen_motion, envgen_key, xpos, ypos);
#else
     glist_grab(x->w.glist, &x->x_obj.te_g, xpos, ypos);
#endif
     x->resizing = 0;     
     if ((xpos > wxpos + x->w.width - 3) && 
	 (fabs(ypos -2 - wypos) < 3.)) {
	  x->resizing = 1;     
	  return;
     }

     x->w.pointerx = xpos;
     x->w.pointery = ypos;
     envgen_followpointer(x);
     envgen_update(x,x->w.glist);
}


#if (PD_VERSION_MINOR > 31) 
static int envgen_newclick(t_gobj *z, struct _glist *glist,
    int xpix, int ypix, int shift, int alt, int dbl, int doit)
{
    	if (doit)
	    envgen_click((t_envgen *)z, (t_floatarg)xpix, (t_floatarg)ypix,
	    	(t_floatarg)shift, 0, (t_floatarg)alt);
	return (1);
}
#endif



t_widgetbehavior envgen_widgetbehavior;

void envgen_setwidget()
{
    envgen_widgetbehavior.w_getrectfn =     envgen_getrect;
    envgen_widgetbehavior.w_displacefn =    envgen_displace;
    envgen_widgetbehavior.w_selectfn =   envgen_select;
    envgen_widgetbehavior.w_activatefn =   envgen_activate;
    envgen_widgetbehavior.w_deletefn =   envgen_delete;
    envgen_widgetbehavior.w_visfn =   envgen_vis;
#if (PD_VERSION_MINOR > 31) 
    envgen_widgetbehavior.w_clickfn = envgen_newclick;
    envgen_widgetbehavior.w_propertiesfn = NULL; 
#endif
    envgen_widgetbehavior.w_savefn =   envgen_save;
    
}

