#ifdef _cplusplus
extern "C" {
#endif

typedef struct {
    sp_data *sp;
    plumber_data pd;
    int sr;
    float theta;
    float ltheta;
    float trig;
    int init;
} moon_base;

int moon_init(moon_base *mb);
int moon_clean(moon_base *mb);
int moon_draw(moon_base *mb);

#ifdef _cplusplus
}
#endif
