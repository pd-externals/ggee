/* (C) Guenter Geiger <geiger@epy.co.at> */


#include <m_imp.h>
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>


#ifndef __linux__
#define CRTSCTS 0
#endif

#define DEBUG(x) 
/*#define DEBUG(x) x*/


#define SETBIT(yy,ww,xx) \
if (!strcmp(yy->s_name,#xx)) \
     valid=1,ww |= ##xx;\
if (!strcmp(yy->s_name,"~"#xx))\
     valid=1,ww &= ~##xx;\
if (!strcmp(yy->s_name,"list"))\
    valid=1,post(#xx);
     

/* ------------------------ serialctl ----------------------------- */

static t_class *serialctl_class;


typedef struct _serialctl
{
     t_object   x_obj;
     t_int      x_fd;
     t_symbol*  x_devname;
     int        read_ok;
     int        started;
     int        numbytes;
     struct termios      x_termios;
     struct termios      x_oldtermios;
} t_serialctl;



static int serialctl_close(t_serialctl *x)
{
     if (x->x_fd <0) return 0;

     close(x->x_fd);
     return 1;
}

static int serialctl_open(t_serialctl *x,t_symbol* s)
{
    serialctl_close(x);

    if (s != &s_)
	 x->x_devname = s;
    
    if (x->x_devname) {
	 post("opening ...");
	 x->x_fd = open(x->x_devname->s_name,O_RDWR | O_NOCTTY);
         if (x->x_fd >= 0 ) post("done");
         else post("failed");
    }
    else {
	 return 1;
    }

    if (x->x_fd >= 0)
	 post("%s opened",x->x_devname->s_name);
    else {
	 post("unable to open %s",x->x_devname->s_name);
	 x->x_fd = -1;
	 return 0;
    }

    return 1;
}



static int serialctl_read(t_serialctl *x,int fd)
{
     unsigned char  c;
     if (x->x_fd < 0) return 0;
     if (x->read_ok) {
       DEBUG(post("reading %d",x->numbytes);)
         if (read(x->x_fd,&c,1) < 0) {
	       post("serialctl: read failed");
               x->read_ok = 0;
	       return 0;
         }
         x->numbytes++;
     }
     outlet_float(x->x_obj.ob_outlet,(float)c);
     
     return 1;
     
}


/*
 * Configuration Options 
 */

static void serialctl_setlines(t_serialctl *x,t_symbol* s,t_floatarg f)
{
     int lines;

     ioctl(x->x_fd,TIOCMGET,&lines);
     if (!strcmp(s->s_name,"RTS")) {
	  if (f)
	       lines |= TIOCM_RTS;
	  else
	       lines &= ~TIOCM_RTS;
     }
     if (!strcmp(s->s_name,"DTR")) {
	  if (f)
	       lines |= TIOCM_DTR;
	  else
	       lines &= ~TIOCM_DTR;
     }



     ioctl(x->x_fd,TIOCMSET,&lines);

}


static void serialctl_getlines(t_serialctl *x)
{
     int lines;
    /* check hardware signals */

    ioctl(x->x_fd,TIOCMGET,&lines);
    post("serialctl:  CD %s",lines&TIOCM_CD ? "on":"off");
    post("serialctl:  DTR %s",lines&TIOCM_DTR ? "on":"off");
    post("serialctl:  DSR %s",lines&TIOCM_DSR ? "on":"off");
    post("serialctl:  RTS %s",lines&TIOCM_RTS ? "on":"off");
    post("serialctl:  CTS %s",lines&TIOCM_CTS ? "on":"off");

}


static void serialctl_setlocal(t_serialctl *x,t_symbol* s,t_int ac, t_atom* at)
{
     int valid =0;
     int i;


     for (i=0;i< ac;i++) {
       valid = 0;

       if (at[i].a_type == A_FLOAT) {
	 x->x_termios.c_lflag = (int)atom_getfloat(at+i);
	 DEBUG(post("localflags set to %d",(int)atom_getfloat(at+i));)
	 valid = 1;
       }

       if (at[i].a_type == A_SYMBOL) {
	 DEBUG(post("symbol %s",atom_getsymbolarg(i,ac,at)->s_name);)
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_lflag, ISIG);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_lflag, ICANON);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_lflag, ECHO);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_lflag, ECHOE);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_lflag, ECHOK);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_lflag, ECHONL);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_lflag, NOFLSH);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_lflag, TOSTOP);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_lflag, IEXTEN);

       if (!valid) post("serialctl: invalide flag %s",atom_getsymbolarg(i,ac,at)->s_name);
       }
     }

      if (x->x_fd < 0) return;

      if (tcsetattr(x->x_fd, TCSANOW,&x->x_termios) < 0)
	post("serialctl: set attributes failed");
      
}

static void serialctl_setcontrol(t_serialctl *x,t_symbol* s,t_int ac,t_atom* at)
{
     int valid =0;
     int i;


     for (i=0;i< ac;i++) {
       valid = 0;

       if (at[i].a_type == A_FLOAT) {
	   x->x_termios.c_cflag = (int) atom_getfloat(at+i);
           DEBUG(post("controlflags set to %d",(int)atom_getfloat(at+i)));
	   valid = 1;
	 }

       if (at[i].a_type == A_SYMBOL) {
	 DEBUG(post("symbol %s",atom_getsymbolarg(i,ac,at)->s_name);)
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_cflag,PARENB);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_cflag,PARODD);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_cflag,CS5);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_cflag,CS6);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_cflag,CS7);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_cflag,CS8);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_cflag,CLOCAL);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_cflag,CREAD);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_cflag,CSTOPB);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_cflag,CRTSCTS);
	 if (!valid) post("serialctl: invalide flag %s",atom_getsymbolarg(i,ac,at)->s_name);
       }
     }

      if (x->x_fd < 0) return;
    
 
      if (tcsetattr(x->x_fd, TCSANOW,&x->x_termios) < 0)
	post("serialctl: set attributes failed");

}


static void serialctl_setinput(t_serialctl *x,t_symbol* s,t_int ac,t_atom* at)
{
     int valid =0;
     int i;


     for (i=0;i< ac;i++) {
       valid = 0;

       if (at[i].a_type == A_FLOAT) {
	 x->x_termios.c_iflag = (int)atom_getfloat(at+i);
	 DEBUG(post("inputflags set to %d",(int)atom_getfloat(at+i)));
	 valid = 1;
       }

       if (at[i].a_type == A_SYMBOL) {
	 DEBUG(post("symbol %s",atom_getsymbolarg(i,ac,at)->s_name);)
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, IGNBRK);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, BRKINT);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, IGNPAR);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, PARMRK);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, INPCK);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, ISTRIP);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, INLCR);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, IGNCR);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, ICRNL);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, IUCLC);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, IXON);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, IXANY);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, IXOFF);
	 SETBIT(atom_getsymbolarg(i,ac,at),x->x_termios.c_iflag, IMAXBEL);

       if (!valid) post("serialctl: invalide flag %s",atom_getsymbolarg(i,ac,at)->s_name);
       }
     }

      if (x->x_fd < 0) return;
    
 
      if (tcsetattr(x->x_fd, TCSANOW,&x->x_termios) < 0)
	post("serialctl: set attributes failed");

}

static void serialctl_makeraw(t_serialctl *x,t_floatarg f)
{
    cfmakeraw(&x->x_termios);
    if (tcsetattr(x->x_fd, TCSANOW,&x->x_termios) < 0)
       post("serialctl: set attributes failed");     
}


static void serialctl_vmintime(t_serialctl *x,t_floatarg f,t_floatarg ft)
{

  post("vmin = %f, vtime = %f",f,ft);
  x->x_termios.c_cc[VMIN] = (int) f;
  x->x_termios.c_cc[VTIME] = (int) ft;
  if (tcsetattr(x->x_fd, TCSANOW,&x->x_termios) < 0)
    post("serialctl: set attributes failed");

}


static void serialctl_canonical(t_serialctl *x,t_floatarg f)
{
     post("setting canonical");
     x->x_termios.c_iflag |= IGNBRK | IGNPAR;
     x->x_termios.c_cflag |= CS8 | CREAD | PARENB| CLOCAL | CRTSCTS;
     x->x_termios.c_lflag = 0;
}


static void serialctl_speed(t_serialctl *x,t_floatarg f)
{

     int speed = f;
     post("setting speed %f",f);

     cfsetospeed (&x->x_termios,speed);
     cfsetispeed (&x->x_termios,speed);
         if (x->x_fd < 0) return;

     if (tcsetattr(x->x_fd, TCSANOW,&x->x_termios) < 0)
	  post("serialctl: set attributes failed");
}


/* Actions */
static void serialctl_bang(t_serialctl* x)
{
   
}

static void serialctl_float(t_serialctl* x,t_float f)
{
     int ret = -1;
     unsigned char c = (unsigned char) f; 
     DEBUG(post("serialctl: sending %d",c));
     if (f < 256.) {
	  ret = write(x->x_fd,&c,1);
	  DEBUG(post("done"));
	  if (ret != 1)
	       post("serialctl: send %f %s",f,((ret < 0) ?  strerror(ret) : "ok"));
	  ioctl(x->x_fd,TCFLSH,TCIOFLUSH); 
	  return;
     }
     post("unable to send char < 256");

}

static void serialctl_send(t_serialctl* x,t_symbol* s)
{
    int ret;
    if (s == &s_) return;
    ret = write(x->x_fd,s->s_name,sizeof(s->s_name));
    post("serialctl: send %s %s",s->s_name,((ret < 0) ?  "not ok" : "ok"));
}


  

void serialctl_start(t_serialctl* x)
{
    if (x->x_fd >= 0 && !x->started) {
       sys_addpollfn(x->x_fd, (t_fdpollfn)serialctl_read, x);
       post("serialctl: start");
       x->started = 1;
    }
}


void serialctl_stop(t_serialctl* x)
{
    if (x->x_fd >= 0 && x->started) { 
        sys_rmpollfn(x->x_fd);
        post("serialctl: stop");
        x->started = 0;
    }
}


/* Misc setup functions */


static void serialctl_free(t_serialctl* x)
{
     if (x->x_fd < 0) return;

     serialctl_stop(x);

     if (tcsetattr(x->x_fd, TCSANOW,&x->x_oldtermios) < 0)
	  post("serialctl: set attributes failed");


     close(x->x_fd);
}



static void *serialctl_new(t_symbol *s)
{
    t_serialctl *x = (t_serialctl *)pd_new(serialctl_class);

    x->x_fd = -1;
    x->read_ok = 1;
    x->numbytes = 0;
    x->started = 0;
    outlet_new(&x->x_obj, &s_float);
    if (s != &s_)
       x->x_devname = s;
    
    /* Open the device and save settings */

    if (!serialctl_open(x,s)) return x;
    
    if (tcgetattr(x->x_fd,&x->x_oldtermios) < 0)
	 post("serialctl: get attributes failed");

    if (tcgetattr(x->x_fd,&x->x_termios) < 0)
         post("serialctl: get attributes failed");


    tcflush(x->x_fd, TCIOFLUSH);
    return (x);
}


void serialctl_setup(void)
{
    serialctl_class = class_new(gensym("serialctl"), (t_newmethod)serialctl_new, 
			     (t_method)serialctl_free,
			     sizeof(t_serialctl), 0,A_DEFSYM,0);

    class_addmethod(serialctl_class,(t_method) serialctl_start,gensym("start"),0);
    class_addmethod(serialctl_class,(t_method) serialctl_stop,gensym("stop"),0);
    class_addmethod(serialctl_class, (t_method) serialctl_open,gensym("open"),A_DEFSYM);
   class_addmethod(serialctl_class,(t_method) serialctl_close,gensym("close"),0);

   /* used to configure the device via symbols or with bytes */

   class_addmethod(serialctl_class, (t_method) serialctl_send, gensym("send"), A_DEFSYM,A_NULL);


    class_addfloat(serialctl_class,(t_method) serialctl_float);

   class_addmethod(serialctl_class,(t_method) serialctl_getlines,gensym("getlines"),0);
   class_addmethod(serialctl_class,(t_method) serialctl_setlines,gensym("setlines"),A_DEFSYM,A_DEFFLOAT,NULL);



    /* general controlling */

    class_addmethod(serialctl_class,(t_method) serialctl_setcontrol,gensym("setcontrol"),A_GIMME,NULL);
    class_addmethod(serialctl_class,(t_method) serialctl_setlocal,gensym("setlocal"),A_GIMME,NULL);
    class_addmethod(serialctl_class,(t_method) serialctl_setinput,gensym("setinput"),A_GIMME,NULL);


    /* specifics */
    
    class_addmethod(serialctl_class,(t_method) serialctl_canonical,gensym("canonical"),A_DEFFLOAT);
    class_addmethod(serialctl_class,(t_method) serialctl_speed,gensym("speed"),A_DEFFLOAT);
    class_addmethod(serialctl_class,(t_method) serialctl_makeraw,gensym("makeraw"),0);
    class_addmethod(serialctl_class,(t_method) serialctl_vmintime,gensym("vtime"),A_DEFFLOAT,A_DEFFLOAT,NULL);

}



