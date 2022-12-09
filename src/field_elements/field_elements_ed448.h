
#ifndef __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_H
#define __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_H

#ifdef __cplusplus
extern "C" {
#endif

#include <global_defs.h>

#if FE3C_32BIT && FE3C_64BIT
    #error "Both FE3C_32BIT and FE3C_64BIT set!"
#elif FE3C_32BIT
    #include <field_elements/field_elements_ed448_32.h>
#elif FE3C_64BIT
    #include <field_elements/field_elements_ed448_64.h>
#else
    #error "Architecture not defined. Set exactly one of FE3C_32BIT or FE3C_64BIT"
#endif /* FE3C_32BIT && FE3C_64BIT */

#ifdef __cplusplus
}
#endif

#endif /* __FE3C_FIELD_ELEMENTS_FIELD_ELEMENTS_ED448_H */
