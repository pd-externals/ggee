/* define_louds_rout.c
(c) Ville Pulkki   10.11.1998 Helsinki University of Technology

functions for loudspeaker table initialization */


#include "define_loudspeakers.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

 
void angle_to_cart(ang_vec *from, cart_vec *to)
     /* from angular to cartesian coordinates*/
{
  float ang2rad = 2 * 3.141592 / 360;
  to->x= (float) (cos((double)(from->azi * ang2rad)) 
		  * cos((double) (from->ele * ang2rad)));
  to->y= (float) (sin((double)(from->azi * ang2rad)) 
		  * cos((double) (from->ele * ang2rad)));
  to->z= (float) (sin((double) (from->ele * ang2rad)));
}  


void choose_ls_triplets(ls lss[MAX_LS_AMOUNT],   
			struct ls_triplet_chain **ls_triplets, int ls_amount) 
     /* Selects the loudspeaker triplets, and
      calculates the inversion matrices for each selected triplet.
     A line (connection) is drawn between each loudspeaker. The lines
     denote the sides of the triangles. The triangles should not be 
     intersecting. All crossing connections are searched and the 
     longer connection is erased. This yields non-intesecting triangles,
     which can be used in panning.*/
{
  int i,j,k,l,m,li, table_size;
  int *i_ptr;
  cart_vec vb1,vb2,tmp_vec;
  int connections[MAX_LS_AMOUNT][MAX_LS_AMOUNT];
  float angles[MAX_LS_AMOUNT];
  int sorted_angles[MAX_LS_AMOUNT];
  float distance_table[((MAX_LS_AMOUNT * (MAX_LS_AMOUNT - 1)) / 2)];
  int distance_table_i[((MAX_LS_AMOUNT * (MAX_LS_AMOUNT - 1)) / 2)];
  int distance_table_j[((MAX_LS_AMOUNT * (MAX_LS_AMOUNT - 1)) / 2)];
  float distance;
  struct ls_triplet_chain *trip_ptr, *prev, *tmp_ptr;

  if (ls_amount == 0) {
    fprintf(stderr,"Number of loudspeakers is zero\nExiting\n");
    exit(-1);
  }
  for(i=0;i<ls_amount;i++)
    for(j=i+1;j<ls_amount;j++)
      for(k=j+1;k<ls_amount;k++){
	if(vol_p_side_lgth(i,j, k, lss) > MIN_VOL_P_SIDE_LGTH){
	  connections[i][j]=1;
	  connections[j][i]=1;
	  connections[i][k]=1;
	  connections[k][i]=1;
	  connections[j][k]=1;
	  connections[k][j]=1;
	  add_ldsp_triplet(i,j,k,ls_triplets, lss);
	}
      }
  /*calculate distancies between all lss and sorting them*/
  table_size =(((ls_amount - 1) * (ls_amount)) / 2); 
  for(i=0;i<table_size; i++)
    distance_table[i] = 100000.0;
  for(i=0;i<ls_amount;i++){ 
    for(j=(i+1);j<ls_amount; j++){ 
      if(connections[i][j] == 1) {
	distance = fabs(vec_angle(lss[i].coords,lss[j].coords));
	k=0;
	while(distance_table[k] < distance)
	  k++;
	for(l=(table_size - 1);l > k ;l--){
	  distance_table[l] = distance_table[l-1];
	  distance_table_i[l] = distance_table_i[l-1];
	  distance_table_j[l] = distance_table_j[l-1];
	}
	distance_table[k] = distance;
	distance_table_i[k] = i;
	distance_table_j[k] = j;
      } else
	table_size--;
    }
  }

  /* disconnecting connections which are crossing shorter ones,
     starting from shortest one and removing all that cross it,
     and proceeding to next shortest */
  for(i=0; i<(table_size); i++){
    int fst_ls = distance_table_i[i];
    int sec_ls = distance_table_j[i];
    if(connections[fst_ls][sec_ls] == 1)
      for(j=0; j<ls_amount ; j++)
	for(k=j+1; k<ls_amount; k++)
	  if( (j!=fst_ls) && (k != sec_ls) && (k!=fst_ls) && (j != sec_ls)){
	    if(lines_intersect(fst_ls, sec_ls, j,k,lss) == 1){
	      connections[j][k] = 0;
	      connections[k][j] = 0;
	    }
	  }
  }

  /* remove triangles which had crossing sides
     with smaller triangles or include loudspeakers*/
  trip_ptr = *ls_triplets;
  prev = NULL;
  while (trip_ptr != NULL){
    i = trip_ptr->ls_nos[0];
    j = trip_ptr->ls_nos[1];
    k = trip_ptr->ls_nos[2];
    if(connections[i][j] == 0 || 
       connections[i][k] == 0 || 
       connections[j][k] == 0 ||
       any_ls_inside_triplet(i,j,k,lss,ls_amount) == 1 ){
      if(prev != NULL) {
	prev->next = trip_ptr->next;
	tmp_ptr = trip_ptr;
	trip_ptr = trip_ptr->next;
	free(tmp_ptr);
      } else {
	*ls_triplets = trip_ptr->next;
	tmp_ptr = trip_ptr;
	trip_ptr = trip_ptr->next;
	free(tmp_ptr);
      }
    } else {
      prev = trip_ptr;
      trip_ptr = trip_ptr->next;

    }
  }
}


int any_ls_inside_triplet(int a, int b, int c,ls lss[MAX_LS_AMOUNT],int ls_amount)
     /* returns 1 if there is loudspeaker(s) inside given ls triplet */
{
  float invdet;
  cart_vec *lp1, *lp2, *lp3;
  float invmx[9];
  int i,j,k;
  float tmp;
  int any_ls_inside, this_inside;

  lp1 =  &(lss[a].coords);
  lp2 =  &(lss[b].coords);
  lp3 =  &(lss[c].coords);

  /* matrix inversion */
  invdet = 1.0 / (  lp1->x * ((lp2->y * lp3->z) - (lp2->z * lp3->y))
		    - lp1->y * ((lp2->x * lp3->z) - (lp2->z * lp3->x))
		    + lp1->z * ((lp2->x * lp3->y) - (lp2->y * lp3->x)));
  
  invmx[0] = ((lp2->y * lp3->z) - (lp2->z * lp3->y)) * invdet;
  invmx[3] = ((lp1->y * lp3->z) - (lp1->z * lp3->y)) * -invdet;
  invmx[6] = ((lp1->y * lp2->z) - (lp1->z * lp2->y)) * invdet;
  invmx[1] = ((lp2->x * lp3->z) - (lp2->z * lp3->x)) * -invdet;
  invmx[4] = ((lp1->x * lp3->z) - (lp1->z * lp3->x)) * invdet;
  invmx[7] = ((lp1->x * lp2->z) - (lp1->z * lp2->x)) * -invdet;
  invmx[2] = ((lp2->x * lp3->y) - (lp2->y * lp3->x)) * invdet;
  invmx[5] = ((lp1->x * lp3->y) - (lp1->y * lp3->x)) * -invdet;
  invmx[8] = ((lp1->x * lp2->y) - (lp1->y * lp2->x)) * invdet;

  any_ls_inside = 0;
  for(i=0; i< ls_amount; i++) {
    if (i != a && i!=b && i != c){
      this_inside = 1;
      for(j=0; j< 3; j++){
	tmp = lss[i].coords.x * invmx[0 + j*3];
	tmp += lss[i].coords.y * invmx[1 + j*3];
	tmp += lss[i].coords.z * invmx[2 + j*3];
	if(tmp < -0.001)
	  this_inside = 0;
      }
      if(this_inside == 1)
	any_ls_inside=1;
    }
  }
  return any_ls_inside;
}


void add_ldsp_triplet(int i, int j, int k, 
		       struct ls_triplet_chain **ls_triplets,
		       ls lss[MAX_LS_AMOUNT])
     /* adds i,j,k triplet to triplet chain*/
{
  struct ls_triplet_chain *trip_ptr, *prev;
  trip_ptr = *ls_triplets;
  prev = NULL;

  while (trip_ptr != NULL){
    prev = trip_ptr;
    trip_ptr = trip_ptr->next;
  }
  trip_ptr = (struct ls_triplet_chain*) 
    malloc (sizeof (struct ls_triplet_chain));
  if(prev == NULL)
    *ls_triplets = trip_ptr;
  else 
    prev->next = trip_ptr;
  trip_ptr->next = NULL;
  trip_ptr->ls_nos[0] = i;
  trip_ptr->ls_nos[1] = j;
  trip_ptr->ls_nos[2] = k;
}




float vec_angle(cart_vec v1, cart_vec v2)
{
  float inner= ((v1.x*v2.x + v1.y*v2.y + v1.z*v2.z)/
	      (vec_length(v1) * vec_length(v2)));
  if(inner > 1.0)
    inner= 1.0;
  if (inner < -1.0)
    inner = -1.0;
  return fabsf((float) acos((double) inner));
}

float vec_length(cart_vec v1)
{
  return (sqrt(v1.x*v1.x + v1.y*v1.y + v1.z*v1.z));
}

float vec_prod(cart_vec v1, cart_vec v2)
{
  return (v1.x*v2.x + v1.y*v2.y + v1.z*v2.z);
}


float vol_p_side_lgth(int i, int j,int k, ls  lss[MAX_LS_AMOUNT] ){
  /* calculate volume of the parallelepiped defined by the loudspeaker
     direction vectors and divide it with total length of the triangle sides. 
     This is used when removing too narrow triangles. */

  float volper, lgth;
  cart_vec xprod;
  cross_prod(lss[i].coords, lss[j].coords, &xprod);
  volper = fabsf(vec_prod(xprod, lss[k].coords));
  lgth = (fabsf(vec_angle(lss[i].coords,lss[j].coords)) 
	  + fabsf(vec_angle(lss[i].coords,lss[k].coords)) 
	  + fabsf(vec_angle(lss[j].coords,lss[k].coords)));
  if(lgth>0.00001)
    return volper / lgth;
  else
    return 0.0;
}

void cross_prod(cart_vec v1,cart_vec v2, 
		cart_vec *res) 
{
  float length;
  res->x = (v1.y * v2.z ) - (v1.z * v2.y);
  res->y = (v1.z * v2.x ) - (v1.x * v2.z);
  res->z = (v1.x * v2.y ) - (v1.y * v2.x);

  length= vec_length(*res);
  res->x /= length;
  res->y /= length;
  res->z /= length;
}


int lines_intersect(int i,int j,int k,int l,ls  lss[MAX_LS_AMOUNT])
     /* checks if two lines intersect on 3D sphere 
      see theory in paper Pulkki, V. Lokki, T. "Creating Auditory Displays
      with Multiple Loudspeakers Using VBAP: A Case Study with
      DIVA Project" in International Conference on 
      Auditory Displays -98. E-mail Ville.Pulkki@hut.fi
     if you want to have that paper.*/
{
  cart_vec v1;
  cart_vec v2;
  cart_vec v3, neg_v3;
  float angle;
  float dist_ij,dist_kl,dist_iv3,dist_jv3,dist_inv3,dist_jnv3;
  float dist_kv3,dist_lv3,dist_knv3,dist_lnv3;

  cross_prod(lss[i].coords,lss[j].coords,&v1);
  cross_prod(lss[k].coords,lss[l].coords,&v2);
  cross_prod(v1,v2,&v3);

  neg_v3.x= 0.0 - v3.x;
  neg_v3.y= 0.0 - v3.y;
  neg_v3.z= 0.0 - v3.z;

  dist_ij = (vec_angle(lss[i].coords,lss[j].coords));
  dist_kl = (vec_angle(lss[k].coords,lss[l].coords));
  dist_iv3 = (vec_angle(lss[i].coords,v3));
  dist_jv3 = (vec_angle(v3,lss[j].coords));
  dist_inv3 = (vec_angle(lss[i].coords,neg_v3));
  dist_jnv3 = (vec_angle(neg_v3,lss[j].coords));
  dist_kv3 = (vec_angle(lss[k].coords,v3));
  dist_lv3 = (vec_angle(v3,lss[l].coords));
  dist_knv3 = (vec_angle(lss[k].coords,neg_v3));
  dist_lnv3 = (vec_angle(neg_v3,lss[l].coords));

  /* if one of loudspeakers is close to crossing point, don't do anything*/


  if(fabsf(dist_iv3) <= 0.01 || fabsf(dist_jv3) <= 0.01 || 
  fabsf(dist_kv3) <= 0.01 || fabsf(dist_lv3) <= 0.01 ||
     fabsf(dist_inv3) <= 0.01 || fabsf(dist_jnv3) <= 0.01 || 
     fabsf(dist_knv3) <= 0.01 || fabsf(dist_lnv3) <= 0.01 )
    return(0);


 
  if (((fabsf(dist_ij - (dist_iv3 + dist_jv3)) <= 0.01 ) &&
       (fabsf(dist_kl - (dist_kv3 + dist_lv3))  <= 0.01)) ||
      ((fabsf(dist_ij - (dist_inv3 + dist_jnv3)) <= 0.01)  &&
       (fabsf(dist_kl - (dist_knv3 + dist_lnv3)) <= 0.01 ))) {
    return (1);
  } else {
    return (0);
  }
}



void  calculate_3x3_matrixes(struct ls_triplet_chain *ls_triplets, 
			 ls lss[MAX_LS_AMOUNT], int ls_amount)
     /* Calculates the inverse matrices for 3D */
{  
  float invdet;
  cart_vec *lp1, *lp2, *lp3;
  float *invmx;
  float *ptr;
  struct ls_triplet_chain *tr_ptr = ls_triplets;
  int triplet_amount = 0, ftable_size,i,j,k;
  float *ls_table;

  if (tr_ptr == NULL){
    fprintf(stderr,"Not valid 3-D configuration\n");
    exit(-1);
  }

  /* counting triplet amount */
  while(tr_ptr != NULL){
    triplet_amount++;
    tr_ptr = tr_ptr->next;
  }

  /* calculations and data storage to a global array */
  ls_table = (float *) malloc( (triplet_amount*12 + 3) * sizeof (float));
  ls_table[0] = 3.0; /*dimension*/
  ls_table[1] = (float) ls_amount;
  ls_table[2] = (float) triplet_amount;
  tr_ptr = ls_triplets;
  ptr = (float *) &(ls_table[3]);
  while(tr_ptr != NULL){
    lp1 =  &(lss[tr_ptr->ls_nos[0]].coords);
    lp2 =  &(lss[tr_ptr->ls_nos[1]].coords);
    lp3 =  &(lss[tr_ptr->ls_nos[2]].coords);

    /* matrix inversion */
    invmx = tr_ptr->inv_mx;
    invdet = 1.0 / (  lp1->x * ((lp2->y * lp3->z) - (lp2->z * lp3->y))
		    - lp1->y * ((lp2->x * lp3->z) - (lp2->z * lp3->x))
		    + lp1->z * ((lp2->x * lp3->y) - (lp2->y * lp3->x)));

    invmx[0] = ((lp2->y * lp3->z) - (lp2->z * lp3->y)) * invdet;
    invmx[3] = ((lp1->y * lp3->z) - (lp1->z * lp3->y)) * -invdet;
    invmx[6] = ((lp1->y * lp2->z) - (lp1->z * lp2->y)) * invdet;
    invmx[1] = ((lp2->x * lp3->z) - (lp2->z * lp3->x)) * -invdet;
    invmx[4] = ((lp1->x * lp3->z) - (lp1->z * lp3->x)) * invdet;
    invmx[7] = ((lp1->x * lp2->z) - (lp1->z * lp2->x)) * -invdet;
    invmx[2] = ((lp2->x * lp3->y) - (lp2->y * lp3->x)) * invdet;
    invmx[5] = ((lp1->x * lp3->y) - (lp1->y * lp3->x)) * -invdet;
    invmx[8] = ((lp1->x * lp2->y) - (lp1->y * lp2->x)) * invdet;
    for(i=0;i<3;i++){
      *(ptr++) = (float) tr_ptr->ls_nos[i]+1;
    }
    for(i=0;i<9;i++){
      *(ptr++) = (float) invmx[i];
    }
    tr_ptr = tr_ptr->next;
  }

  k=3;
  printf("Configured %d sets in 3 dimensions:\n", triplet_amount);
  for(i=0 ; i < triplet_amount ; i++) {
    printf("Triplet %d Loudspeakers: ", i);
    for (j=0 ; j < 3 ; j++) {
      printf("%d ", (int) ls_table[k++]);
    }
    printf(" Matrix ");
    for (j=0 ; j < 9; j++) {
      printf("%f ", ls_table[k]);
      k++;
    }
    printf("\n");
  }
}



void choose_ls_tuplets( ls lss[MAX_LS_AMOUNT],
			ls_triplet_chain **ls_triplets,
			int ls_amount)
     /* selects the loudspeaker pairs, calculates the inversion
	matrices and stores the data to a global array*/
{
  float atorad = (2 * 3.1415927 / 360) ;
  int i,j,k;
  float w1,w2;
  float p1,p2;
  int sorted_lss[MAX_LS_AMOUNT];
  int exist[MAX_LS_AMOUNT];
  int amount=0;
  float inv_mat[MAX_LS_AMOUNT][4], *ptr;
  float *ls_table;
  
  for(i=0;i<MAX_LS_AMOUNT;i++){
    exist[i]=0;
  }

  /* sort loudspeakers according their aximuth angle */
  sort_2D_lss(lss,sorted_lss,ls_amount);

  /* adjacent loudspeakers are the loudspeaker pairs to be used.*/
  for(i=0;i<(ls_amount-1);i++){
    if((lss[sorted_lss[i+1]].angles.azi - 
	lss[sorted_lss[i]].angles.azi) <= (3.1415927 - 0.175)){
      if (calc_2D_inv_tmatrix( lss[sorted_lss[i]].angles.azi, 
			       lss[sorted_lss[i+1]].angles.azi, 
			       inv_mat[i]) != 0){
	exist[i]=1;
	amount++;
      }
    }
  }

  if(((6.283 - lss[sorted_lss[ls_amount-1]].angles.azi) 
      +lss[sorted_lss[0]].angles.azi) <= (3.1415927 - 0.175)) {
    if(calc_2D_inv_tmatrix(lss[sorted_lss[ls_amount-1]].angles.azi, 
			   lss[sorted_lss[0]].angles.azi, 
			   inv_mat[ls_amount-1]) != 0) { 
      	exist[ls_amount-1]=1;
	amount++;
    } 
  }
  ls_table = (float*) malloc ((amount * 6 + 3 + 100 ) * sizeof (float));
  ls_table[0] = 2.0; /*dimension*/
  ls_table[1] = (float) ls_amount;
  ls_table[2] = (float) amount;
  ptr = &(ls_table[3]);
  for (i=0;i<ls_amount - 1;i++){
    if(exist[i] == 1) {
      *(ptr++) = sorted_lss[i]+1;
      *(ptr++) = sorted_lss[i+1]+1;
      for(j=0;j<4;j++) {
        *(ptr++) = inv_mat[i][j];
      }
    }
  }

  if(exist[ls_amount-1] == 1) {
    *(ptr++) = sorted_lss[ls_amount-1]+1;
    *(ptr++) = sorted_lss[0]+1;
    for(j=0;j<4;j++) {
      *(ptr++) = inv_mat[ls_amount-1][j];
    }
  }
  k=3;
  printf("Configured %d pairs in 2 dimensions:\n",amount);
  for(i=0 ; i < amount ; i++) {
    printf("Pair %d Loudspeakers: ", i);
    for (j=0 ; j < 2 ; j++) {
      printf("%d ", (int) ls_table[k++]);
    }
    printf(" Matrix ");
    for (j=0 ; j < 4; j++) {
      printf("%f ", ls_table[k]);
      k++;
    }
    printf("\n");
  }
}

void sort_2D_lss(ls lss[MAX_LS_AMOUNT], int sorted_lss[MAX_LS_AMOUNT], 
		 int ls_amount)
{
  int i,j,index;
  float tmp, tmp_azi;
  float rad2ang = 360.0 / ( 2 * 3.141592 );

  float x,y;
  /* Transforming angles between -180 and 180 */
  for (i=0;i<ls_amount;i++) {
    angle_to_cart(&lss[i].angles, &lss[i].coords);
    lss[i].angles.azi = (float) acos((double) lss[i].coords.x);
    if (fabsf(lss[i].coords.y) <= 0.001)
    	tmp = 1.0;
    else
    	tmp = lss[i].coords.y / fabsf(lss[i].coords.y);
    lss[i].angles.azi *= tmp;
  }
  for (i=0;i<ls_amount;i++){
    tmp = 2000;
    for (j=0 ; j<ls_amount;j++){
      if (lss[j].angles.azi <= tmp){
	tmp=lss[j].angles.azi;
	index = j ;
      }
    }
    sorted_lss[i]=index;
    tmp_azi = (lss[index].angles.azi);
    lss[index].angles.azi = (tmp_azi + (float) 4000.0);
  }
  for (i=0;i<ls_amount;i++) {
    tmp_azi = (lss[i].angles.azi);
    lss[i].angles.azi = (tmp_azi - (float) 4000.0);
  }
}
  

int calc_2D_inv_tmatrix(float azi1,float azi2, float inv_mat[4])
{
  float x1,x2,x3,x4; /* x1 x3 */
  float y1,y2,y3,y4; /* x2 x4 */
  float det;
  x1 = (float) cos((double) azi1 );
  x2 = (float) sin((double) azi1 );
  x3 = (float) cos((double) azi2 );
  x4 = (float) sin((double) azi2 );
  det = (x1 * x4) - ( x3 * x2 );
   if(fabsf(det) <= 0.001) {

    inv_mat[0] = 0.0;
    inv_mat[1] = 0.0;
    inv_mat[2] = 0.0;
    inv_mat[3] = 0.0;
    return 0;
  } else {
    inv_mat[0] =  (float) (x4 / det);
    inv_mat[1] =  (float) (-x3 / det);
    inv_mat[2] =  (float) (-x2 / det);
    inv_mat[3] =  (float)  (x1 / det);
    return 1;
  }
}


