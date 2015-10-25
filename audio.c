#include <stdlib.h>
#include <math.h>
#include "soundpipe.h"
#include "sporth.h"
#include "base.h"
#include "audio.h"

typedef struct orbit_d {
    moon_circle *moon;
    sp_ftbl *ft;
    sp_fosc *osc;
    sp_tenv *env;
    sp_tenv *env_timbre;
    sp_metro *met;
    //float time;
    //float dur;
} orbit_d;

static int orbit_create(moon_base *mb, orbit_d **orbptr, int n)
{
    *orbptr = malloc(sizeof(orbit_d) * n);
    int i;
    orbit_d *orb = *orbptr;
    for(i = 0; i < n; i++) {
        sp_ftbl_create(mb->sp, &orb[i].ft, 4096);
        sp_fosc_create(&orb[i].osc);
        sp_metro_create(&orb[i].met);
        sp_tenv_create(&orb[i].env);
        sp_tenv_create(&orb[i].env_timbre);
    }
    return 0;
}

static int orbit_destroy(moon_base *mb, orbit_d **orbptr)
{
    orbit_d *orb = *orbptr;
    sp_fosc_destroy(&orb->osc);
    sp_ftbl_destroy(&orb->ft);
    sp_metro_destroy(&orb->met);
    sp_tenv_destroy(&orb->env);
    sp_tenv_destroy(&orb->env_timbre);
    free(orb);
    return 0;
}

int orbit_init(moon_base *mb, orbit_d *orb, moon_circle *moon)
{
    sp_gen_sine(mb->sp, orb->ft);

    sp_fosc_init(mb->sp, orb->osc, orb->ft);
    orb->osc->amp = 0.15;
    orb->osc->indx = 4;
    orb->osc->mod = 9;

    sp_metro_init(mb->sp, orb->met);
    orb->met->freq = 0.1;

    sp_tenv_init(mb->sp, orb->env);
    orb->env->atk = 0.02;
    orb->env->rel = 1.7;
    orb->env->hold = 0.01;

    sp_tenv_init(mb->sp, orb->env_timbre);
    orb->env_timbre->atk = 0.001;
    orb->env_timbre->rel = 0.1;
    orb->env_timbre->hold = 0;

    moon->time = 0;
    orb->moon = moon;

    return 0;
}

static SPFLOAT orbit_compute(moon_base *mb, orbit_d *orb)
{
    SPFLOAT osc = 0, met = 0, env = 0, env_timbre = 0;
    SPFLOAT dur = mb->speed * orb->moon->radius;
    if(floor(orb->moon->time) == 0) {
        met = 1;
        orb->osc->freq = sp_midi2cps(mb->scale->tbl[orb->moon->note]);
        fprintf(stderr,"adding a new ripple.");
        ripple_add(mb, &mb->ripples, orb->moon->radius, orb->moon->theta);
        rstack_add(&mb->rstack);
        fprintf(stderr, "there are now %d ripples.\n", mb->rstack.size);
    }


    sp_tenv_compute(mb->sp, orb->env, &met, &env);
    sp_tenv_compute(mb->sp, orb->env_timbre, &met, &env_timbre);

    orb->osc->indx = 0.01 + env_timbre * 0.5;
    sp_fosc_compute(mb->sp, orb->osc, NULL, &osc);

    orb->moon->theta = orb->moon->itheta +
        ((float) orb->moon->time / (dur * mb->sp->sr)) *
        2 * M_PI;
    orb->moon->time++;
    orb->moon->time = fmod(orb->moon->time, dur * mb->sp->sr);

    return osc * env;
}

static int orbits(sporth_stack *stack, void *ud)
{
    int i;
    plumber_data *pd = ud;
    SPFLOAT out = 0;
    sporth_func_d *fd;
    orbit_d *od;
    moon_base *mb = pd->ud;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in create mode.\n");
#endif

            fd = pd->last->ud;
            orbit_create(mb, &od, mb->satellites.max_moons);

            fd->ud = od;

            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in init mode.\n");
#endif

            if(sporth_check_args(stack, "") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            fd = pd->last->ud;
            od  = fd->ud;
            for(i = 0; i < mb->satellites.max_moons; i++) {
                orbit_init(mb, &od[i], &mb->satellites.moon[i]);
            }
            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            if(sporth_check_args(stack, "") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for mo000on\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            fd = pd->last->ud;
            od  = fd->ud;
            for(i = 0; i < mb->satellites.nmoons; i++) {
                out += orbit_compute(mb, &od[i]);
            }

            sporth_stack_push_float(stack, out);

            if(mb->undo == 1) {
                    mb->undo++;
            } else if(mb->undo == 2) {
                pd->p[0] = 0;
                mb->undo = 0;
            }

            if(mb->fade == 1) {
                mb->fade++;
                pd->p[1] = 1;
            } else if(mb->fade == 2) {
                pd->p[1] = 0;
                mb->fade = 0;
            }

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in destroy mode.\n");
#endif
            fd = pd->last->ud;
            od = fd->ud;
            orbit_destroy(mb, &od);
            break;

        default:
            fprintf(stderr, "aux (f)unction: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int moon_sound_init(moon_base *mb)
{
    sp_createn(&mb->sp, 2);
    mb->sp->sr = mb->sr;
    plumber_register(&mb->pd);
    plumber_init(&mb->pd);
    mb->pd.f[0] = orbits;
    mb->pd.sp = mb->sp;
    mb->pd.ud = mb;
    char *str =
        "'scale' '62 67 69 74 76 78 85' gen_vals "
        "'sine' 4096 gen_sine "
        "0 f dup 0.5 1.1 delay 1500 butlp 0.3 * + "
        "0 p 0.1 0.1 0.2 tenv 0.05 noise * 2000 butlp + "

        "0 'scale' tget 24 - 0.07 port mtof 0.3 1 1 3 fm "
        "0.08 1 0.75 'sine' osc 0 1 scale * "
        "1 'scale' tget 12 - 0.07 port mtof 0.1 1 1 3 fm "
        "0.03 1 0.75 'sine' osc 0 1 scale * "
        "2 'scale' tget 12 - 0.07 port mtof 0.1 1 1 3 fm "
        "0.08 1 0.75 'sine' osc 0 1 scale * "
        "+ + "
        "0 0.6 0.1 randi * + "
        "1 p 1 1 tenv2 * "
        "dup dup 0.97 10000 revsc "
        "0.15 * swap 0.15 * "
        "rot dup rot + rot rot +"
        ;

    plumber_parse_string(&mb->pd, str);
    plumber_compute(&mb->pd, PLUMBER_INIT);

    plumber_ftmap_search(&mb->pd, "scale", &mb->scale);

    return 0;
}

int moon_sound_compute(moon_base *mb)
{
    plumber_compute(&mb->pd, PLUMBER_COMPUTE);
    mb->sp->out[0] = sporth_stack_pop_float(&mb->pd.sporth.stack);
    mb->sp->out[1] = sporth_stack_pop_float(&mb->pd.sporth.stack);
    return 0;
}

int moon_sound_destroy(moon_base *mb)
{
    plumber_clean(&mb->pd);
    sp_destroy(&mb->sp);
    return 0;
}

/* TODO: perhaps merge this with the C++ counterpart somehow... */
int ripple_add(moon_base *mb, moon_cluster *mc, float radius, float theta)
{
    if(mc->nmoons >= mc->max_moons) {
        fprintf(stderr, "Warning: max number of moons created!");
        return 0;
    }

    int id;
    //mc->nmoons++;

    id = rstack_get(&mb->rstack, mb->rstack.size);

    while(theta >= 2 * M_PI) theta -= 2 * M_PI;
    while(theta < 0) theta += 2 * M_PI;

    mc->moon[id].theta = theta;
    mc->moon[id].itheta = theta;
    mc->moon[id].radius = radius;
    mc->moon[id].time = 0;
    /* alpha starts at full blast, then decays */
    mc->moon[id].alpha = 1;
    /* toggle visibility */
    mc->moon[id].decay = 0.05;
    mc->moon[id].decay_mode = 0;
    mc->moon[id].nmoons = &mc->nmoons;
    mc->moon[id].size = 0.07;

    theta = fabs(theta) / (2.0 * M_PI);
    /* this is the only reason why we need moon_base in this function */
    theta = floor(mb->scale->size * theta);
    mc->moon[id].note= (int)theta;
    fprintf(stderr, "the note is %d!, theta is %g\n",
            mc->moon[id].note, theta);
}
int rstack_init(ripple_stack *rs)
{
    rs->max = MAX_RIPPLES;
    rs->offset = 0;
    rs->size = 0;
    return 0;
}

int rstack_add(ripple_stack *rs)
{
    if(rs->size >= rs->max) {
        fprintf(stderr, "Warning: rstack size at maximum (%d)\n", rs->max);
    }
    int pos = rs->offset + rs->size;
    pos %= rs->max;
    rs->size++;

    return 0;
}

int rstack_get(ripple_stack *rs, int index)
{
    int pos = (rs->offset + index) % rs->max;
    return pos;
}

int rstack_pop(ripple_stack *rs)
{
    if(rs->size <= 0) {
        fprintf(stderr,"uh oh. looks like there is nothing to pop!\n");
    }
    rs->offset++;
    rs->offset %= rs->max;
    rs->size--;
}
