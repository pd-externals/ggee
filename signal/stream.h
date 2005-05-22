/* (C) Guenter Geiger 1999 */

#define SF_FLOAT  1
#define SF_DOUBLE 2
#define SF_8BIT   10
#define SF_16BIT  11
#define SF_32BIT  12
#define SF_ALAW   20
#define SF_MP3    30

#define SF_SIZEOF(a) (a == SF_FLOAT ? sizeof(t_float) : a == SF_16BIT ? sizeof(short) : 1)



typedef struct _tag {      /* size (bytes) */
     char version;         /*    1         */
     char format;          /*    1         */
     int count;            /*    4         */
     char channels;        /*    1         */
     int framesize;        /*    4         */
     char  extension[5];   /*    5         */
} t_tag;                   /*--------------*/
                           /*   16         */


typedef struct _frame {
     t_tag  tag;
     char*  data;
} t_frame;
