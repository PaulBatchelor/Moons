#ifdef __cplusplus
extern "C" {
#endif

#define MAX_MOONS 8
#define MAX_RIPPLES 10 

typedef struct {
    float theta;
    float itheta;
/* radius from center point */
    float radius;
    float size;
    int note;
    float time;
    float alpha;
    float decay;
    int decay_mode;
    int *nmoons;
} moon_circle;

typedef struct {
    moon_circle *moon;
    int max_moons;
    int nmoons;
} moon_cluster;

typedef struct {
    int max;
    int offset;
    int size;
} ripple_stack;

typedef struct {
    sp_data *sp;
    sp_ftbl *scale;
    plumber_data pd;
    int sr;
    //moon_circle moon[MAX_MOONS];
    float speed;
    //int nmoons;
    //int max_moons;
    moon_cluster satellites;
    moon_cluster ripples;
    ripple_stack rstack;
    int undo;
    int fade;
} moon_base;

int moon_init(moon_base *mb);
int moon_clean(moon_base *mb);
int moon_draw(moon_base *mb);
int moon_cluster_create(moon_cluster *mc, unsigned int max_moons);
int moon_cluster_destroy(moon_cluster *mc);

int moon_add(moon_base *mb, moon_cluster *mc, float radius, float theta);

/* TODO: merge moon_add and ripple_add*/
int ripple_add(moon_base *mb, moon_cluster *mc, float radius, float theta);

int rstack_init(ripple_stack *rs);
int rstack_add(ripple_stack *rs);
int rstack_get(ripple_stack *rs, int index);
int rstack_pop(ripple_stack *rs);

#ifdef __cplusplus
}
#endif

