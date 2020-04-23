// 6.9 Bit-fields
// This is just informative. Don't run this.

#define KEYWORD 01
#define EXTRENAL 02
#define STATIC 04
// enum { KEYWORD = 01, EXTERNAL = 02, STATIC = 04 };
flags |= EXTERNAL | STATIC; // turns on the EXTERNAL and STATIC bits in flags
flags &= ~(EXTERNAL | STATIC); // turns off the EXTERNAL and STATIC bits in flags

struct {
    unsigned int is_keyword : 1;
    unsigned int is_extern : 1;
    unsigned int is_static : 1;
} flags;


