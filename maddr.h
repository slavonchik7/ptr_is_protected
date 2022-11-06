
#ifndef MADDR_H
#define MADDR_H

#if __WORDSIZE == 64
    typedef unsigned long int addr64_t;
    typedef addr64_t addr_t;
#else
    typedef unsigned long int addr32_t;
    typedef addr32_t addr_t;
#endif /* __WORDSIZE */

typedef addr_t ptr_id_t;

#endif /* MADDR_H */