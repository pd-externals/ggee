/* (C) Guenter Geiger <geiger@epy.co.at> */


#include <m_pd.h>
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

/* ------------------------ shell ----------------------------- */

#define INBUFSIZE 1024

static t_class *shell_class;


typedef struct _shell
{
     t_object x_obj;
     int      x_echo;
     char *sr_inbuf;
     int sr_inhead;
     int sr_intail;
     void* x_binbuf;
     int fdpipe[2];
     int pid;
} t_shell;

static int shell_pid;

void child_handler(int n)
{
	int ret;
	waitpid(-1,&ret,WNOHANG);
}

void shell_bang(t_shell *x)
{
     post("bang");
}

#if 1
static void shell_doit(void *z, t_binbuf *b)
{
    t_atom messbuf[1024];
    t_shell *x = (t_shell *)z;
    int msg, natom = binbuf_getnatom(b);
    t_atom *at = binbuf_getvec(b);

    for (msg = 0; msg < natom;)
    {
    	int emsg;
	for (emsg = msg; emsg < natom && at[emsg].a_type != A_COMMA
	    && at[emsg].a_type != A_SEMI; emsg++)
	    	;
	if (emsg > msg)
	{
	    int i;
	    for (i = msg; i < emsg; i++)
	    	if (at[i].a_type == A_DOLLAR || at[i].a_type == A_DOLLSYM)
	    {
	    	pd_error(x, "netreceive: got dollar sign in message");
		goto nodice;
	    }
	    if (at[msg].a_type == A_FLOAT)
	    {
	    	if (emsg > msg + 1)
		    outlet_list(x->x_obj.ob_outlet,  0, emsg-msg, at + msg);
		else outlet_float(x->x_obj.ob_outlet,  at[msg].a_w.w_float);
	    }
	    else if (at[msg].a_type == A_SYMBOL)
	    	outlet_anything(x->x_obj.ob_outlet,  at[msg].a_w.w_symbol,
		    emsg-msg-1, at + msg + 1);
	}
    nodice:
    	msg = emsg + 1;
    }
}


void shell_read(t_shell *x, int fd)
{
     char buf[INBUFSIZE];
     t_binbuf* bbuf = binbuf_new();
     int i;
     int readto =
	  (x->sr_inhead >= x->sr_intail ? INBUFSIZE : x->sr_intail-1);
     int ret;

     ret = read(fd, buf,INBUFSIZE);
     buf[ret] = '\0';

     for (i=0;i<ret;i++)
       if (buf[i] == '\n') buf[i] = ';';
     if (ret < 0)
       {
	 error("shell: pipe read error");
	 sys_rmpollfn(fd);
	 x->fdpipe[0] = -1;
	 close(fd);
	 return;
       }
     else if (ret == 0)
       {
	 post("EOF on socket %d\n", fd);
	 sys_rmpollfn(fd);
	 x->fdpipe[0] = -1;
	 close(fd);
	 return;
       }
     else
       {
	 int natom;
	 t_atom *at;
	 binbuf_text(bbuf, buf, strlen(buf));
	 
	 natom = binbuf_getnatom(bbuf);
	 at = binbuf_getvec(bbuf);
	 shell_doit(x,bbuf);
	 
       }
     binbuf_free(bbuf);
}

#endif

static void shell_anything(t_shell *x, t_symbol *s, int ac, t_atom *at)
{
     int i;
     char* argv[20];

     argv[0] = s->s_name;

     if (x->fdpipe[0] != -1) {
	  close(x->fdpipe[0]);
	  close(x->fdpipe[1]);
	  sys_rmpollfn(x->fdpipe[0]);
	  x->fdpipe[0] = -1;
	  x->fdpipe[1] = -1;
	  kill(x->pid,SIGKILL);
     }


	  
     for (i=1;i<=ac;i++) {
	  argv[i] = atom_getsymbolarg(i-1,ac,at)->s_name;
	  /* post("argument %s",argv[i]);*/
     }
     argv[i] = 0;

     if (pipe(x->fdpipe) < 0)
	  error("unable to create pipe");

     sys_addpollfn(x->fdpipe[0],shell_read,x);

     if (!(x->pid = fork())) {
	  /* reassign stdout */
	  dup2(x->fdpipe[1],1);
	  execvp(s->s_name,argv);
	  exit(0);
     }

     if (x->x_echo)
	  outlet_anything(x->x_obj.ob_outlet, s, ac, at); 
}



void shell_free(t_shell* x)
{
    binbuf_free(x->x_binbuf);
}

static void *shell_new()
{
    t_shell *x = (t_shell *)pd_new(shell_class);

    x->x_echo = 0;
    x->fdpipe[0] = -1;
    x->fdpipe[1] = -1;

    x->sr_inhead = x->sr_intail = 0;
    if (!(x->sr_inbuf = (char*) malloc(INBUFSIZE))) bug("t_shell");;

    x->x_binbuf = binbuf_new();

    outlet_new(&x->x_obj, &s_list);
    return (x);
}

void shell_setup(void)
{
    shell_class = class_new(gensym("shell"), (t_newmethod)shell_new, 
			    (t_method)shell_free,sizeof(t_shell), 0,0);
    class_addbang(shell_class,shell_bang);
    class_addanything(shell_class, shell_anything);
    signal(SIGCHLD, child_handler);
}


