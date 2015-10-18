#include <stdlib.h>
#include "soundpipe.h"
#include "sporth.h"
#include "base.h"
#include "audio.h"

typedef struct {
    sp_ftbl *ft;
    sp_fosc *osc;
    sp_tenv *env;
    sp_metro *met;
} orbit_d;

static int orbits(sporth_stack *stack, void *ud) 
{
    plumber_data *pd = ud;
    SPFLOAT step, base;
    SPFLOAT osc = 0, met = 0, env = 0;
    sporth_func_d *fd;
    orbit_d *od;
    moon_base *md = pd->ud;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in create mode.\n");
#endif

            fd = pd->last->ud;
            od = malloc(sizeof(orbit_d));
            sp_ftbl_create(pd->sp, &od->ft, 4096);
            sp_fosc_create(&od->osc);
            sp_metro_create(&od->met);
            sp_tenv_create(&od->env);

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
            sp_gen_sine(pd->sp, od->ft);
            sp_fosc_init(pd->sp, od->osc, od->ft);
            od->osc->freq = sp_midi2cps(69);
            od->osc->amp = 0.5;
            sp_metro_init(pd->sp, od->met);
            od->met->freq = 0.1;
            sp_tenv_init(pd->sp, od->env);
            od->env->atk = 0.005;
            od->env->rel = 0.1;
            od->env->hold = 0.01;
            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            if(sporth_check_args(stack, "") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            fd = pd->last->ud;
            od  = fd->ud;
            sp_metro_compute(pd->sp, od->met, NULL, &met);
            sp_tenv_compute(pd->sp, od->env, &md->trig, &env);
            sp_fosc_compute(pd->sp, od->osc, NULL, &osc);
            sporth_stack_push_float(stack, osc * env);

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in destroy mode.\n");
#endif
            fd = pd->last->ud;
            od = fd->ud;

            sp_fosc_destroy(&od->osc);
            sp_ftbl_destroy(&od->ft);
            sp_metro_destroy(&od->met);
            sp_tenv_destroy(&od->env);
            free(od);

            break;

        default:
            fprintf(stderr, "aux (f)unction: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int moon_sound_init(moon_base *md) 
{
    sp_createn(&md->sp, 2);
    md->sp->sr = md->sr;
    plumber_register(&md->pd);
    plumber_init(&md->pd);
    md->pd.f[0] = orbits;
    md->pd.sp = md->sp;
    md->pd.ud = md;
    char *str = 
        "0 f dup dup 0.97 10000 revsc "
        "0.3 * swap 0.3 * "
        "rot dup rot + rot +"
        ;

    plumber_parse_string(&md->pd, str);
    plumber_compute(&md->pd, PLUMBER_INIT);

    return 0;
}

int moon_sound_compute(moon_base *md)
{
    plumber_compute(&md->pd, PLUMBER_COMPUTE);
    md->sp->out[0] = sporth_stack_pop_float(&md->pd.sporth.stack);
    md->sp->out[1] = sporth_stack_pop_float(&md->pd.sporth.stack);
    return 0;
}

int moon_sound_destroy(moon_base *md)
{
    plumber_clean(&md->pd);
    sp_destroy(&md->sp);
    return 0;
}
