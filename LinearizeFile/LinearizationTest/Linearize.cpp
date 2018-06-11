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
*						Shift registers included.
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
#define NUMBER_OF_TEST_SAMPLES 27
#define NUMBER_OF_COEFFICIENTS 12
#define NUMBER_OF_DIVISION_STEPS 9
#define NUMBER_OF_TEMP_DECIMAL_PLACES 1
#define NUMBER_OF_Z_DECIMAL_PLACES 3
#define NUMBER_OF_SHIFT_VALUES 3

	long 	capRatio[MAXSAMPLES] 		= { 0x5353F7, 0x523D70, 0x51374B, 0x504189, 0x4F5C28, 0x4E76C8, 0x4D9168, 0x4CBC6A, 0x4C5A1C, 0x53126E, 0x51EB85, 0x50E560, 0x4FDF3B, 0x4EF9DB, 0x4E147A, 0x4D1EB8, 0x4C49BA, 0x4BE76C, 0x529FBE, 0x5178D4, 0x50624D, 0x4F4BC6, 0x4E5604, 0x4D6041, 0x4C6A7E, 0x4B851E, 0x4B126E };	
	double	measuredTemperature[MAXSAMPLES] 	= { 15.6, 15.6, 15.6, 15.6, 15.6, 15.6, 15.6, 15.6, 15.6, 25.1, 25.1, 25.1, 25.1, 25.1, 25.1, 25.1, 25.1, 25.1, 39.7, 39.7, 39.7, 39.7, 39.7, 39.7, 39.7, 39.7, 39.7 };
	double	z[MAXSAMPLES] 		= { 0.208, 0.309, 0.408, 0.507, 0.606, 0.704, 0.803, 0.901, 0.945, 0.208, 0.309, 0.408, 0.507, 0.606, 0.704, 0.803, 0.901, 0.945, 0.208, 0.309, 0.408, 0.507, 0.606,  0.704, 0.803, 0.901, 0.945 };	
	
// LinearizationTest.cpp : Defines the entry point for the console application.

BOOL loadInputDataFromBinaryFile(long *ptrCommand, long *ptrCapRatio, double *ptrTemperature, double *ptrZvalue);
BOOL CreateInputTestFile(long *ptrCommand, long *ptrCapRatio, double *ptrTemperature, double *ptrZvalue);
BOOL writeOutputDataToBinaryFile(long *ptrCommands, double *ptrCoefficients, unsigned long *ptrFPP, unsigned long *ptrDivisionSteps, long *ptrCnShift);

// int n_samples = 27;
#define n_samples (int) arrCommand[INDEX_NUM_SAMPLES]

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



// int _tmain(int argc, _TCHAR* argv[])
int main()
{
// #define n_cn_shift 	3 
#define C_fpp 		22
#define C_int_fpp	26
#define z_fpp		8
#define inverse 	1
	
	long arrCommand[MAXCOMMANDS] = {1, 27, 0};
		
	/* Outputs*/
	double	coefficients[NUMBER_OF_COEFFICIENTS];
	unsigned long 	cc_fpp[NUMBER_OF_COEFFICIENTS];
	long 	ccx[NUMBER_OF_COEFFICIENTS];
	double 	cci[NUMBER_OF_COEFFICIENTS];
	unsigned long 	divisionSteps[NUMBER_OF_DIVISION_STEPS];
	long 	cn_shift[NUMBER_OF_SHIFT_VALUES];
	double	error_vs_z[MAXSAMPLES];


	/* ----------------------------------------- */

	/* ------ Parameter for c_2p_nominal ------- */
	double z_nominal[] 		= { 22.8, 73.6 };
	double theta_nominal[]	= { 15.4, 39.7 };
	#define n_samples_2P	(sizeof(z_nominal) / sizeof(z_nominal[0]))

	/* Outputs*/
	//int 	C_out_hex[n_samples_2P]	;
	//double 	C_out[n_samples_2P]		;
	int i;

	loadInputDataFromBinaryFile (arrCommand, capRatio, measuredTemperature, z);
	// CreateInputTestFile(arrCommand, capRatio, measuredTemperature, z);
	
	/* ----------------------------------------- */
	

		/* ----------------- Start ----------------- */
	c_lin_coeff_dut (	capRatio, z, measuredTemperature, C_fpp, C_int_fpp, z_fpp,  inverse,	// input
						coefficients, cc_fpp, ccx, cci, error_vs_z, divisionSteps, cn_shift,	// output
						n_samples, NUMBER_OF_COEFFICIENTS, NUMBER_OF_DIVISION_STEPS, NUMBER_OF_SHIFT_VALUES);				// size of arrays (input)		

	writeOutputDataToBinaryFile(arrCommand, coefficients, cc_fpp, divisionSteps, cn_shift);
						
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
	
	// printf("\nPress any key to continue");
	// getch();	
	
	return 0;	/* return value main() */
	
} // EOF


BOOL writeOutputDataToBinaryFile(long *ptrCommands, double *ptrCoefficients, unsigned long *ptrFPP, unsigned long *ptrDivisionSteps, long *ptrCnShift)
{
	std::ofstream outFile;
	double dblValue;
	int i;

	// CREATE OUTPUT FILE
	outFile.open(OutputDataFileName, ios::out|ios::binary|ios::trunc);
	if (!outFile.is_open()) return false;

	// WRITE COMMAND HEADER TO FILE
	for (i = 0; i < MAXCOMMANDS; i++)
	{
		dblValue = (double) ptrCommands[i];
		outFile.write(reinterpret_cast<char *>(&dblValue), sizeof(dblValue));		
	} 

	// WRITE COEFFICIENTS TO FILE
	for (i = 0; i < NUMBER_OF_COEFFICIENTS; i++)
	{
		dblValue = (double) ptrCoefficients[i];
		outFile.write(reinterpret_cast<char *>(&dblValue), sizeof(dblValue));		
	} 
		
	// WRITE FPP TO FILE
	for (i = 0; i < NUMBER_OF_COEFFICIENTS; i++)
	{
		dblValue = (double) ptrFPP[i];
		outFile.write(reinterpret_cast<char *>(&dblValue), sizeof(dblValue));		
	} 
	
	// WRITE DIVISION STEPS TO FILE
	for (i = 0; i < NUMBER_OF_DIVISION_STEPS; i++)
	{
		dblValue = (double) ptrDivisionSteps[i];
		outFile.write(reinterpret_cast<char *>(&dblValue), sizeof(dblValue));		
	} 

	outFile.close();	
	return true;
}

BOOL loadInputDataFromBinaryFile(long *ptrCommand, long *ptrCapRatio, double *ptrTemperature, double *ptrZvalue)
{
	std::ifstream inFile;	
	long longValue;
	int numberOfSamples;
	int i;

	inFile.open(InputDataFileName, ios::in|ios::binary); 	
	if (!inFile.is_open()) return false;
	else 
	{
		// READ COMMAND HEADER FROM FILE
		for (i = 0; i < MAXCOMMANDS; i++)
		{ 
			inFile.read(reinterpret_cast<char *>(&longValue), sizeof(longValue));			
			ptrCommand[i] = longValue;
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
			inFile.read(reinterpret_cast<char *>(&longValue), sizeof(longValue));			
			ptrCapRatio[i] = longValue;
			if (inFile.eof()) 
			{
				inFile.close();
				return false;
			}			
		} 

		// READ MEASURED TEMPERATURES FROM FILE
		for (i = 0; i < numberOfSamples; i++)
		{ 
			inFile.read(reinterpret_cast<char *>(&longValue), sizeof(longValue));	
			ptrTemperature[i] = ((double)longValue) / pow((double)10, (double)NUMBER_OF_TEMP_DECIMAL_PLACES);
			if (inFile.eof()) 
			{
				inFile.close();
				return false;
			}			
		} 

		// READ Z VALUES FROM FILE 
		for (i = 0; i < numberOfSamples; i++)
		{ 
			inFile.read(reinterpret_cast<char *>(&longValue), sizeof(longValue));	
			ptrZvalue[i] = ((double)longValue) / pow((double)10, (double)NUMBER_OF_Z_DECIMAL_PLACES);
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

BOOL CreateInputTestFile(long *ptrCommand, long *ptrCapRatio, double *ptrTemperature, double *ptrZvalue)
{
	std::ofstream outFile;
	long longValue;
	int i;

	// CREATE OUTPUT FILE
	outFile.open(InputDataFileName, ios::out|ios::binary|ios::trunc);
	if (!outFile.is_open()) return false;

	// WRITE COMMAND HEADER TO FILE
	for (i = 0; i < MAXCOMMANDS; i++)
	{
		longValue = (long) ptrCommand[i];
		outFile.write(reinterpret_cast<char *>(&longValue), sizeof(longValue));		
	} 

	// WRITE TEST CAPACITOR RATIOS TO FILE
	for (i = 0; i < NUMBER_OF_TEST_SAMPLES; i++)
	{
		longValue = (long) ptrCapRatio[i];
		outFile.write(reinterpret_cast<char *>(&longValue), sizeof(longValue));		
	} 

	// WRITE TEST TEMPERATURES TO FILE
	for (i = 0; i < NUMBER_OF_TEST_SAMPLES; i++)
	{
		double dblTemperature = ptrTemperature[i];
		longValue = (long) (dblTemperature * pow((double)10, (double)NUMBER_OF_TEMP_DECIMAL_PLACES));
		outFile.write(reinterpret_cast<char *>(&longValue), sizeof(longValue));		
	} 

	// WRITE TEST Z VALUES TO FILE
	for (i = 0; i < NUMBER_OF_TEST_SAMPLES; i++)
	{
		longValue = (long) (ptrZvalue[i] * pow((double)10, (double)NUMBER_OF_Z_DECIMAL_PLACES));
		outFile.write(reinterpret_cast<char *>(&longValue), sizeof(longValue));		
	} 

	outFile.close();	
	return true;
}

