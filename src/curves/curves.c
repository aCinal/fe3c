#include <fe3c/eddsa.h>
#include <curves/curves.h>

#if FE3C_SUPPORT_CURVE_ED25519
extern curve ed25519_curve;
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
extern curve ed448_curve;
#endif /* FE3C_SUPPORT_CURVE_ED448 */

const curve *curves[EDDSA_NUMBER_OF_SUPPORTED_CURVES] = {

#if FE3C_SUPPORT_CURVE_ED25519
    [EDDSA_CURVE_ED25519] = &ed25519_curve,
#endif /* FE3C_SUPPORT_CURVE_ED25519 */

#if FE3C_SUPPORT_CURVE_ED448
    [EDDSA_CURVE_ED448] = &ed448_curve
#endif /* FE3C_SUPPORT_CURVE_ED448 */
};
