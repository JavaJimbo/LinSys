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

CString InputDataFileName = "C:\\Temp\\CalDataInputFile";
CString OutputDataFileName = "C:\\Temp\\CalDataOutputFile";

#define printPrgHeader	printf("\n--------------------------------------------------------"); \
						printf("\n-------------- PCap02plus Linearization ----------------"); \
						printf("\n--------- (c) acam messelectronic gmbh, 2013 -----------"); \
						printf("\n--------------------------------------------------------\n\n");
 

void __cdecl c_lin_coeff_dut(int capRatio[], double z[], double measuredTemperature[], 
	int C_fpp, int C_int_fpp, int z_fpp, int inverse, 
	double coefficients[], int cc_fpp[], int ccx[], double cci[], 
	double error_vs_z[], int divisionSteps[], int cn_shift[], 
	int n_samples, int n_cc, int n_cn_div, int n_cn_shift);			

void __cdecl C_2p_nominal(double z[], double measuredTemperature[], double coefficients[], 
	int C_fpp, int C_out_hex[], double C_out[], int n_samples, 
	int n_cc);					

#define MAXSAMPLES 33
#define NUMBER_OF_TEST_SAMPLES 15 // 27
#define NUMBER_OF_COEFFICIENTS 12
#define NUMBER_OF_DIVISION_STEPS 9
#define NUMBER_OF_SHIFT_VALUES 3

long 	capRatio[MAXSAMPLES];
double	z[MAXSAMPLES] = {0.0, 25, 50, 75, 100, 0, 25, 50, 75, 100, 0, 25, 50, 75, 100};
double	measuredTemperature[MAXSAMPLES]	= {23.0, 23.0, 23.0, 23.0, 23.0, -20.0, -20.0, -20.0, -20.0, -20.0, 59.7, 59.7, 59.7, 59.7, 59.7};
double floCapRatio[NUMBER_OF_TEST_SAMPLES] = {1.105170, 1.125230, 1.148520, 1.176690, 1.212040, 1.105170, 1.125230, 1.148520, 1.176690, 1.212040, 1.105170, 1.125230, 1.148520, 1.176690, 1.212040};
unsigned long 	divisionSteps[] = {25,28,26, 24,24,24, 24,24,24};

BOOL writeOutputDataToBinaryFile(unsigned long *ptrCommands, unsigned long *ptrCoefficients, unsigned long *ptrDivisionSteps, long *ptrCnShift);
BOOL loadInputDataFromBinaryFile(unsigned long *ptrCommand, long *ptrCapRatio, double *ptrTemperature, double *ptrZvalue);
BOOL CreateInputTestFile(long *ptrCommand, long *ptrCapRatio, double *ptrTemperature, double *ptrZvalue);

bool writeCalDataToBinaryFile(long *ptrCommand, long *ptrCapRatio, double *ptrTemperature, double *ptrZvalue);

bool convertCoefficientToHex(double decimalValue, double FPPvalue, unsigned long *ptrTwosComplementHex);
unsigned long HEXcoefficients[NUMBER_OF_COEFFICIENTS];

enum {
	INDEX_VERSION = 0,
	INDEX_NUM_SAMPLES,
	INDEX_ERRORS,
	SPARE1,
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
	double	coefficients[NUMBER_OF_COEFFICIENTS] = {1508.0352, -6487.8037, 10622.436, -5851.0303, 0, 0, -0.00000047683716, 0.00000017881393, 0, -0.000000029802322, 0.000000029802322, -0.00000001071021}; 
	unsigned long 	cc_fpp[NUMBER_OF_COEFFICIENTS] = {8, 10, 9, 10, 15, 18, 21, 24, 22, 25, 28, 31};
	long 	ccx[NUMBER_OF_COEFFICIENTS];
	double 	cci[NUMBER_OF_COEFFICIENTS];
	unsigned long 	divisionSteps[NUMBER_OF_DIVISION_STEPS];
	long 	cn_shift[NUMBER_OF_SHIFT_VALUES] = {0,0,0};
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
	loadInputDataFromBinaryFile (arrCommand, capRatio, measuredTemperature, z);		
	n_samples = (long) arrCommand[INDEX_NUM_SAMPLES];

	printf("Number of input samples: %d\n", n_samples);
	for (i = 0; i < n_samples; i++)	
		printf("CRatio #%d: %08X, T = %f, Z = %f\n", i, capRatio[i], measuredTemperature[i], z[i]);	
		
	/* ----------------- Start ----------------- */
	
	c_lin_coeff_dut (capRatio, z, measuredTemperature, C_fpp, C_int_fpp, z_fpp,  inverse,	// input
						coefficients, cc_fpp, ccx, cci, error_vs_z, divisionSteps, cn_shift,	// output
						n_samples, NUMBER_OF_COEFFICIENTS, NUMBER_OF_DIVISION_STEPS, NUMBER_OF_SHIFT_VALUES);				// size of arrays (input)		
	

	for (i = 0; i < NUMBER_OF_COEFFICIENTS; i++) convertCoefficientToHex(coefficients[i], cc_fpp[i], &HEXcoefficients[i]);			
	writeOutputDataToBinaryFile(arrCommand, HEXcoefficients, divisionSteps, cn_shift);
						
	printf("Coefficients for single devices -- c_lin_coeff_dut\n");
	printf("Calibration values coefficients (ascending / [0..11])\n");
	for ( i=0; i<=NUMBER_OF_COEFFICIENTS/4-1; i++ )
	{
		printf("k0%d %f\tk1%d %f\tk2%d %f\tk3%d %f\n", i, coefficients[i*4+0], i, coefficients[i*4+1], i, coefficients[i*4+2], i, coefficients[i*4+3]);
	}	

	printf("\ncc_fpp (ascending / [0..11])\n");
	for ( i=0; i<=NUMBER_OF_COEFFICIENTS/4-1; i++ )
	{
		printf("#%d %d\t#%d %d\t#%d %d\t#%d %d\n", i*4+0, cc_fpp[i*4+0], i*4+1, cc_fpp[i*4+1], i*4+2, cc_fpp[i*4+2], i*4+3, cc_fpp[i*4+3]);
	}		
	
	printf("\ncn_div (ascending / [0..8])\n");
	for ( i=0; i<=NUMBER_OF_DIVISION_STEPS/3-1; i++ )
	{
		printf("#%d %d\t#%d %d\t#%d %d\n", i*3+0, divisionSteps[i*3+0], i*3+1, divisionSteps[i*3+1], i*3+2, divisionSteps[i*3+2]);		
	}		

	printf("\nCN shifts (ascending / [0..2])\n");
	for ( i=0; i<NUMBER_OF_SHIFT_VALUES; i++ )
	{
		printf("#%d %d\n", i, (int) cn_shift[i]);		
	}
		
	printf("\n k00: %08X", HEXcoefficients[0]);
	printf("\n k10: %08X", HEXcoefficients[1]);
	printf("\n k20: %08X", HEXcoefficients[2]);
	printf("\n k30: %08X", HEXcoefficients[3]); 

	printf("\n k01: %08X", HEXcoefficients[4]);
	printf("\n k11: %08X", HEXcoefficients[5]);
	printf("\n k21: %08X", HEXcoefficients[6]);
	printf("\n k31: %08X", HEXcoefficients[7]);

	printf("\n k02: %08X", HEXcoefficients[8]);
	printf("\n k12: %08X", HEXcoefficients[9]);
	printf("\n k22: %08X", HEXcoefficients[10]);
	printf("\n k32: %08X", HEXcoefficients[11]);

	//printf("\nPress any key to continue");
	//getch();	
	
	return 0;	/* return value main() */
	
} // EOF


BOOL loadInputDataFromBinaryFile(unsigned long *ptrCommand, long *ptrCapRatio, double *ptrTemperature, double *ptrZvalue)
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
			ptrCommand[i] = (unsigned long) doubleValue;
			if (inFile.eof()) 
			{
				inFile.close();
				return false;
			}
		} 

		numberOfSamples = (int) ptrCommand[INDEX_NUM_SAMPLES];
		if (numberOfSamples == 0 || numberOfSamples > MAXSAMPLES) return false;

		// READ MEASURED CAPACITANCE RATIOS FROM FILE
		for (i = 0; i < numberOfSamples; i++)
		{ 
			inFile.read(reinterpret_cast<char *>(&doubleValue), sizeof(doubleValue));			
			ptrCapRatio[i] = (long) doubleValue;
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
		inFile.close();		
	}
	return TRUE;
}


bool convertCoefficientToHex(double decimalValue, double FPPvalue, unsigned long *ptrTwosComplementHex)
{
	double actualValue;
	bool negFlag = false;

	if (ptrTwosComplementHex == NULL) return false;

	if (decimalValue == 0)
	{
		ptrTwosComplementHex[0] = 0x00;
		ptrTwosComplementHex[1] = 0x00;
		ptrTwosComplementHex[2] = 0x00;
		ptrTwosComplementHex[3] = 0x00;
		return true;
	}

	double multiplier = pow ((double)2.0, (double)FPPvalue);	
	actualValue = decimalValue * multiplier;

	if (actualValue < 0)
	{
		actualValue = 0 - actualValue;
		negFlag = true;
	}

	unsigned long hexValue;
	
	hexValue = (unsigned long) actualValue;
	// If integer is negative, get two's complement:
	if (negFlag)
	{
		hexValue = hexValue - 1;
		hexValue = (~hexValue) & 0xFFFFFFFF;
	}

	*ptrTwosComplementHex = hexValue;
	return true;
}

BOOL writeOutputDataToBinaryFile(unsigned long *ptrCommands, unsigned long *ptrCoefficients, unsigned long *ptrDivisionSteps, long *ptrCnShift)
{
	std::ofstream outFile;
	unsigned long lngValue;
	int i;

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
	for (i = 0; i < NUMBER_OF_COEFFICIENTS; i++)
	{
		lngValue = ptrCoefficients[i];
		outFile.write(reinterpret_cast<char *>(&lngValue), sizeof(lngValue));		
	} 		
	
	// WRITE DIVISION STEPS TO FILE
	for (i = 0; i < NUMBER_OF_DIVISION_STEPS; i++)
	{
		lngValue = ptrDivisionSteps[i];
		outFile.write(reinterpret_cast<char *>(&lngValue), sizeof(lngValue));		
	} 

	// WRITE CN SHIFTS TO FILE
	for ( i = 0; i < NUMBER_OF_SHIFT_VALUES; i++ )
	{
		lngValue = (unsigned long) ptrCnShift[i];
		outFile.write(reinterpret_cast<char *>(&lngValue), sizeof(lngValue));
	}

	outFile.close();	
	return true;
}
