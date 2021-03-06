/**Copyright (C) Austin Hicks, 2014-2016
This file is part of Libaudioverse, a library for realtime audio applications.
This code is dual-licensed.  It is released under the terms of the Mozilla Public License version 2.0 or the Gnu General Public License version 3 or later.
You may use this code under the terms of either license at your option.
A copy of both licenses may be found in license.gpl and license.mpl at the root of this repository.
If these files are unavailable to you, see either http://www.gnu.org/licenses/ (GPL V3 or later) or https://www.mozilla.org/en-US/MPL/2.0/ (MPL 2.0).*/
#pragma once

namespace libaudioverse_implementation {

//Apparently, C's mod is in fact not the discrete math operation.
//This function handles that.
int ringmodi(int dividend, int divisor);
float ringmodf(float dividend, float divisor);
double ringmod(double dividend,double divisor);

//Complex absolute value.
float cabs(float real, float imag);

//Work with decibals.
//These two are for gain conversions, and assume 0 db is 1.0.
double gainToDb(double gain);
double dbToGain(double gain);

double scalarToDb(double scalar, double reference);
double dbToScalar(double db, double reference);

//Run euclid's algorithm on two integers:
//Only works on positive integers.
int greatestCommonDivisor(int a, int b);

//Fill a buffer with a hadamard matrix of order n (n must be power of 2).
void hadamard(int n, float* buffer, bool shouldNormalize=true);

//Fill a buffer with a matrix representing a reflectiona bout a plane whose normal is (1, 1, 1, 1...)
//This is also known as a householder matrix.
void householder(int n, float* buffer, bool shouldNormalize =true);
}