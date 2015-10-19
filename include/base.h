#ifdef _cplusplus
extern "C" {
#endif

typedef struct {
    float theta;
    float itheta;
    float radius;
    int note;
} moon_circle;

typedef struct {
    sp_data *sp;
    plumber_data pd;
    int sr;
    moon_circle moon[4];
    float speed;
    int notes[4];
} moon_base;

int moon_init(moon_base *mb);
int moon_clean(moon_base *mb);
int moon_draw(moon_base *mb);

#ifdef _cplusplus
}
#endif
