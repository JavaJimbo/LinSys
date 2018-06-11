#include "extcode.h"
#pragma pack(push)
#pragma pack(1)

#ifdef __cplusplus
extern "C" {
#endif

#define int32_t long
#define uint32_t unsigned long
#define uint16_t unsigned long
#define LVBoolean unsigned char


/*!
 * Function: determines polynomial coefficients which fit best with least 
 * square method for 3rd order to capacity and 2nd order to temperature. The 
 * coefficients were determined as double values and 3byte-Integer to write to 
 * EEPROM to PCap. 
 */
void __cdecl c_lin_coeff_dut(int32_t C_in[], double z[], double theta[], 
	int32_t C_fpp, int32_t C_int_fpp, int32_t z_fpp, LVBoolean inverse, 
	double cc[], uint32_t cc_fpp[], int32_t ccx[], double cci[], 
	double error_vs_z[], uint32_t cn_div[], int32_t cn_shift[], 
	int32_t n_samples, int32_t n_cc, int32_t n_cn_div, int32_t n_cn_shift);
/*!
 * Function: Determines nominal values from reference values and coefficients
 */
void __cdecl C_2p_nominal(double z[], double theta[], double cc[], 
	int32_t C_fpp, int32_t C_out_hex[], double C_out[], int32_t n_samples, 
	int32_t n_cc);
/*!
 * C_lin_coeff_batch
 */
void __cdecl C_lin_coeff_batch(double z[], int32_t z_fpp, double theta[], 
	int32_t C_in[], LVBoolean inverse, int32_t C_fpp, int32_t C_int_fpp, 
	uint16_t method, int32_t ci_mean_sel, double cc[], uint32_t cc_fpp[], 
	int32_t ccx[], double cci[], double best_fit[], uint32_t cn_div[], 
	int32_t cn_shift[], uint32_t n_samples, int32_t n_samples_x_devices, 
	int32_t n_cc, int32_t n_cn_div, int32_t n_cn_shift);
/*!
 * Function: determines polynomial coefficients which fit best with least 
 * square method for 3rd order to capacity and 2nd order to temperature. The 
 * coefficients were determined as double values and 3byte-Integer to write to 
 * EEPROM to PCap. 
 */
void __cdecl T_lin_coeff_dut(int32_t R_in[], double theta[], uint32_t r_fpp, 
	uint32_t r_int_fpp, uint32_t theta_fpp, LVBoolean r_inverse, double tc[], 
	uint32_t tc_fpp[], int32_t tcx[], double tci[], uint32_t tn_div[], 
	double error_vs_t[], int32_t n_samples, int32_t n_tn_div, int32_t n_tc);
/*!
 * Function: Determines nominal values from reference values and coefficients
 */
void __cdecl T_2p_nominal(double theta[], double tc[], int32_t R_fpp, 
	int32_t T_out_hex[], double T_out[], int32_t n_points, int32_t n_tc);
/*!
 * Rgp
 */
void __cdecl Rgp(double X[], double Y[], double *slope, double *offset, 
	int32_t n_input);

long __cdecl LVDLLStatus(char *errStr, int errStrLen, void *module);

#ifdef __cplusplus
} // extern "C"
#endif

#pragma pack(pop)

