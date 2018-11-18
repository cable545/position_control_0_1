#ifndef SENSOR_FUSION_H
#define	SENSOR_FUSION_H

//#define GYRO_ANGLES

#define TAU 0.0796

void complementary_filter(float* rate, float* angle, float* calc_angle);
void sensor_fusion(void);

#endif
