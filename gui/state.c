/* (C) Guenter Geiger <geiger@epy.co.at> */


#include <m_pd.h>
#include <stdio.h>
#include <string.h>
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif


/*
 * The iem stuff doesn't work yet !!!
*/

typedef struct _iem_fstyle_flags
{
    unsigned int x_font_style:6;
    unsigned int x_rcv_able:1;
    unsigned int x_snd_able:1;
    unsigned int x_lab_is_unique:1;
    unsigned int x_rcv_is_unique:1;
    unsigned int x_snd_is_unique:1;
    unsigned int x_lab_arg_tail_len:6;
    unsigned int x_lab_is_arg_num:6;
    unsigned int x_shiftdown:1;
    unsigned int x_selected:1;
    unsigned int x_finemoved:1;
    unsigned int x_put_in2out:1;
    unsigned int x_change:1;
    unsigned int x_thick:1;
    unsigned int x_lin0_log1:1;
    unsigned int x_steady:1;
    unsigned int dummy:1;
} t_iem_fstyle_flags;

typedef struct _iem_init_symargs
{
    unsigned int x_loadinit:1;
    unsigned int x_rcv_arg_tail_len:6;
    unsigned int x_snd_arg_tail_len:6;
    unsigned int x_rcv_is_arg_num:6;
    unsigned int x_snd_is_arg_num:6;
    unsigned int x_scale:1;
    unsigned int x_flashed:1;
    unsigned int x_locked:1;
    unsigned int dummy:4;
} t_iem_init_symargs;


typedef struct _iemgui
{
    t_object           x_obj;
    void            *x_glist;
    void*        x_draw;
    int                x_h;
    int                x_w;
    int                x_ldx;
    int                x_ldy;
    char               x_font[16];
    t_iem_fstyle_flags x_fsf;
    int                x_fontsize;
    t_iem_init_symargs x_isa;
    int                x_fcol;
    int                x_bcol;
    int                x_lcol;
    int                x_unique_num;
    t_symbol           *x_snd;
    t_symbol           *x_rcv;
    t_symbol           *x_lab;
} t_iemgui;



/* hacks .... this duplicates definitions in pd and must be 
 * adjusted if something changes there !!!! */

#define EMPTYSYMBOL "emptysymbol"

typedef struct _mygatom
{
    t_text a_text;
    t_atom a_atom;
} t_mygatom; /* this is not !! the real t_atom ...*/

#define TATOM(a) (&((t_mygatom*)a)->a_atom)


/* glist's are not visible, but the only thing we need is to
   get the list link from them */

typedef struct _myglist
{
     t_object gl_gobj;
     t_gobj* g_list;
} t_myglist;

#define FIRSTOBJECT(a) (((t_myglist*)a)->g_list)


#ifndef vmess 
#define vmess pd_vmess
#endif


/* ------------------------ State ----------------------------- */

#include "envgen.h"


static t_class *state_class;


typedef struct _state
{
     t_object  x_obj;
     t_canvas* x_canvas;
     t_symbol* x_name;
     int       x_slot;
     t_symbol* x_symslot;
     int       x_save;
     int       x_loading;
     t_clock*  x_clock;
} t_state;




void state_dosave(t_state *x)
{
     char name[255];
     FILE* fp;
     t_text* a;
     t_symbol* dir;
     char    dirstr[255];

#ifdef NT
     dir = gensym("");
#else
     dir = canvas_getdir(x->x_canvas);
#endif

     strcpy(dirstr,dir->s_name);

#ifndef NT
     strcat(dirstr,"/");
#endif

     if (x->x_symslot)
	  sprintf(name,"%s%s.%s",dirstr,x->x_name->s_name,x->x_symslot->s_name);
     else
	  sprintf(name,"%s%s.%d",dirstr,x->x_name->s_name,x->x_slot);

     fp = fopen(name,"w");
     if (!fp) {
	  post("state: unable to open %s",name);
	  return;
     }

     a = (t_text*)FIRSTOBJECT(x->x_canvas);

     do {  
	  if (a->te_type == T_ATOM) {
	       if (TATOM(a)->a_type == A_SYMBOL) {
		    if (strlen(TATOM(a)->a_w.w_symbol->s_name))
/*			 fprintf(fp,"%s\n",TATOM(a)->a_w.w_symbol->s_name);*/
			 fprintf(fp,"%s\n",atom_getsymbol(TATOM(a))->s_name);
		    else
			 fprintf(fp,EMPTYSYMBOL"\n");
	       }
	       else {
		    fprintf(fp,"%f\n",atom_getfloat(TATOM(a)));
		    fprintf(stderr,"%f\n",atom_getfloat(TATOM(a)));
	       }
	  }

	  /* slider should be an atom as well ... how to do it ? */

	  if (!strcmp(class_getname(a->te_pd),"slider")) {
	        float val = atom_getfloat(TATOM(a));
		fprintf(fp,"%f\n",val);
		fprintf(stderr,"slider %f\n",atom_getfloat(TATOM(a)));
	  }
#if 0
	  if (!strcmp(class_getname(a->te_pd),"vsl")) {
	    /*	    float val = atom_getfloat(TATOM(a));*/
	    float val = *((float*) (((char*)a) + sizeof(t_iemgui) + sizeof(int)));
		fprintf(fp,"%f\n",val);
		fprintf(stderr,"vslider %f\n",val);
	  }
	  if (!strcmp(class_getname(a->te_pd),"hsl")) {
	    float val = *((float*) (((char*)a) + sizeof(t_iemgui) + sizeof(int)));
		fprintf(fp,"%f\n",val);
		fprintf(stderr,"hslider %f\n",val);
	  }
#endif
	  if (!strncmp(class_getname(a->te_pd),"envgen",6)) {
	       int i;
	       t_envgen* e = (t_envgen*) a;

	       fprintf(fp,"%d ",e->last_state);
	       fprintf(fp,"%f ",e->finalvalues[0]);
	       for (i=1;i <= e->last_state;i++)
		    fprintf(fp,"%f %f ",e->duration[i] - e->duration[i-1],e->finalvalues[i]);
	       fprintf(fp,"\n");
	  }



     } while ((a = (t_text*)((t_gobj*)a)->g_next));
     post("state saved to: %s",name);

     fclose(fp);

}

void state_save(t_state *x) 
{
     x->x_save = 1;
     clock_delay(x->x_clock,2000);
}


void state_saveoff(t_state *x) 
{
     x->x_save = 0;
}

void state_load(t_state *x)
{
     char name[255];
     FILE* fp;
     t_text* a;
     t_float  in;
     t_symbol* dir;
     char    dirstr[255];

#ifdef NT
     dir = gensym("");
#else
     dir = canvas_getdir(x->x_canvas);
#endif

     strcpy(dirstr,dir->s_name);

#ifndef NT
     strcat(dirstr,"/");
#endif


     if (x->x_symslot)
	  sprintf(name,"%s%s.%s",dirstr,x->x_name->s_name,x->x_symslot->s_name);
     else
	  sprintf(name,"%s%s.%d",dirstr,x->x_name->s_name,x->x_slot);

     fp = fopen(name,"r");
     if (!fp) {
	  post("state: unable to open %s",name);
	  return;
     }

     a = (t_text*) FIRSTOBJECT(x->x_canvas);

     x->x_loading = 1;
     post("state loading from: %s",name);
     *name = 0;
     do {
	  if (a->te_type == T_ATOM || 
	      !strcmp(class_getname(a->te_pd),"slider")
	/* ||
	     !strcmp(class_getname(a->te_pd),"vsl") ||
	      !strcmp(class_getname(a->te_pd),"hsl" ) */
	      ) {
	       if (TATOM(a)->a_type == A_SYMBOL) {
		    fscanf(fp,"%s",name);
		    if (strcmp(name,EMPTYSYMBOL))
			vmess((t_pd*)a,gensym("set"),"s",gensym(name));
	       }
	       else {
		    fscanf(fp,"%f",&in);
		    vmess((t_pd*)a,&s_float,"f",in);
	       }
	  }

	  if (!strncmp(class_getname(a->te_pd),"envgen",6)) {
	       int i;
	       int end;
	       float val;
	       float dur;
	       t_atom  ilist[255];

	       fscanf(fp,"%f",&in);
	       end = in;

	       fscanf(fp,"%f",&val);
	       SETFLOAT(ilist,val);
	       for (i=1 ;i <= end;i++) {
		    fscanf(fp,"%f",&dur);
		    fscanf(fp,"%f",&val);
		    SETFLOAT(ilist +2*i-1,dur);
		    SETFLOAT(ilist+2*i,val);
	       }
	       pd_typedmess((t_pd*)a,&s_list,2*end+1,ilist);
	       post("ok %d",end);
	  }


     } while ((a = (t_text*)((t_gobj*)a)->g_next) && !feof(fp));

     x->x_loading = 0;
     fclose(fp);

}

void state_float(t_state *x,t_floatarg f)
{
     if (x->x_loading) return;
     x->x_symslot = NULL;
     x->x_slot = f;
     if (x->x_save) {
	  x->x_save = 0;
	  state_dosave(x);
	  return;
     }
	  
     state_load(x);
}


void state_anything(t_state *x,t_symbol* s,t_int argc,t_atom* argv)
{
     x->x_symslot = s;
     if (x->x_save) {
	  x->x_save = 0;
	  state_dosave(x);
	  return;
     }

     state_load(x);
}



void state_bang(t_state *x)
{
     if (x->x_symslot) outlet_symbol(x->x_obj.ob_outlet,x->x_symslot);
     else
	  outlet_float(x->x_obj.ob_outlet,x->x_slot);
}


static void *state_new(t_symbol* name)
{
    t_state *x = (t_state *)pd_new(state_class);
    x->x_canvas = canvas_getcurrent();
    if (name != &s_)
	 x->x_name = name;
    else
	 x->x_name = gensym("state");

    x->x_clock = clock_new(x, (t_method)state_saveoff);
    x->x_slot = 0;
    x->x_symslot = NULL;
    x->x_loading = 0;
    x->x_save = 0;
    outlet_new(&x->x_obj, &s_float);
    return (x);
}



void state_setup(void)
{
    state_class = class_new(gensym("state"), (t_newmethod)state_new, 0,
				sizeof(t_state), 0,A_DEFSYM,0);
    class_addfloat(state_class,state_float);
    class_addmethod(state_class,(t_method) state_save, gensym("save"), 0);
    class_addanything(state_class,(t_method) state_anything);
    class_addbang(state_class,state_bang);
/*    class_addmethod(state_class, (t_method)state_load, gensym("load"), 0);*/
}


