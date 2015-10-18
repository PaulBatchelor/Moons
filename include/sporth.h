/* This is a generated header file */
#ifdef MINIMAL
#include <stdlib.h>
#endif

enum {
SPORTH_FLOAT,
SPORTH_STRING,
SPORTH_IGNORE,
SPORTH_FUNC,
SPORTH_NOTOK,
SPORTH_OK
};

#define SPORTH_FOFFSET 2
#define SPORTH_MAXCHAR 200

typedef struct {
    float fval;
    char sval[SPORTH_MAXCHAR];
    int type;
} sporth_stack_val;

typedef struct {
    int pos;
    uint32_t error;
    sporth_stack_val stack[32];
} sporth_stack;

typedef struct sporth_entry {
    uint32_t val;
    char *key;
    struct sporth_entry *next;
} sporth_entry;

typedef struct {
    uint32_t count;
    sporth_entry root;
    sporth_entry *last;
} sporth_list ;

typedef struct {
    sporth_list list[256];
} sporth_htable;

typedef struct {
    const char *name;
    int (*func)(sporth_stack *, void *);
    void *ud;
} sporth_func;

typedef struct sporth_data {
    sporth_htable dict;
    uint32_t nfunc;
    sporth_func *flist;
    sporth_stack stack;
} sporth_data;

uint32_t sporth_hash(const char *str);
int sporth_search(sporth_htable *ht, const char *key, uint32_t *val);
int sporth_htable_add(sporth_htable *ht, const char *key, uint32_t val);
int sporth_htable_init(sporth_htable *ht);
int sporth_htable_destroy(sporth_htable *ht);

int sporth_stack_init(sporth_stack *stack);
int sporth_stack_push_float(sporth_stack *stack, float val);
int sporth_stack_push_string(sporth_stack *stack, const char *str);
float sporth_stack_pop_float(sporth_stack *stack);
char * sporth_stack_pop_string(sporth_stack *stack);
int sporth_check_args(sporth_stack *stack, const char *args);
int sporth_register_func(sporth_data *sporth, sporth_func *flist);
int sporth_exec(sporth_data *sporth, const char *keyword);
int sporth_init(sporth_data *sporth);
int sporth_destroy(sporth_data *sporth);

int sporth_gettype(sporth_data *sporth, char *str, int mode);
int sporth_parse(sporth_data *sporth, const char *filename);
char * sporth_tokenizer(sporth_data *sporth, char *str,
        uint32_t size, uint32_t *pos);
int sporth_lexer(sporth_data *sporth, char *str, int32_t size);

enum {
SP_DUMMY = SPORTH_FOFFSET - 1,
SPORTH_ADD,
SPORTH_ADDv2,
SPORTH_SUB,
SPORTH_SUBv2,
SPORTH_MUL,
SPORTH_MULv2,
SPORTH_DIV,
SPORTH_DIVv2,
SPORTH_SINE,
SPORTH_CONSTANT,
SPORTH_MIX,
SPORTH_METRO,
SPORTH_TENV,
SPORTH_FM,
SPORTH_MTOF,
SPORTH_DUP,
SPORTH_SWAP,
SPORTH_DROP,
SPORTH_REVSC,
SPORTH_GEN_SINE,
SPORTH_OSC,
SPORTH_GEN_VALS,
SPORTH_TSEQ,
SPORTH_IN,
SPORTH_PORT,
SPORTH_NSMP,
SPORTH_PROP,
SPORTH_NOISE,
SPORTH_DCBLK,
SPORTH_BUTLP,
SPORTH_BUTHP,
SPORTH_MAYGATE,
SPORTH_RANDI,
SPORTH_ROT,
SPORTH_RPT,
SPORTH_REVERSE,
SPORTH_SAMPHOLD,
SPORTH_DELAY,
SPORTH_SWITCH,
SPORTH_MODE,
SPORTH_CLIP,
SPORTH_P,
SPORTH_PSET,
SPORTH_EQ,
SPORTH_LT,
SPORTH_GT,
SPORTH_NE,
SPORTH_BRANCH,
SPORTH_POS,
SPORTH_COUNT,
SPORTH_F,
SPORTH_GEN_SINESUM,
SPORTH_GEN_LINE,
SPORTH_DMETRO,
SPORTH_TIN,
SPORTH_GBUZZ,
SPORTH_JITTER,
SPORTH_DISKIN,
SPORTH_PLUCK,
SPORTH_JCREV,
SPORTH_SCALE,
SPORTH_TENV2,
SPORTH_MOOGLADDER,
SPORTH_VDELAY,
SPORTH_LAST
};

enum {
PLUMBER_CREATE,
PLUMBER_INIT,
PLUMBER_COMPUTE,
PLUMBER_DESTROY,
PLUMBER_OK,
PLUMBER_NOTOK,
PLUMBER_PANIC
};

enum {
DRIVER_FILE,
DRIVER_RAW
};

typedef struct plumber_ftbl {
    sp_ftbl *ft;
    char *name;
    struct plumber_ftbl *next;
} plumber_ftbl;

typedef struct {
    uint32_t nftbl;
    plumber_ftbl root;
    plumber_ftbl *last;
} plumber_ftentry;

typedef struct plumber_pipe {
    uint32_t type;
    size_t size;
    void *ud;
    struct plumber_pipe *next;
} plumber_pipe;

typedef struct {
    int (*fun)(sporth_stack *, void *);
    void *ud;
} sporth_func_d;


typedef struct plumber_data {
    int nchan;
    int mode;
    int seed;
    sp_data *sp;
    FILE *fp;
    char *filename;
    sporth_data sporth;
    sp_ftbl tbl_stack[32];
    uint32_t npipes;
    plumber_pipe root;
    plumber_pipe *last;

    plumber_ftentry ftmap[256];

    SPFLOAT p[16];
    int (*f[16])(sporth_stack *, void *);
    void *ud;
} plumber_data;

int plumber_init(plumber_data *plumb);
int plumber_register(plumber_data *plumb);
int plumber_clean(plumber_data *plumb);
int plumber_add_float(plumber_data *plumb, float num);
int plumber_add_string(plumber_data *plumb, const char *str);
int plumber_add_module(plumber_data *plumb,
        uint32_t id, size_t size, void *ud);
int plumber_compute(plumber_data *plumb, int mode);
int plumber_parse(plumber_data *plumb);
int plumber_parse_string(plumber_data *plumb, char *str);
int plumber_recompile(plumber_data *plumb);
int plumber_gettype(plumber_data *plumb, char *str, int mode);
int plumber_show_pipes(plumber_data *plumb);
int plumber_pipes_destroy(plumber_data *plumb);
int plumber_error(plumber_data *plumb, const char *str);
int plumber_ftmap_init(plumber_data *plumb);
int plumber_ftmap_add(plumber_data *plumb, const char *str, sp_ftbl *ft);
int plumber_ftmap_search(plumber_data *plumb, const char *str, sp_ftbl **ft);
int plumber_ftmap_destroy(plumber_data *plumb);
void sporth_run(plumber_data *pd, int argc, char *argv[],
    void *ud, void (*process)(sp_data *, void *));
