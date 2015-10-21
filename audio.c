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
    orb->osc->indx = 1.2;
    orb->osc->mod = 7;

    sp_metro_init(mb->sp, orb->met);
    orb->met->freq = 0.1;

    sp_tenv_init(mb->sp, orb->env);
    orb->env->atk = 0.02;
    orb->env->rel = 1.7;
    orb->env->hold = 0.01;

    sp_tenv_init(mb->sp, orb->env_timbre);
    orb->env_timbre->atk = 0.02;
    orb->env_timbre->rel = 0.2;
    orb->env_timbre->hold = 0.01;

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
    }
    
    orb->osc->freq = sp_midi2cps(mb->notes[orb->moon->note]);

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
            orbit_create(mb, &od, mb->max_moons);

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
            for(i = 0; i < mb->max_moons; i++) {
                orbit_init(mb, &od[i], &mb->moon[i]);
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
            for(i = 0; i < mb->nmoons; i++) {
                out += orbit_compute(mb, &od[i]);
            }

            sporth_stack_push_float(stack, out);
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
        "0 f dup 0.5 1.1 delay 1000 butlp 0.2 * + dup dup 0.97 10000 revsc "
        "0.1 * swap 0.1 * "
        "rot dup rot + rot rot +"  
        ;

    plumber_parse_string(&mb->pd, str);
    plumber_compute(&mb->pd, PLUMBER_INIT);

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
