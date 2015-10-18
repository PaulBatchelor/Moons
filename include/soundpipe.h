#include <stdint.h>
#include <stdio.h>

#ifndef NO_LIBSNDFILE
#include "sndfile.h"
#endif 

#define SP_BUFSIZE 4096
#define SPFLOAT float
#define SP_OK 1
#define SP_NOT_OK 0

#define SP_RANDMAX 2147483648

#ifndef SOUNDPIPE_H
#define SOUNDPIPE_H
#endif

typedef unsigned long sp_frame;

typedef struct sp_auxdata {
    size_t size;
    void *ptr;
} sp_auxdata;

typedef struct sp_data { 
    SPFLOAT *out;
    int sr;
    int nchan;
    unsigned long len;
    unsigned long pos;
    char filename[200];
    int k;
    uint32_t rand;
} sp_data; 

typedef struct {
    char state;
    SPFLOAT val;
} sp_param;

int sp_auxdata_alloc(sp_auxdata *aux, size_t size);
int sp_auxdata_free(sp_auxdata *aux);
int sp_auxdata_getbuf(sp_auxdata *aux, uint32_t pos, SPFLOAT *out);
int sp_auxdata_setbuf(sp_auxdata *aux, uint32_t pos, SPFLOAT *in);

int sp_create(sp_data **spp);
int sp_createn(sp_data **spp, int nchan);

int sp_destroy(sp_data **spp);
int sp_process(sp_data *sp, void *ud, void (*callback)(sp_data *, void *));
int sp_process_raw(sp_data *sp, void *ud, void (*callback)(sp_data *, void *));

SPFLOAT sp_midi2cps(SPFLOAT nn);

int sp_set(sp_param *p, SPFLOAT val);

int sp_out(sp_data *sp, uint32_t chan, SPFLOAT val);

uint32_t sp_rand(sp_data *sp);
void sp_srand(sp_data *sp, uint32_t val);
enum{
SPEVT_FREE,
SPEVT_QUEUED,
SPEVT_ON,
SPEVT_OFF,
SPEVT_ERROR,
SPEVT_ONESAMP
};

enum{
SPEVSTK_SEARCH = -2,
SPEVSTK_NOFREE
};

typedef struct {
    int mode;
    sp_frame start;
    sp_frame end;
    void (*init_cb)(void *);
    void (*evton_cb)(void *);
    void (*evtoff_cb)(void *);
    void *ud;
}sp_event; 

typedef struct {
    sp_event *evt;
    int nevents;
    size_t ud_size;
    int nxtfree;
    int curfree;
    int lstfree;
    int nalloc;
    void (*init_cb)(void *);
    void (*evton_cb)(void *);
    void (*evtoff_cb)(void *);
    void *ud;
    int *t;
}sp_evtstack; 

int sp_event_create(sp_event **evt, int nevents);
int sp_event_init(sp_event *evt);
int sp_event_destroy(sp_event **evt);
/* poorly named function because create implies memory mgt. change. */
int sp_event_insert(sp_event *evt, 
        sp_frame cpos, sp_frame start, sp_frame dur,
        void(*init_cb)(void *),
        void(*evton_cb)(void *),
        void(*evtoff_cb)(void *),
        void *ud); 
int sp_event_clear(sp_event *evt);
int sp_event_update(sp_event *evt, sp_frame pos);
int sp_event_exec(sp_event *evt);

/* event stack functions */

int sp_evtstack_create(sp_evtstack **es, int nevents);
int sp_evtstack_init(sp_evtstack *es, 
        void(*init_cb)(void *),
        void(*evton_cb)(void *),
        void(*evtoff_cb)(void *), 
        void *ud, size_t ud_size);
int sp_evtstack_destroy(sp_evtstack **es);

int sp_evtstack_add(sp_evtstack *es, 
        sp_frame cpos, sp_frame start, sp_frame dur, int *pos);

int sp_evtstack_nextfree(sp_evtstack *es, int *id);

int sp_evtstack_update(sp_evtstack *es, sp_frame pos);
int sp_evtstack_exec(sp_evtstack *es);
int sp_evtstack_full(sp_evtstack *es);
typedef struct{
    SPFLOAT amp;
}sp_noise;

int sp_noise_create(sp_noise **ns);
int sp_noise_init(sp_data *sp, sp_noise *ns);
int sp_noise_compute(sp_data *sp, sp_noise *ns, SPFLOAT *in, SPFLOAT *out);
int sp_noise_destroy(sp_noise **ns);
typedef struct {
    SPFLOAT hp;
    SPFLOAT c1, c2, yt1, prvhp;
    SPFLOAT tpidsr;
} sp_tone;

int sp_tone_create(sp_tone **t);
int sp_tone_destroy(sp_tone **t);
int sp_tone_init(sp_data *sp, sp_tone *t);
int sp_tone_compute(sp_data *sp, sp_tone *t, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT freq;
    SPFLOAT res;
    SPFLOAT istor;

    SPFLOAT delay[6];
    SPFLOAT tanhstg[3];
    SPFLOAT oldfreq;
    SPFLOAT oldres;
    SPFLOAT oldacr;
    SPFLOAT oldtune;
} sp_moogladder;

int sp_moogladder_create(sp_moogladder **t);
int sp_moogladder_destroy(sp_moogladder **t);
int sp_moogladder_init(sp_data *sp, sp_moogladder *p);
int sp_moogladder_compute(sp_data *sp, sp_moogladder *p, SPFLOAT *in, SPFLOAT *out);
typedef struct  {
    SPFLOAT sr, freq, istor;
    SPFLOAT lkf;
    SPFLOAT a[8];
    SPFLOAT pidsr;
} sp_butlp;

typedef struct  {
    SPFLOAT sr, freq, istor;
    SPFLOAT lkf;
    SPFLOAT a[8];
    SPFLOAT pidsr;
} sp_buthp;

typedef struct  {
    SPFLOAT sr, freq, bw, istor;
    SPFLOAT lkf, lkb;
    SPFLOAT a[8];
    SPFLOAT pidsr, tpidsr;
} sp_butbp;

int sp_butlp_create(sp_butlp **p);
int sp_butlp_destroy(sp_butlp **p);
int sp_butlp_init(sp_data *sp, sp_butlp *p);
int sp_butlp_compute(sp_data *sp, sp_butlp *p, SPFLOAT *in, SPFLOAT *out);

int sp_buthp_create(sp_buthp **p);
int sp_buthp_destroy(sp_buthp **p);
int sp_buthp_init(sp_data *sp, sp_buthp *p);
int sp_buthp_compute(sp_data *sp, sp_buthp *p, SPFLOAT *in, SPFLOAT *out);

int sp_butbp_create(sp_butbp **p);
int sp_butbp_destroy(sp_butbp **p);
int sp_butbp_init(sp_data *sp, sp_butbp *p);
int sp_butbp_compute(sp_data *sp, sp_butbp *p, SPFLOAT *in, SPFLOAT *out);

int sp_butter_filter(SPFLOAT *in, SPFLOAT *out, SPFLOAT *a);
typedef struct{
size_t size;
void *auxp;
}auxData;
typedef struct {
    int     writePos;
    int     bufferSize;
    int     readPos;
    int     readPosFrac;
    int     readPosFrac_inc;
    int     dummy;
    int     seedVal;
    int     randLine_cnt;
    SPFLOAT filterState;
    SPFLOAT *buf;
} sp_revsc_dl;

typedef struct  {
    SPFLOAT feedback, lpfreq;
    SPFLOAT iSampleRate, iPitchMod, iSkipInit;
    SPFLOAT sampleRate;
    SPFLOAT dampFact;
    SPFLOAT prv_LPFreq;
    int initDone;
    sp_revsc_dl delayLines[8];
    sp_auxdata aux;
} sp_revsc;

int sp_revsc_create(sp_revsc **p);
int sp_revsc_destroy(sp_revsc **p);
int sp_revsc_init(sp_data *sp, sp_revsc *p);
int sp_revsc_compute(sp_data *sp, sp_revsc *p, SPFLOAT *in1, SPFLOAT *in2, SPFLOAT *out1, SPFLOAT *out2);
#define SP_FT_MAXLEN 0x1000000L
#define SP_FT_PHMASK 0x0FFFFFFL

typedef struct sp_ftbl{
    size_t size;
    uint32_t lobits;
    uint32_t lomask;
    SPFLOAT lodiv;
    SPFLOAT sicvt;
    SPFLOAT *tbl;
}sp_ftbl;

int sp_ftbl_create(sp_data *sp, sp_ftbl **ft, size_t size);
int sp_ftbl_destroy(sp_ftbl **ft);

int sp_gen_vals(sp_data *sp, sp_ftbl *ft, const char *string);

int sp_gen_sine(sp_data *sp, sp_ftbl *ft);
int sp_gen_file(sp_data *sp, sp_ftbl *ft, const char *filename);
int sp_gen_sinesum(sp_data *sp, sp_ftbl *ft, const char *argstring);
int sp_gen_line(sp_data *sp, sp_ftbl *ft, const char *argstring);
int sp_gen_xline(sp_data *sp, sp_ftbl *ft, const char *argstring);
int sp_gen_gauss(sp_data *sp, sp_ftbl *ft, SPFLOAT scale, uint32_t seed);
typedef struct {
    SPFLOAT freq, amp, iphs;
    int32_t   lphs;
    sp_ftbl *tbl;
    int inc;
} sp_osc;

int sp_osc_create(sp_osc **osc);
int sp_osc_destroy(sp_osc **osc);
int sp_osc_init(sp_data *sp, sp_osc *osc, sp_ftbl *ft, SPFLOAT iphs);
int sp_osc_compute(sp_data *sp, sp_osc *osc, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT freq, amp, iphs;
    sp_ftbl *tbl;
    int32_t tablen;
    SPFLOAT tablenUPsr;
    SPFLOAT phs;
    SPFLOAT onedsr;
} sp_posc3;

int sp_posc3_create(sp_posc3 **posc3);
int sp_posc3_destroy(sp_posc3 **posc3);
int sp_posc3_init(sp_data *sp, sp_posc3 *posc3, sp_ftbl *ft);
int sp_posc3_compute(sp_data *sp, sp_posc3 *posc3, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT min, max, cps, mode, fstval;
    int16_t cpscod;
    int32_t phs;
    SPFLOAT num1, num2, dfdmax;
    int holdrand;
    SPFLOAT sicvt;
} sp_randi;

int sp_randi_create(sp_randi **p);
int sp_randi_destroy(sp_randi **p);
int sp_randi_init(sp_data *sp, sp_randi *p);
int sp_randi_compute(sp_data *sp, sp_randi *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT gg;
    SPFLOAT outputs;
    SPFLOAT inputs;
    SPFLOAT gain;
} sp_dcblock;

int sp_dcblock_create(sp_dcblock **p);
int sp_dcblock_destroy(sp_dcblock **p);
int sp_dcblock_init(sp_data *sp, sp_dcblock *p);
int sp_dcblock_compute(sp_data *sp, sp_dcblock *p, SPFLOAT *in, SPFLOAT *out);
typedef struct{
    void (*reinit)(void *);
    void (*compute)(void *, SPFLOAT *out);
    void *ud;
    int started;
}sp_tevent;

int sp_tevent_create(sp_tevent **te);
int sp_tevent_destroy(sp_tevent **te);
int sp_tevent_init(sp_data *sp, sp_tevent *te, 
        void (*reinit)(void*), void (*compute)(void *, SPFLOAT *out), void *ud);
int sp_tevent_compute(sp_data *sp, sp_tevent *te, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_metro{
    SPFLOAT sr, freq, iphs;
    SPFLOAT curphs;
    int flag;
    SPFLOAT onedsr;
} sp_metro;

int sp_metro_create(sp_metro **p);
int sp_metro_destroy(sp_metro **p);
int sp_metro_init(sp_data *sp, sp_metro *p);
int sp_metro_compute(sp_data *sp, sp_metro *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_tenv{
    sp_tevent *te;
    uint32_t pos, atk_end, rel_start, sr, totaldur;
    SPFLOAT atk, rel, hold;
    SPFLOAT atk_slp, rel_slp;
    SPFLOAT last;
    int sigmode;
    SPFLOAT input;
} sp_tenv;

int sp_tenv_create(sp_tenv **p);
int sp_tenv_destroy(sp_tenv **p);
int sp_tenv_init(sp_data *sp, sp_tenv *p);
void sp_tenv_reinit(void *ud);
void sp_tenv_comp(void *ud, SPFLOAT *out);
int sp_tenv_compute(sp_data *sp, sp_tenv *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_maygate{
    SPFLOAT prob;
    SPFLOAT gate;
    int mode;
} sp_maygate;

int sp_maygate_create(sp_maygate **p);
int sp_maygate_destroy(sp_maygate **p);
int sp_maygate_init(sp_data *sp, sp_maygate *p);
int sp_maygate_compute(sp_data *sp, sp_maygate *p, SPFLOAT *in, SPFLOAT *out);
typedef struct{
    SPFLOAT freq, atk, dec, istor;
    SPFLOAT tpidsr;
    SPFLOAT sr;
    SPFLOAT delay[4];
}sp_fofilt;

int sp_fofilt_create(sp_fofilt **t);
int sp_fofilt_destroy(sp_fofilt **t);
int sp_fofilt_init(sp_data *sp, sp_fofilt *p);
int sp_fofilt_compute(sp_data *sp, sp_fofilt *p, SPFLOAT *in, SPFLOAT *out);

typedef struct{
    SPFLOAT b0, b1, b2, a0, a1, a2, reinit, xnm1, xnm2, ynm1, ynm2, cutoff, res;
    SPFLOAT sr;
    SPFLOAT tpidsr;
}sp_biquad;

int sp_biquad_create(sp_biquad **p);
int sp_biquad_destroy(sp_biquad **p);
int sp_biquad_init(sp_data *sp, sp_biquad *p);
int sp_biquad_compute(sp_data *sp, sp_biquad *p, SPFLOAT *in, SPFLOAT *out);
typedef struct{
    SPFLOAT fco, res, dist, asym, iskip, y, y1, y2;
    int fcocod, rezcod;
    SPFLOAT sr;
    SPFLOAT onedsr;

}sp_tbvcf;

int sp_tbvcf_create(sp_tbvcf **p);
int sp_tbvcf_destroy(sp_tbvcf **p);
int sp_tbvcf_init(sp_data *sp, sp_tbvcf *p);
int sp_tbvcf_compute(sp_data *sp, sp_tbvcf *p, SPFLOAT *in, SPFLOAT *out);
typedef struct{
    SPFLOAT freq, q, xnm1, ynm1, ynm2, a0, a1, a2, d, lfq, lq;
    SPFLOAT sr;
}sp_mode;

int sp_mode_create(sp_mode **p);
int sp_mode_destroy(sp_mode **p);
int sp_mode_init(sp_data *sp, sp_mode *p);
int sp_mode_compute(sp_data *sp, sp_mode *p, SPFLOAT *in, SPFLOAT *out);
typedef struct  {
    SPFLOAT delay;
    uint32_t bufpos;
    uint32_t bufsize;
    sp_auxdata buf;
} sp_reverse;

int sp_reverse_create(sp_reverse **p);
int sp_reverse_destroy(sp_reverse **p); 
int sp_reverse_init(sp_data *sp, sp_reverse *p, SPFLOAT delay);
int sp_reverse_compute(sp_data *sp, sp_reverse *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_dtrig{
    sp_ftbl *ft;
    uint32_t counter;
    uint32_t pos;
    int running;
    int loop;
    SPFLOAT delay;
} sp_dtrig;

int sp_dtrig_create(sp_dtrig **p);
int sp_dtrig_destroy(sp_dtrig **p);
int sp_dtrig_init(sp_data *sp, sp_dtrig *p, sp_ftbl *ft);
int sp_dtrig_compute(sp_data *sp, sp_dtrig *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_rpt{
    uint32_t playpos;
    uint32_t bufpos;
    int running;
    int count, reps;
    SPFLOAT sr;
    uint32_t size;
    SPFLOAT bpm;
    int div, rep;
    sp_auxdata aux;
} sp_rpt;

int sp_rpt_create(sp_rpt **p);
int sp_rpt_destroy(sp_rpt **p);
int sp_rpt_init(sp_data *sp, sp_rpt *p, SPFLOAT maxdur);
int sp_rpt_compute(sp_data *sp, sp_rpt *p, SPFLOAT *trig, 
        SPFLOAT *in, SPFLOAT *out);

int sp_rpt_set(sp_rpt *p, SPFLOAT bpm, int div, int rep);
typedef struct sp_dust{
    SPFLOAT amp, density; 
    SPFLOAT density0, thresh, scale;
    SPFLOAT onedsr;
    int bipolar; /* 1 = bipolar 0 = unipolar */
    uint32_t rand;
} sp_dust;

int sp_dust_create(sp_dust **p);
int sp_dust_destroy(sp_dust **p);
int sp_dust_init(sp_data *sp, sp_dust *p, SPFLOAT amp, SPFLOAT density);
int sp_dust_compute(sp_data *sp, sp_dust *p, SPFLOAT *in, SPFLOAT *out);


typedef struct{
    SPFLOAT htime;
    SPFLOAT c1, c2, yt1, prvhtim;
    SPFLOAT sr, onedsr;
}sp_port;

int sp_port_create(sp_port **p);
int sp_port_destroy(sp_port **p);
int sp_port_init(sp_data *sp, sp_port *p, SPFLOAT htime);
int sp_port_compute(sp_data *sp, sp_port *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_vdelay{
    SPFLOAT del, maxdel;
    SPFLOAT sr;
    sp_auxdata buf;
    int32_t left;
} sp_vdelay;

int sp_vdelay_create(sp_vdelay **p);
int sp_vdelay_destroy(sp_vdelay **p);
int sp_vdelay_init(sp_data *sp, sp_vdelay *p, SPFLOAT maxdel);
int sp_vdelay_compute(sp_data *sp, sp_vdelay *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_count{
    int32_t count, curcount;
    int mode;
} sp_count;

int sp_count_create(sp_count **p);
int sp_count_destroy(sp_count **p);
int sp_count_init(sp_data *sp, sp_count *p);
int sp_count_compute(sp_data *sp, sp_count *p, SPFLOAT *in, SPFLOAT *out);
typedef struct{
    SPFLOAT freq, fdbgain;
    SPFLOAT LPdelay, APdelay;
    SPFLOAT *Cdelay;
    sp_auxdata buf;
    int wpointer, rpointer, size;
}sp_streson;

int sp_streson_create(sp_streson **p);
int sp_streson_destroy(sp_streson **p);
int sp_streson_init(sp_data *sp, sp_streson *p);
int sp_streson_compute(sp_data *sp, sp_streson *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_tseq {
    sp_ftbl *ft;
    SPFLOAT val;
    uint32_t pos;
    int shuf;
} sp_tseq;

int sp_tseq_create(sp_tseq **p);
int sp_tseq_destroy(sp_tseq **p);
int sp_tseq_init(sp_data *sp, sp_tseq *p, sp_ftbl *ft);
int sp_tseq_compute(sp_data *sp, sp_tseq *p, SPFLOAT *trig, SPFLOAT *val);
typedef struct sp_bal{
    SPFLOAT asig, csig, ihp;
    SPFLOAT c1, c2, prvq, prvr, prva;
} sp_bal;

int sp_bal_create(sp_bal **p);
int sp_bal_destroy(sp_bal **p);
int sp_bal_init(sp_data *sp, sp_bal *p);
int sp_bal_compute(sp_data *sp, sp_bal *p, SPFLOAT *sig, SPFLOAT *comp, SPFLOAT *out);
typedef struct sp_drip{

    SPFLOAT amp; /* How loud */
    SPFLOAT dettack; /* How loud */
    SPFLOAT num_tubes;
    SPFLOAT damp;
    SPFLOAT shake_max;
    SPFLOAT freq;
    SPFLOAT freq1;
    SPFLOAT freq2;

    SPFLOAT num_objectsSave;
    SPFLOAT shake_maxSave;
    SPFLOAT shakeEnergy;
    SPFLOAT outputs00;
    SPFLOAT outputs01;
    SPFLOAT outputs10;
    SPFLOAT outputs11;
    SPFLOAT outputs20;
    SPFLOAT outputs21;
    SPFLOAT coeffs00;
    SPFLOAT coeffs01;
    SPFLOAT coeffs10;
    SPFLOAT coeffs11;
    SPFLOAT coeffs20;
    SPFLOAT coeffs21;
    SPFLOAT finalZ0;
    SPFLOAT finalZ1;
    SPFLOAT finalZ2;
    SPFLOAT sndLevel;
    SPFLOAT gains0;
    SPFLOAT gains1;
    SPFLOAT gains2;
    SPFLOAT center_freqs0;
    SPFLOAT center_freqs1;
    SPFLOAT center_freqs2;
    SPFLOAT soundDecay;
    SPFLOAT systemDecay;
    SPFLOAT num_objects;
    SPFLOAT totalEnergy;
    SPFLOAT decayScale;
    SPFLOAT res_freq0;
    SPFLOAT res_freq1;
    SPFLOAT res_freq2;
    SPFLOAT shake_damp;
    int kloop;
} sp_drip;

int sp_drip_create(sp_drip **p);
int sp_drip_destroy(sp_drip **p);
int sp_drip_init(sp_data *sp, sp_drip *p, SPFLOAT dettack);
int sp_drip_compute(sp_data *sp, sp_drip *p, SPFLOAT *trig, SPFLOAT *out);
typedef struct{
    SPFLOAT arg, lim, k1, k2;
    int meth;
}sp_clip;

int sp_clip_create(sp_clip **p);
int sp_clip_destroy(sp_clip **p);
int sp_clip_init(sp_data *sp, sp_clip *p, int meth, SPFLOAT lim);
int sp_clip_compute(sp_data *sp, sp_clip *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_fosc{
    SPFLOAT amp, freq, car, mod, indx, iphs;
    int32_t mphs, cphs;
    sp_ftbl *ft;
} sp_fosc;

int sp_fosc_create(sp_fosc **p);
int sp_fosc_destroy(sp_fosc **p);
int sp_fosc_init(sp_data *sp, sp_fosc *p, sp_ftbl *ft);
int sp_fosc_compute(sp_data *sp, sp_fosc *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_dist{
    SPFLOAT pregain, postgain, shape1, shape2, mode;
} sp_dist;

int sp_dist_create(sp_dist **p);
int sp_dist_destroy(sp_dist **p);
int sp_dist_init(sp_data *sp, sp_dist *p);
int sp_dist_compute(sp_data *sp, sp_dist *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
        SPFLOAT amp, freq, nharm, lharm, mul, iphs;
        int16_t ampcod, cpscod, prvn;
        SPFLOAT prvr, twor, rsqp1, rtn, rtnp1, rsumr;
        int32_t lphs;
        int reported;
        SPFLOAT last;
        sp_ftbl *ft;
} sp_gbuzz;

int sp_gbuzz_create(sp_gbuzz **p);
int sp_gbuzz_destroy(sp_gbuzz **p);
int sp_gbuzz_init(sp_data *sp, sp_gbuzz *p, sp_ftbl *ft, SPFLOAT iphs);
int sp_gbuzz_compute(sp_data *sp, sp_gbuzz *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_rms{
    SPFLOAT ihp, istor;
    SPFLOAT c1, c2, prvq;
} sp_rms;

int sp_rms_create(sp_rms **p);
int sp_rms_destroy(sp_rms **p);
int sp_rms_init(sp_data *sp, sp_rms *p);
int sp_rms_compute(sp_data *sp, sp_rms *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_jitter{
    SPFLOAT amp, cpsMin, cpsMax;
    SPFLOAT cps;
    int32_t phs;
    int initflag;
    SPFLOAT num1, num2, dfdmax;
} sp_jitter;

int sp_jitter_create(sp_jitter **p);
int sp_jitter_destroy(sp_jitter **p);
int sp_jitter_init(sp_data *sp, sp_jitter *p);
int sp_jitter_compute(sp_data *sp, sp_jitter *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_lpf18{
    SPFLOAT cutoff, res, dist;
    SPFLOAT ay1, ay2, aout, lastin, onedsr;
} sp_lpf18;

int sp_lpf18_create(sp_lpf18 **p);
int sp_lpf18_destroy(sp_lpf18 **p);
int sp_lpf18_init(sp_data *sp, sp_lpf18 *p);
int sp_lpf18_compute(sp_data *sp, sp_lpf18 *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    int mti;
    /* do not change value 624 */
    uint32_t mt[624];
} sp_randmt;

void sp_randmt_seed(sp_randmt *p,
    const uint32_t *initKey, uint32_t keyLength);

uint32_t sp_randmt_compute(sp_randmt *p);
typedef struct {
    SPFLOAT revtime, looptime;
    SPFLOAT coef, prvt;
    sp_auxdata aux;
    uint32_t bufpos;
    uint32_t bufsize;
} sp_allpass;

int sp_allpass_create(sp_allpass **p);
int sp_allpass_destroy(sp_allpass **p);
int sp_allpass_init(sp_data *sp, sp_allpass *p, SPFLOAT looptime);
int sp_allpass_compute(sp_data *sp, sp_allpass *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_comb{
    SPFLOAT revtime, looptime;
    SPFLOAT coef, prvt;
    sp_auxdata aux;
    uint32_t bufpos;
    uint32_t bufsize;
} sp_comb;

int sp_comb_create(sp_comb **p);
int sp_comb_destroy(sp_comb **p);
int sp_comb_init(sp_data *sp, sp_comb *p, SPFLOAT looptime);
int sp_comb_compute(sp_data *sp, sp_comb *p, SPFLOAT *in, SPFLOAT *out);
typedef struct sp_phasor{
    SPFLOAT freq, phs;
    SPFLOAT curphs, onedsr;
} sp_phasor;

int sp_phasor_create(sp_phasor **p);
int sp_phasor_destroy(sp_phasor **p);
int sp_phasor_init(sp_data *sp, sp_phasor *p, SPFLOAT iphs);
int sp_phasor_compute(sp_data *sp, sp_phasor *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    sp_ftbl *ft;
    SPFLOAT speed;  
    SPFLOAT pos;
} sp_tabread;

int sp_tabread_create(sp_tabread **p);
int sp_tabread_destroy(sp_tabread **p);
int sp_tabread_init(sp_data *sp, sp_tabread *p, sp_ftbl *ft);
int sp_tabread_compute(sp_data *sp, sp_tabread *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
  SPFLOAT freq, bw, gain;
  SPFLOAT z1,z2, sr;
  SPFLOAT frv, bwv;
  SPFLOAT c,d;
} sp_eqfil;

int sp_eqfil_create(sp_eqfil **p);
int sp_eqfil_destroy(sp_eqfil **p);
int sp_eqfil_init(sp_data *sp, sp_eqfil *p);
int sp_eqfil_compute(sp_data *sp, sp_eqfil *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT bar;
} sp_foo;

int sp_foo_create(sp_foo **p);
int sp_foo_destroy(sp_foo **p);
int sp_foo_init(sp_data *sp, sp_foo *p);
int sp_foo_compute(sp_data *sp, sp_foo *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT sr, freq, bw, istor;
    SPFLOAT lkf, lkb;
    SPFLOAT a[8];
    SPFLOAT pidsr, tpidsr;
} sp_butbr;

int sp_butbr_create(sp_butbr **p);
int sp_butbr_destroy(sp_butbr **p);
int sp_butbr_init(sp_data *sp, sp_butbr *p);
int sp_butbr_compute(sp_data *sp, sp_butbr *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    FILE *fp;
} sp_in;

int sp_in_create(sp_in **p);
int sp_in_destroy(sp_in **p);
int sp_in_init(sp_data *sp, sp_in *p);
int sp_in_compute(sp_data *sp, sp_in *p, SPFLOAT *in, SPFLOAT *out);
typedef struct prop_event {
    char type;
    uint32_t pos;
    uint32_t val;
    struct prop_event *next;
} prop_event;

typedef struct {
    uint32_t mul;
    uint32_t div;
    uint32_t tmp;
    uint32_t num;
    SPFLOAT scale;
    int mode;
    uint32_t pos;
    uint32_t evtpos;
    prop_event root;
    prop_event *last;
} prop_data;

typedef struct {
   prop_data *prp;
   prop_event evt;
   uint32_t count;
   SPFLOAT bpm;
   SPFLOAT lbpm;
} sp_prop;


int prop_create(prop_data **pd);
int prop_parse(prop_data *pd, const char *str);
prop_event prop_next(prop_data *pd);
float prop_time(prop_data *pd, prop_event evt);
int prop_destroy(prop_data **pd);

int sp_prop_create(sp_prop **p);
int sp_prop_destroy(sp_prop **p);
int sp_prop_init(sp_data *sp, sp_prop *p, const char *str);
int sp_prop_compute(sp_data *sp, sp_prop *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT val;
} sp_samphold;

int sp_samphold_create(sp_samphold **p);
int sp_samphold_destroy(sp_samphold **p);
int sp_samphold_init(sp_data *sp, sp_samphold *p);
int sp_samphold_compute(sp_data *sp, sp_samphold *p, SPFLOAT *trig, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT time;
    SPFLOAT feedback;
    SPFLOAT last;
    sp_auxdata buf;
    uint32_t bufsize;
    uint32_t bufpos;
    int init;
} sp_delay;

int sp_delay_create(sp_delay **p);
int sp_delay_destroy(sp_delay **p);
int sp_delay_init(sp_data *sp, sp_delay *p, SPFLOAT time);
int sp_delay_compute(sp_data *sp, sp_delay *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT mode;
} sp_switch;

int sp_switch_create(sp_switch **p);
int sp_switch_destroy(sp_switch **p);
int sp_switch_init(sp_data *sp, sp_switch *p);
int sp_switch_compute(sp_data *sp, sp_switch *p, SPFLOAT *trig,
    SPFLOAT *in1, SPFLOAT *in2, SPFLOAT *out);
typedef struct {
    SPFLOAT time;
    uint32_t counter;
} sp_dmetro;

int sp_dmetro_create(sp_dmetro **p);
int sp_dmetro_destroy(sp_dmetro **p);
int sp_dmetro_init(sp_data *sp, sp_dmetro *p);
int sp_dmetro_compute(sp_data *sp, sp_dmetro *p, SPFLOAT *in, SPFLOAT *out);
typedef struct { 
    SPFLOAT min;
    SPFLOAT max;
} sp_random;

int sp_random_create(sp_random **p);
int sp_random_destroy(sp_random **p);
int sp_random_init(sp_data *sp, sp_random *p);
int sp_random_compute(sp_data *sp, sp_random *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT plk, amp, freq, pick, reflect;
    sp_auxdata upper;
    sp_auxdata lower;
    sp_auxdata up_data;
    sp_auxdata down_data;
    SPFLOAT ifreq;
    SPFLOAT state;
    int scale;
    int rail_len;
    int irail_len;
    int plucked;
} sp_pluck;

int sp_pluck_create(sp_pluck **p);
int sp_pluck_destroy(sp_pluck **p);
int sp_pluck_init(sp_data *sp, sp_pluck *p, SPFLOAT ifreq);
int sp_pluck_compute(sp_data *sp, sp_pluck *p, SPFLOAT *trig, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT hp;
    SPFLOAT c1, c2, yt1, prvhp;
    SPFLOAT tpidsr;
} sp_atone;

int sp_atone_create(sp_atone **p);
int sp_atone_destroy(sp_atone **p);
int sp_atone_init(sp_data *sp, sp_atone *p);
int sp_atone_compute(sp_data *sp, sp_atone *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    FILE *fp;
    SPFLOAT val;
} sp_tin;

int sp_tin_create(sp_tin **p);
int sp_tin_destroy(sp_tin **p);
int sp_tin_init(sp_data *sp, sp_tin *p);
int sp_tin_compute(sp_data *sp, sp_tin *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT inmin, inmax, outmin, outmax;
} sp_scale;

int sp_scale_create(sp_scale **p);
int sp_scale_destroy(sp_scale **p);
int sp_scale_init(sp_data *sp, sp_scale *p);
int sp_scale_compute(sp_data *sp, sp_scale *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT pan;
    uint32_t type;
} sp_pan2;

int sp_pan2_create(sp_pan2 **p);
int sp_pan2_destroy(sp_pan2 **p);
int sp_pan2_init(sp_data *sp, sp_pan2 *p);
int sp_pan2_compute(sp_data *sp, sp_pan2 *p, SPFLOAT *in, SPFLOAT *out1, SPFLOAT *out2);
typedef struct {
    void *ud;
} sp_jcrev;

int sp_jcrev_create(sp_jcrev **p);
int sp_jcrev_destroy(sp_jcrev **p);
int sp_jcrev_init(sp_data *sp, sp_jcrev *p);
int sp_jcrev_compute(sp_data *sp, sp_jcrev *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    int state;
    SPFLOAT atk, rel;
    uint32_t totaltime;
    uint32_t timer;
    SPFLOAT slope;
    SPFLOAT last;
} sp_tenv2;

int sp_tenv2_create(sp_tenv2 **p);
int sp_tenv2_destroy(sp_tenv2 **p);
int sp_tenv2_init(sp_data *sp, sp_tenv2 *p);
int sp_tenv2_compute(sp_data *sp, sp_tenv2 *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT incr;
    SPFLOAT index;
    int32_t sample_index;
    SPFLOAT value;
} sp_fold;

int sp_fold_create(sp_fold **p);
int sp_fold_destroy(sp_fold **p);
int sp_fold_init(sp_data *sp, sp_fold *p);
int sp_fold_compute(sp_data *sp, sp_fold *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT bitdepth;
    SPFLOAT srate;
    sp_fold *fold;
} sp_decimator;

int sp_decimator_create(sp_decimator **p);
int sp_decimator_destroy(sp_decimator **p);
int sp_decimator_init(sp_data *sp, sp_decimator *p);
int sp_decimator_compute(sp_data *sp, sp_decimator *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
} sp_inverse;

int sp_inverse_create(sp_inverse **p);
int sp_inverse_destroy(sp_inverse **p);
int sp_inverse_init(sp_data *sp, sp_inverse *p);
int sp_inverse_compute(sp_data *sp, sp_inverse *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
} sp_mul;

int sp_mul_create(sp_mul **p);
int sp_mul_destroy(sp_mul **p);
int sp_mul_init(sp_data *sp, sp_mul *p);
int sp_mul_compute(sp_data *sp, sp_mul *p, SPFLOAT *in1, SPFLOAT *in2, SPFLOAT *out);
typedef struct {
} sp_add;

int sp_add_create(sp_add **p);
int sp_add_destroy(sp_add **p);
int sp_add_init(sp_data *sp, sp_add *p);
int sp_add_compute(sp_data *sp, sp_add *p, SPFLOAT *in1, SPFLOAT *in2, SPFLOAT *out);
typedef struct {
} sp_sub;

int sp_sub_create(sp_sub **p);
int sp_sub_destroy(sp_sub **p);
int sp_sub_init(sp_data *sp, sp_sub *p);
int sp_sub_compute(sp_data *sp, sp_sub *p, SPFLOAT *in1, SPFLOAT *in2, SPFLOAT *out);
typedef struct {
} sp_div;

int sp_div_create(sp_div **p);
int sp_div_destroy(sp_div **p);
int sp_div_init(sp_data *sp, sp_div *p);
int sp_div_compute(sp_data *sp, sp_div *p, SPFLOAT *in1, SPFLOAT *in2, SPFLOAT *out);
typedef struct {
} sp_abs;

int sp_abs_create(sp_abs **p);
int sp_abs_destroy(sp_abs **p);
int sp_abs_init(sp_data *sp, sp_abs *p);
int sp_abs_compute(sp_data *sp, sp_abs *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT pan;
    uint32_t type;
} sp_panst;

int sp_panst_create(sp_panst **p);
int sp_panst_destroy(sp_panst **p);
int sp_panst_init(sp_data *sp, sp_panst *p);
int sp_panst_compute(sp_data *sp, sp_panst *p, SPFLOAT *in1, SPFLOAT *in2, SPFLOAT *out1, SPFLOAT *out2);
typedef struct {
    SPFLOAT bcL, bcR, iK, ib, scan, T30;
    SPFLOAT pos, vel, wid;

    SPFLOAT *w, *w1, *w2;
    int step, first;
    SPFLOAT s0, s1, s2, t0, t1;
    int i_bcL, i_bcR, N;
    sp_auxdata w_aux;
} sp_bar;

int sp_bar_create(sp_bar **p);
int sp_bar_destroy(sp_bar **p);
int sp_bar_init(sp_data *sp, sp_bar *p, SPFLOAT iK, SPFLOAT ib);
int sp_bar_compute(sp_data *sp, sp_bar *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    int nbars, skip;
    int counter;
    uint32_t len;
} sp_progress;

int sp_progress_create(sp_progress **p);
int sp_progress_destroy(sp_progress **p);
int sp_progress_init(sp_data *sp, sp_progress *p);
int sp_progress_compute(sp_data *sp, sp_progress *p, SPFLOAT *in, SPFLOAT *out);
typedef struct {
    SPFLOAT fc, v, q, mode;

    SPFLOAT xnm1, xnm2, ynm1, ynm2;
    SPFLOAT prv_fc, prv_v, prv_q;
    SPFLOAT b0, b1, b2, a1, a2;
    SPFLOAT tpidsr;
    int imode;
} sp_pareq;

int sp_pareq_create(sp_pareq **p);
int sp_pareq_destroy(sp_pareq **p);
int sp_pareq_init(sp_data *sp, sp_pareq *p);
int sp_pareq_compute(sp_data *sp, sp_pareq *p, SPFLOAT *in, SPFLOAT *out);
