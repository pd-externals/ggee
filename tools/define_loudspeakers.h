#define MAX_LS_AMOUNT 32
#define MIN_VOL_P_SIDE_LGTH 0.01

typedef struct {
  float x;
  float y;
  float z;
} cart_vec;


typedef struct {
  float azi;
  float ele;
  float length;
} ang_vec;


/* A struct for a loudspeaker triplet or pair (set) */
typedef struct {
  int ls_nos[3];
  float ls_mx[9];
  float set_weights[3];
  float smallest_wt;
} LS_SET;


/* A struct for a loudspeaker instance */
typedef struct { 
  cart_vec coords;
  ang_vec angles;
  int channel_nbr;
} ls;

/* A struct for all loudspeakers */
typedef struct ls_triplet_chain {
  int ls_nos[3];
  float inv_mx[9];
  struct ls_triplet_chain *next;
} ls_triplet_chain;

/* functions */

void angle_to_cart( ang_vec *from,  cart_vec *to);
extern void choose_ls_triplets( ls lss[MAX_LS_AMOUNT],
			        ls_triplet_chain **ls_triplets, 
				int ls_amount);
extern void choose_ls_tuplets( ls lss[MAX_LS_AMOUNT],
			        ls_triplet_chain **ls_triplets,
			       int ls_amount);
int lines_intersect(int i,int j,int k,int l, ls lss[MAX_LS_AMOUNT]);
int any_ls_inside_triplet(int a, int b, int c,ls lss[MAX_LS_AMOUNT], int ls_amount);
float vec_angle(cart_vec v1, cart_vec v2);
float vec_prod(cart_vec v1, cart_vec v2);
float vec_length(cart_vec v1);
void cross_prod(cart_vec v1,cart_vec v2, 
		cart_vec *res) ;
extern void add_ldsp_triplet(int i, int j, int k, 
		       ls_triplet_chain **ls_triplets,
		       ls *lss);

extern void  calculate_3x3_matrixes(ls_triplet_chain *ls_triplets,
				 ls lss[MAX_LS_AMOUNT], int ls_amount);
int calc_2D_inv_tmatrix(float azi1,float azi2, float inv_mat[4]);
extern void sort_2D_lss(ls lss[MAX_LS_AMOUNT], int sorted_lss[MAX_LS_AMOUNT],
			int ls_amount);

float vol_p_side_lgth(int i, int j,int k, ls  lss[MAX_LS_AMOUNT] );


