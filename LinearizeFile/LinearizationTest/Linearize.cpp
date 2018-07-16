/*---------------------------------------------------------------------------
*	Filename: Linearize.c
*	
*	Description:  Handling with acam linearization dll
*
*
*	--------------------------------------------------------------------
*	History:	Revision	Description			Date
*				1.0			Initial Version		24.06.2013
*   ---
*	--------------------------------------------------------------------
*	acam messelectronic Proprietary and Confidential
*	Copyright (c) 2013 by acam messelectronic gmbh
*	All rights reserved
*
*	6-11-18 Warwick: Added routines to read wnd write coefficients to binary files.
*			Shift registers included.
*	6-12-18 Warwick: Converted input routine to fetch floats
*	6-14-18 Boxborough: Modified for demo purposes - working version.
*			Commented out getch();
*	6-19-18 Warwick: Started coding the temperature routine - compiles OK so far.
*	7-09-18 Boxborough: fixed bug in convertCoefficientToHex()
*	7-09-18 Boxborough: eliminated need for convertCoefficientToHex() - using ccx[] array of integer coefficeints instead.
*   7-11-18 Warwick: Modified code to calculate coefficeints for either pressure or temperature.
*	7-15-18 Warwick: Created routines to test polynomial with coefficients: calculateZPressure(), writeCRatiosToBinaryFile(), writeCalDataToBinaryFile()
*	7-16-19 Warwick: Added routine to store coefficients for graphing: writeCoefficientsToBinaryFile()
*----------------------------------------------------------------------------*/

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include "linearize_r01.h"
#include "extcode.h"

#include <string.h>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <math.h>

using std::ios;   

#define CALCULATE_PRESSURE_COEFFICIENTS 0
#define CALCULATE_TEMPERATURE_COEFFICIENTS 1

CString InputDataFileName = "C:\\Temp\\CalDataInputFile";
CString OutputDataFileName = "C:\\Temp\\CalDataOutputFile";
CString OutputCoefficientFileName = "C:\\Temp\\CoefficientOutputFile";

#define printPrgHeader	printf("\n--------------------------------------------------------"); \
						printf("\n-------------- PCap02plus Linearization ----------------"); \
						printf("\n--------- (c) acam messelectronic gmbh, 2013 -----------"); \
						printf("\n--------------------------------------------------------\n\n");
 

void __cdecl c_lin_coeff_dut(int arrRatio[], double z[], double measuredTemperature[], 
	int C_fpp, int C_int_fpp, int z_fpp, int inverse, 
	double coefficients[], int cc_fpp[], int ccx[], double cci[], 
	double error_vs_z[], int divisionSteps[], int cn_shift[], 
	int n_samples, int n_cc, int n_cn_div, int n_cn_shift);			

void __cdecl C_2p_nominal(double z[], double measuredTemperature[], double coefficients[], 
	int C_fpp, int C_out_hex[], double C_out[], int n_samples, 
	int n_cc);					

#define MAXSAMPLES 33
#define NUMBER_OF_TEST_SAMPLES 15 // 27
#define NUMBER_OF_PRESSURE_COEFFICIENTS 12
#define NUMBER_OF_PRESSURE_DIVISION_STEPS 9
#define NUMBER_OF_PRESSURE_SHIFT_VALUES 3
#define NUMBER_OF_TEMPERATURE_COEFFICIENTS 4
#define NUMBER_OF_TEMPERATURE_DIVISION_STEPS 3
#define NUMBER_OF_TEMPERATURE_SHIFT_VALUES 0

//long 	arrRatio[MAXSAMPLES];
//double	z[MAXSAMPLES] = {0.0, 25, 50, 75, 100, 0, 25, 50, 75, 100, 0, 25, 50, 75, 100};
//double	measuredTemperature[MAXSAMPLES]	= {23.0, 23.0, 23.0, 23.0, 23.0, -20.0, -20.0, -20.0, -20.0, -20.0, 59.7, 59.7, 59.7, 59.7, 59.7};

long 	arrRatio[MAXSAMPLES] = { 0x5353F7, 0x523D70, 0x51374B, 0x504189, 0x4F5C28, 0x4E76C8, 0x4D9168, 0x4CBC6A, 0x4C5A1C, 0x53126E, 0x51EB85, 0x50E560, 0x4FDF3B, 0x4EF9DB, 0x4E147A, 0x4D1EB8, 0x4C49BA, 0x4BE76C, 0x529FBE, 0x5178D4, 0x50624D, 0x4F4BC6, 0x4E5604, 0x4D6041, 0x4C6A7E, 0x4B851E, 0x4B126E };	
double	measuredTemperature[MAXSAMPLES]	= { 15.6, 15.6, 15.6, 15.6, 15.6, 15.6, 15.6, 15.6, 15.6, 25.1, 25.1, 25.1, 25.1, 25.1, 25.1, 25.1, 25.1, 25.1, 39.7, 39.7, 39.7, 39.7, 39.7, 39.7, 39.7, 39.7, 39.7 };
double	z[MAXSAMPLES] = { 0.208, 0.309, 0.408, 0.507, 0.606, 0.704, 0.803, 0.901, 0.945, 0.208, 0.309, 0.408, 0.507, 0.606, 0.704, 0.803, 0.901, 0.945, 0.208, 0.309, 0.408, 0.507, 0.606,  0.704, 0.803, 0.901, 0.945 };	

double floCapRatio[NUMBER_OF_TEST_SAMPLES] = {1.105170, 1.125230, 1.148520, 1.176690, 1.212040, 1.105170, 1.125230, 1.148520, 1.176690, 1.212040, 1.105170, 1.125230, 1.148520, 1.176690, 1.212040};
unsigned long 	divisionSteps[] = {25,28,26, 24,24,24, 24,24,24};

BOOL writeOutputDataToBinaryFile(unsigned long *ptrCommands, unsigned long *ptrCoefficients, unsigned long *ptrDivisionSteps, long *ptrCnShift);
BOOL loadInputDataFromBinaryFile(unsigned long *ptrCommand, long *ptrCapRatio, double *ptrTemperature, double *ptrZvalue);
// BOOL CreateInputTestFile(long *ptrCommand, long *ptrRatio, double *ptrTemperature, double *ptrZvalue);
BOOL writeCoefficientsToBinaryFile(double *pCoefficients);

// double calculateZPressure (long CRatio, double t, double *pCoefficients);
// BOOL writeCRatiosToBinaryFile(long *pCRatio, double *Z, double *Zcalculated);
// bool writeCalDataToBinaryFile(long *ptrCommand, long *ptrCapRatio, double *ptrTemperature, double *ptrZvalue);

enum {
	INDEX_VERSION = 0,
	INDEX_NUM_SAMPLES,
	INDEX_ERRORS,
	INDEX_CALIBRATION_SELECT,
	SPARE2,
	SPARE3,
	SPARE4,
	SPARE5,
	MAXCOMMANDS
};


int main()
{
// #define n_cn_shift 	3 
#define C_fpp 		22 
#define C_int_fpp	26
#define z_fpp		8
#define inverse 	1
		
	unsigned long arrCommand[MAXCOMMANDS] = {1, NUMBER_OF_TEST_SAMPLES, 0}; 
		
	/* Outputs*/
	double	coefficients[NUMBER_OF_PRESSURE_COEFFICIENTS] = {1508.0352, -6487.8037, 10622.436, -5851.0303, 0, 0, -0.00000047683716, 0.00000017881393, 0, -0.000000029802322, 0.000000029802322, -0.00000001071021}; 
	unsigned long 	cc_fpp[NUMBER_OF_PRESSURE_COEFFICIENTS] = {8, 10, 9, 10, 15, 18, 21, 24, 22, 25, 28, 31};
	long 	ccx[NUMBER_OF_PRESSURE_COEFFICIENTS];
	double 	cci[NUMBER_OF_PRESSURE_COEFFICIENTS];
	unsigned long 	divisionSteps[NUMBER_OF_PRESSURE_DIVISION_STEPS];
	long 	cn_shift[NUMBER_OF_PRESSURE_SHIFT_VALUES] = {0,0,0};
	double	error_vs_z[MAXSAMPLES];
		
	/* ----------------------------------------- */

	/* ------ Parameter for c_2p_nominal ------- */
	double z_nominal[] 		= { 22.8, 73.6 };
	double theta_nominal[]	= { 15.4, 39.7 };
	#define n_samples_2P	(sizeof(z_nominal) / sizeof(z_nominal[0]))
	int i;
	long n_samples = 15;
	
	printf("WORKING V1.0:\n");
	printf("Reading data from binary input file:");	

	loadInputDataFromBinaryFile (arrCommand, arrRatio, measuredTemperature, z);		
	n_samples = (long) arrCommand[INDEX_NUM_SAMPLES];
	// arrCommand[INDEX_CALIBRATION_SELECT] = CALCULATE_PRESSURE_COEFFICIENTS;

	if (arrCommand[INDEX_CALIBRATION_SELECT] == CALCULATE_PRESSURE_COEFFICIENTS)
	{
		printf("\rPRESSURE CALIBRATION");
		printf("Number of input samples: %d\n", n_samples);
		for (i = 0; i < n_samples; i++)	
			printf("CRatio #%d: %08X, T = %f, Z = %f\n", i, arrRatio[i], measuredTemperature[i], z[i]);	
		
		/* ----------------- Start ----------------- */	
		c_lin_coeff_dut (arrRatio, z, measuredTemperature, C_fpp, C_int_fpp, z_fpp,  inverse,	// input
						coefficients, cc_fpp, ccx, cci, error_vs_z, divisionSteps, cn_shift,	// output
						n_samples, NUMBER_OF_PRESSURE_COEFFICIENTS, NUMBER_OF_PRESSURE_DIVISION_STEPS, NUMBER_OF_PRESSURE_SHIFT_VALUES);// size of arrays (input)			
		
		// double Zcalculated[MAXSAMPLES];
		// for (i = 0; i < 12; i++) Zcalculated[i] = calculateZPressure (arrRatio[i], 25.1, coefficients);
		// writeCRatiosToBinaryFile(arrRatio, z, Zcalculated);
		writeOutputDataToBinaryFile(arrCommand, (unsigned long *) ccx, divisionSteps, cn_shift);
		writeCoefficientsToBinaryFile(coefficients);
						
		printf("Coefficients for single devices -- c_lin_coeff_dut\n");
		printf("Calibration coefficients (ascending / [0..11])\n");
		for ( i=0; i<=NUMBER_OF_PRESSURE_COEFFICIENTS/4-1; i++ )		
			printf("k0%d %f\tk1%d %f\tk2%d %f\tk3%d %f\n", i, coefficients[i*4+0], i, coefficients[i*4+1], i, coefficients[i*4+2], i, coefficients[i*4+3]);			

		printf("\ncc_fpp (ascending / [0..11])\n");
		for ( i=0; i<=NUMBER_OF_PRESSURE_COEFFICIENTS/4-1; i++ )		
			printf("#%d %d\t#%d %d\t#%d %d\t#%d %d\n", i*4+0, cc_fpp[i*4+0], i*4+1, cc_fpp[i*4+1], i*4+2, cc_fpp[i*4+2], i*4+3, cc_fpp[i*4+3]);				
	
		printf("\ncn_div (ascending / [0..8])\n");
		for ( i=0; i<=NUMBER_OF_PRESSURE_DIVISION_STEPS/3-1; i++ )		
			printf("#%d %d\t#%d %d\t#%d %d\n", i*3+0, divisionSteps[i*3+0], i*3+1, divisionSteps[i*3+1], i*3+2, divisionSteps[i*3+2]);					

		printf("\nCN shifts (ascending / [0..2])\n");
		for ( i=0; i<NUMBER_OF_PRESSURE_SHIFT_VALUES; i++ )	
			printf("#%d %d\n", i, (int) cn_shift[i]);			

		printf ("\nPRESSURE COEFFICIENTS:");
		printf("\n k00: %08X", ccx[0]);
		printf("\n k10: %08X", ccx[1]);
		printf("\n k20: %08X", ccx[2]);
		printf("\n k30: %08X", ccx[3]); 

		printf("\n k01: %08X", ccx[4]);
		printf("\n k11: %08X", ccx[5]);
		printf("\n k21: %08X", ccx[6]);
		printf("\n k31: %08X", ccx[7]);

		printf("\n k02: %08X", ccx[8]);
		printf("\n k12: %08X", ccx[9]);
		printf("\n k22: %08X", ccx[10]);
		printf("\n k32: %08X", ccx[11]);
	}
	else
	{	
		// long R_in[50];
		// double theta[50];
		double tc[50], tci[50], error_vs_t[5];
		unsigned long r_fpp = 22, r_int_fpp = 25, theta_fpp = 8;
		unsigned char r_inverse = true;
		unsigned long tc_fpp[50]; // tn_div[50];
		long tcx[50];
		long n_tn_div = 3, n_tc = 4;

		printf("\rTEMPERATURE CALIBRATION");
		printf("Number of input samples: %d\n", n_samples);
		for (i = 0; i < n_samples; i++)	
			printf("RRatio #%d: %08X, T = %f\n", i, arrRatio[i], measuredTemperature[i]);	

		// T_lin_coeff_dut (R_in, theta, r_fpp, r_int_fpp, theta_fpp, r_inverse, tc, tc_fpp, tcx, tci, tn_div, error_vs_t, n_samples, n_tn_div, n_tc);	
		T_lin_coeff_dut (arrRatio, measuredTemperature, r_fpp, r_int_fpp, theta_fpp, r_inverse, tc, tc_fpp, tcx, tci, divisionSteps, error_vs_t, n_samples, n_tn_div, n_tc);	

		writeOutputDataToBinaryFile(arrCommand, (unsigned long *)tcx, divisionSteps, NULL);
						
		printf("Coefficients for single devices -- c_lin_coeff_dut\n");
		printf("Calibration coefficients (ascending / [0..3])\n");
		printf("tc0: %f\tc1: %f\tc2: %f\tc3: %f\n", coefficients[0], coefficients[1], coefficients[2], coefficients[3]);			

		printf("\ncc_fpp (ascending / [0..3])\n");
		printf("#0: %d\t#1: %d\t#2: %d\t#3: %d\n", tc_fpp[0], tc_fpp[1], tc_fpp[2], tc_fpp[3]);
	
		printf("\ncn_div (ascending / [0..2])\n");
		printf("#0: %d\t#1: %d\t#2: %d", divisionSteps[0], divisionSteps[1], divisionSteps[2]);
		
		printf ("\nTEMPERATURE COEFFICIENTS:");
		printf("\n tc0: %08X", tcx[0]);
		printf("\n tc1: %08X", tcx[1]);
		printf("\n tc2: %08X", tcx[2]);
		printf("\n tc3: %08X", tcx[3]); 
	}
	printf("\nPress any key to continue");
	getch();	
	
	return 0;	/* return value main() */
	
} // EOF

BOOL loadInputDataFromBinaryFile(unsigned long *ptrCommands, long *ptrRatio, double *ptrTemperature, double *ptrZvalue)
{
	std::ifstream inFile;		
	double doubleValue;
	int numberOfSamples;
	int i;

	inFile.open(InputDataFileName, ios::in|ios::binary); 	
	if (!inFile.is_open()) return false;
	else 
	{
		// READ COMMAND HEADER FROM FILE
		for (i = 0; i < MAXCOMMANDS; i++)
		{ 
			inFile.read(reinterpret_cast<char *>(&doubleValue), sizeof(doubleValue));			
			ptrCommands[i] = (unsigned long) doubleValue;
			if (inFile.eof()) 
			{
				inFile.close();
				return false;
			}
		} 

		numberOfSamples = (int) ptrCommands[INDEX_NUM_SAMPLES];
		if (numberOfSamples == 0 || numberOfSamples > MAXSAMPLES) return false;

		// READ MEASURED CAPACITANCE RATIOS FROM FILE
		for (i = 0; i < numberOfSamples; i++)
		{ 
			inFile.read(reinterpret_cast<char *>(&doubleValue), sizeof(doubleValue));			
			ptrRatio[i] = (long) doubleValue;
			if (inFile.eof()) 
			{
				inFile.close();
				return false;
			}			
		} 

		// READ MEASURED TEMPERATURES FROM FILE
		for (i = 0; i < numberOfSamples; i++)
		{ 
			inFile.read(reinterpret_cast<char *>(&doubleValue), sizeof(doubleValue));			
			ptrTemperature[i] = doubleValue;
			if (inFile.eof()) 
			{
				inFile.close();
				return false;
			}			
		} 

		if (ptrCommands[INDEX_CALIBRATION_SELECT] == CALCULATE_PRESSURE_COEFFICIENTS)
		{
			// READ Z VALUES FROM FILE 
			for (i = 0; i < numberOfSamples; i++)
			{ 
				inFile.read(reinterpret_cast<char *>(&doubleValue), sizeof(doubleValue));			
				ptrZvalue[i] = doubleValue;
				if (inFile.eof()) 
				{
					inFile.close();
					return false;
				}			
			} 
		}
		inFile.close();		
	}
	return TRUE;
}


BOOL writeOutputDataToBinaryFile(unsigned long *ptrCommands, unsigned long *ptrCoefficients, unsigned long *ptrDivisionSteps, long *ptrCnShift)
{
	std::ofstream outFile;
	unsigned long lngValue;
	unsigned int numberOfCoefficients, numberOfDivisionSteps, numberOfShiftValues;
	unsigned int i;

	if (ptrCommands[INDEX_CALIBRATION_SELECT] == CALCULATE_PRESSURE_COEFFICIENTS)
	{
		numberOfCoefficients = NUMBER_OF_PRESSURE_COEFFICIENTS;
		numberOfDivisionSteps = NUMBER_OF_PRESSURE_DIVISION_STEPS;
		numberOfShiftValues = NUMBER_OF_PRESSURE_SHIFT_VALUES;
	}
	else
	{
		numberOfCoefficients = NUMBER_OF_TEMPERATURE_COEFFICIENTS;
		numberOfDivisionSteps = NUMBER_OF_TEMPERATURE_DIVISION_STEPS;
		numberOfShiftValues = NUMBER_OF_TEMPERATURE_SHIFT_VALUES;
	}

	// CREATE OUTPUT FILE
	outFile.open(OutputDataFileName, ios::out|ios::binary|ios::trunc);
	if (!outFile.is_open()) return false;

	// WRITE COMMAND HEADER TO FILE
	for (i = 0; i < MAXCOMMANDS; i++)
	{
		lngValue = ptrCommands[i];
		outFile.write(reinterpret_cast<char *>(&lngValue), sizeof(lngValue));		
	} 

	// WRITE COEFFICIENTS TO FILE
	for (i = 0; i < numberOfCoefficients; i++)
	{
		lngValue = ptrCoefficients[i];
		outFile.write(reinterpret_cast<char *>(&lngValue), sizeof(lngValue));		
	} 		
	
	// WRITE DIVISION STEPS TO FILE
	for (i = 0; i < numberOfDivisionSteps; i++)
	{
		lngValue = ptrDivisionSteps[i];
		outFile.write(reinterpret_cast<char *>(&lngValue), sizeof(lngValue));		
	} 

	// WRITE CN SHIFTS TO FILE
	for ( i = 0; i < numberOfShiftValues; i++ )
	{
		if (ptrCnShift != NULL)
		{
			lngValue = (unsigned long) ptrCnShift[i];
			outFile.write(reinterpret_cast<char *>(&lngValue), sizeof(lngValue));
		}
	}

	outFile.close();	
	return true;
}


#define NUMBER_OF_COEFFICENTS 12
BOOL writeCoefficientsToBinaryFile(double *pCoefficients)
{
	std::ofstream outFile;
	double dblValue;
	unsigned int i;

	// CREATE OUTPUT FILE
	outFile.open(OutputCoefficientFileName, ios::out|ios::binary|ios::trunc);
	if (!outFile.is_open()) return false;

	// WRITE COEFFICIENTS TO FILE
	for (i = 0; i < NUMBER_OF_COEFFICENTS; i++)
	{
		dblValue = pCoefficients[i];
		outFile.write(reinterpret_cast<char *>(&dblValue), sizeof(dblValue));		
	} 		
	
	outFile.close();	
	return true;
}

/*
#define NUMBER_OF_SAMPLES 9
BOOL writeCRatiosToBinaryFile(long *pCRatio, double *Z, double *Zcalculated)
{	
	CString fileName = "CRatios.txt";
	CString strHexLine;
	std::ofstream myfile;
	unsigned int i;
	long offset;

	// CREATE OUTPUT FILE
	myfile.open(fileName);

	offset = pCRatio[0];
	for (i = 1; i < NUMBER_OF_SAMPLES; i++)
	{
		if (pCRatio[i] < offset)
			offset = pCRatio[i];
	}

	// offset = offset & 0x100000;

	// WRITE COEFFICIENTS TO FILE
	for (i = 0; i < NUMBER_OF_SAMPLES; i++)
	{
		strHexLine.Format("%0.6X, %d, %0.1f, %0.1f\r\n", pCRatio[i], (pCRatio[i] - offset), (Z[i] * 100.0), (Zcalculated[i] * 100.0));
		myfile << strHexLine;
	} 		
	
	myfile.close();	
	return true;
}


// This routine calculates the percent of ful scale pressure, or Z value,
// using the linearization polynomial: 
//  Z = (k3 * C^3) + (k2 * C^2) + (k1 * C) + k0
//
// The K values are corrected for temperature changes using the formulas:
//	k3 = (cc32 * t^2) + (cc31 * t) + cc30
//	k2 = (cc22 * t^2) + (cc21 * t) + cc20
//	k1 = (cc12 * t^2) + (cc11 * t) + cc10
//	k0 = (cc02 * t^2) + (cc01 * t) + cc00
//
//  Function inputs: 
//		CRatio - measured value of PCap capacitor ratio as a long integer
//		t - measured temp in Celsius
//		pCoefficients - the array of calculated polynomial coefficients
//
//  Returns: Result Z of polynomial
double calculateZPressure (long lngCRatio, double t, double *pCoefficients)
{
	double k3, k2, k1, k0;
	double cc00, cc10, cc20, cc30, cc01, cc11, cc21, cc31, cc02, cc12, cc22, cc32;
	double Zresult;
	double CRatio = 1.0 / ((double) lngCRatio / (double) 4194304);	// To use the measured C Ratio value in the polynomial, 
																	// divide by 2^22 (4194304) and invert result
																	// to get inverted C Ratio.

	if (pCoefficients == NULL) return 0;

	cc00 = pCoefficients[0];
	cc10 = pCoefficients[1];
	cc20 = pCoefficients[2];
	cc30 = pCoefficients[3];
	cc01 = pCoefficients[4];
	cc11 = pCoefficients[5];
	cc21 = pCoefficients[6];
	cc31 = pCoefficients[7];
	cc02 = pCoefficients[8];
	cc12 = pCoefficients[9];
	cc22 = pCoefficients[10];
	cc32 = pCoefficients[11];

	k3 = (cc32 * t * t) + (cc31 * t) + cc30;
	k2 = (cc22 * t * t) + (cc21 * t) + cc20;
	k1 = (cc12 * t * t) + (cc11 * t) + cc10;
	k0 = (cc02 * t * t) + (cc01 * t) + cc00;

	Zresult = (k3 * CRatio * CRatio * CRatio) + (k2 * CRatio * CRatio) + (k1 * CRatio) + k0;
	return Zresult;
}
*/