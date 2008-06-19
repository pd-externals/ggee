/* define_loudspeakers.c 0.1
(c) Ville Pulkki   2.2.1999 Helsinki University of Technology*/

#include <stdio.h>
#include "define_loudspeakers.h"


void load_ls_triplets(ls lss[MAX_LS_AMOUNT],   
		      struct ls_triplet_chain **ls_triplets, 
		      int ls_amount, char *filename) 
{
  struct ls_triplet_chain *trip_ptr, *prev;
  int i,j,k;
  FILE *fp;
  char c[10000];
  char *toke;

  trip_ptr = *ls_triplets;
  prev = NULL;
  while (trip_ptr != NULL){
    prev = trip_ptr;
    trip_ptr = trip_ptr->next;
  }

  if((fp=fopen(filename,"r")) == NULL){
    fprintf(stderr,"Could not open loudspeaker setup file.\n");
    exit(-1);
  }

  while(1) {
    if(fgets(c,10000,fp) == NULL)
      break;
    toke = (char *) strtok(c, " ");
    if(sscanf(toke, "%d",&i)>0){
      toke = (char *) strtok(NULL," ");
      sscanf(toke, "%d",&j);
      toke = (char *) strtok(NULL," ");
      sscanf(toke, "%d",&k);
    } else {
      break;
    }

    trip_ptr = (struct ls_triplet_chain*) 
      malloc (sizeof (struct ls_triplet_chain));
    
    if(prev == NULL)
      *ls_triplets = trip_ptr;
    else 
      prev->next = trip_ptr;
    
    trip_ptr->next = NULL;
    trip_ptr->ls_nos[0] = i-1;
    trip_ptr->ls_nos[1] = j-1;
    trip_ptr->ls_nos[2] = k-1;
    prev=trip_ptr;
    trip_ptr=NULL;
  }
}



main(int argc,char **argv)
     /* Inits the loudspeaker data. Calls choose_ls_tuplets or _triplets
	according to current dimension. The inversion matrices are 
     stored in transposed form to ease calculation at run time.*/
{
  char *s;
  int dim;
  float tmp;
  struct ls_triplet_chain *ls_triplets = NULL;
  ls lss[MAX_LS_AMOUNT];
  char c[10000];  
  char *toke;  
  ang_vec a_vector;
  cart_vec c_vector;
  int i=0,j;
  float azi, ele;
  int ls_amount;
  FILE *fp;

  if(argc != 2 && argc != 3){
    fprintf(stderr,"Usage: define_loudspeakers loudspeaker_directions_file [loudspeaker_triplet_file]\n");
    exit(-1);
  }

  if((fp=fopen(argv[1],"r")) == NULL){
    fprintf(stderr,"Could not open loudspeaker setup file.%s\n",argv[1]);
    exit(-1);
  }


  fgets(c,10000,fp);
  toke = (char *) strtok(c, " ");
  sscanf(toke, "%d",&dim);
  if (!((dim==2) || (dim == 3))){
    fprintf(stderr,"Error in loudspeaker dimension.\n");
    exit (-1);
  }
  printf("File: %s ",argv[1]);
  while(1) {
    if(fgets(c,10000,fp) == NULL)
      break;
    toke = (char *) strtok(c, " ");
    if(sscanf(toke, "%f",&azi)>0){
      if(dim == 3) {
	toke = (char *) strtok(NULL," ");
	sscanf(toke, "%f",&ele);
      } else if(dim == 2) {
	ele=0.0;
      }
    } else {
      break;
    }

    a_vector.azi = azi;
    a_vector.ele = ele;
    angle_to_cart(&a_vector,&c_vector);
    lss[i].coords.x = c_vector.x;
    lss[i].coords.y = c_vector.y;
    lss[i].coords.z = c_vector.z;
    lss[i].angles.azi = a_vector.azi;
    lss[i].angles.ele = a_vector.ele;
    lss[i].angles.length = 1.0;
    i++;
  }
  ls_amount = i;
  if(ls_amount < dim) {
    fprintf(stderr,"Too few loudspeakers %d\n",ls_amount);
    exit (-1);
  }

  if(dim == 3){
    if(argc==2) /* select triplets */
      choose_ls_triplets(lss, &ls_triplets,ls_amount);
    else /* load triplets from a file */
      load_ls_triplets(lss, &ls_triplets,ls_amount,argv[2]);
    calculate_3x3_matrixes(ls_triplets,lss,ls_amount);
  } else if (dim ==2) {
    choose_ls_tuplets(lss, &ls_triplets,ls_amount); 
  }
}

