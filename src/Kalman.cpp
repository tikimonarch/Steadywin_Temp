/* Copyright (C) 2012 Kristian Lauszus, TKJ Electronics. All rights reserved.

 This software may be distributed and modified under the terms of the GNU
 General Public License version 2 (GPL2) as published by the Free Software
 Foundation and appearing in the file GPL2.TXT included in the packaging of
 this file. Please note that GPL2 Section 2[b] requires that all works based
 on this software must also be made publicly available under the terms of
 the GPL2 ("Copyleft").

 Contact information
 -------------------

 Kristian Lauszus, TKJ Electronics
 Web      :  http://www.tkjelectronics.com
 e-mail   :  kristianl@tkjelectronics.com
 */

#include "Kalman.h"
#include <math.h>

Kalman::Kalman() {
    /* We will set the variables like so, these can also be tuned by the user */
    Q_angle = 0.001f;
    Q_rate = 0.63f;
    // R_measure = 20.0f;
    R_angle = 0.001f;
    R_rate = 40.0f;

    angle = 0.0f; // Reset the angle
    bias = 0.0f; // Reset bias

    P[0][0] = 0.0f; // Since we assume that the bias is 0 and we know the starting angle (use setAngle), the error covariance matrix is set like so - see: http://en.wikipedia.org/wiki/Kalman_filter#Example_application.2C_technical
    P[0][1] = 0.0f;
    P[1][0] = 0.0f;
    P[1][1] = 0.0f;

    R[0][0] = R_angle; // Since we assume that the bias is 0 and we know the starting angle (use setAngle), the error covariance matrix is set like so - see: http://en.wikipedia.org/wiki/Kalman_filter#Example_application.2C_technical
    R[0][1] = 0.0f;
    R[1][0] = 0.0f;
    R[1][1] = R_rate;
};

// The angle should be in degrees and the rate should be in degrees per second and the delta time in seconds
// float Kalman::getAngle(float newAngle, float newRate, float dt) {
//     // KasBot V2  -  Kalman filter module - http://www.x-firm.com/?page_id=145
//     // Modified by Kristian Lauszus
//     // See my blog post for more information: http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it

//     // Discrete Kalman filter time update equations - Time Update ("Predict")
//     // Update xhat - Project the state ahead
//     /* Step 1 */
//     // rate = newRate - bias;
//     angle += dt * rate;

//     // Update estimation error covariance - Project the error covariance ahead
//     /* Step 2 */
//     P[0][0] += dt * (dt*P[1][1] - P[0][1] - P[1][0] + Q_angle);
//     P[0][1] -= dt * P[1][1];
//     P[1][0] -= dt * P[1][1];
//     P[1][1] += Q_bias * dt;

//     // Discrete Kalman filter measurement update equations - Measurement Update ("Correct")
//     // Calculate Kalman gain - Compute the Kalman gain
//     /* Step 4 */
//     float S = P[0][0] + R_measure; // Estimate error
//     /* Step 5 */
//     float K[2]; // Kalman gain - This is a 2x1 vector
//     K[0] = P[0][0] / S;
//     K[1] = P[1][0] / S;

//     // Calculate angle and bias - Update estimate with measurement zk (newAngle)
//     /* Step 3 */
//     float y = newAngle - angle; // Angle difference
//     float ydot = newRate - rate;
//     /* Step 6 */
//     angle += K[0] * y;
//     rate += K[1] * ydot;

//     // Calculate estimation error covariance - Update the error covariance
//     /* Step 7 */
//     float P00_temp = P[0][0];
//     float P01_temp = P[0][1];

//     P[0][0] -= K[0] * P00_temp;
//     P[0][1] -= K[0] * P01_temp;
//     P[1][0] -= K[1] * P00_temp;
//     P[1][1] -= K[1] * P01_temp;

//     return angle;
// };

float Kalman::getAngle(float newAngle, float newRate, float torque, float dt) {
    float K[2][2]; // Kalman gain - This is a 2x2 vector
    float S[2][2]; // Kalman gain - This is a 2x2 vector
    /* Step 1 */
    angle = angle - rate*dt+torque*0.5*pow(dt,2)*6;
    rate = rate*dt + 206*dt*torque;

    // Update estimation error covariance - Project the error covariance ahead
    /* Step 2 */
    P[0][0] += dt * (dt*P[1][1] - P[0][1] - P[1][0]) + Q_angle;
    P[0][1] -= dt * P[1][1];
    P[1][0] -= dt * P[1][1];
    P[1][1] += Q_rate; 

    float temp = (P[0][0] + R[0][0]) * (P[1][1] + R[1][1]) - (P[0][1] + R[0][1]) * (P[1][0] + R[1][0]);
    float det_const; 
    if (temp != 0) 
        det_const = 1.0f / temp; 
    else 
        det_const = 10000.0f; 
    S[0][0] = det_const * (P[1][1] + R[0][1]); 
    S[0][1] = det_const * (-P[0][1]); 
    S[1][0] = det_const * (-P[1][0]); 
    S[1][1] = det_const * (P[0][0] + R[0][0]); 
    K[0][0] = P[0][0] * S[0][0] + P[0][1]*S[1][0]; 
    K[0][1] = P[0][0] * S[1][0] + P[1][0]*S[1][1]; 
    K[1][0] = P[1][0] * S[0][0] + P[1][1]*S[1][0]; 
    K[1][1] = P[1][0] * S[0][1] + P[1][1]*S[1][1];
    
    // K[0][0] = P[0][0] / (P[0][0]+R[0][0]);
    // K[0][1] = 1.0f;
    // K[1][0] = 1.0f;
    // K[1][1] = P[1][1] / (P[1][1]+R[0][1]);

    /* Step 3 */
    float y = newAngle - angle; // Angle difference
    float ydot = newRate - rate;
    /* Step 6 */
    angle += K[0][0] * y + K[0][1] * ydot;
    rate += K[1][0] * y + K[1][1] * ydot;

    float omk00 = 1 - K[0][0]; 
    float omk11 = 1 - K[1][1]; 
 
    float P_temp[2][2];
    P_temp[0][0] = P[0][0];
    P_temp[0][1] = P[0][1];
    P_temp[1][0] = P[1][0];
    P_temp[1][1] = P[1][1];  
 
    P[0][0] = omk00 * (omk00 * P_temp[0][0] - K[0][1]*P_temp[0][1]) - K[0][1] * (omk00 * P_temp[1][0] - K[0][1]*P_temp[1][1]); 
    P[0][1] = omk00 * (omk11 * P_temp[0][1] - K[1][0]*P_temp[0][0]) - K[0][1] * (omk11 * P_temp[1][1] - K[1][0]*P_temp[1][0]); 
     
    P[1][0] = omk11 * (omk00 * P_temp[1][0] - K[0][1]*P_temp[1][1]) - K[1][0] * (omk00 * P_temp[0][0] - K[0][1]*P_temp[0][1]); 
    P[1][1] = omk11 * (omk11 * P_temp[1][1] - K[1][0]*P_temp[1][0]) - K[1][0] * (omk11 * P_temp[0][1] - K[1][0]*P_temp[0][0]); 
 
    P[0][0] += K[0][0] * K[0][0] * R[0][0] + K[1][0] * K[1][0] * R[0][1]; 
    P[0][1] += K[0][0] * K[1][0] * R[0][0] + K[0][1] * K[1][1] * R[0][1]; 
     
    P[1][0] += K[0][0] * K[1][0] * R[0][0] + K[0][1] * K[1][1] * R[0][1]; 
    P[1][1] += K[1][0] * K[1][0] * R[0][0] + K[1][1] * K[1][1] * R[0][1];

    return angle;
};

void Kalman::setAngle(float angle) { this->angle = angle; }; // Used to set angle, this should be set as the starting angle
float Kalman::getRate() { return this->rate; }; // Return the unbiased rate

/* These are used to tune the Kalman filter */
void Kalman::setQangle(float Q_angle) { this->Q_angle = Q_angle; };
void Kalman::setQbias(float Q_rate) { this->Q_rate = Q_rate; };
void Kalman::setRmeasure(float R_angle) { this->R_angle = R_angle; };

float Kalman::getQangle() { return this->Q_angle; };
float Kalman::getQbias() { return this->Q_rate; };
float Kalman::getRmeasure() { return this->R_angle; };
