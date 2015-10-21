#ifdef _cplusplus
extern "C" {
#endif

#define MAX_MOONS 8
#define MAX_NOTES 4

typedef struct {
    float theta;
    float itheta;
    float radius;
    int note;
    float time;
} moon_circle;

typedef struct {
    sp_data *sp;
    plumber_data pd;
    int sr;
    moon_circle moon[MAX_MOONS];
    float speed;
    int notes[MAX_NOTES];
    int nmoons;
    int max_moons;
} moon_base;

int moon_init(moon_base *mb);
int moon_clean(moon_base *mb);
int moon_draw(moon_base *mb);

int moon_add(moon_base *mb, float radius, float theta, int note);

#ifdef _cplusplus
}
#endif
