#ifndef FILTER_H
#define FILTER_H

#define MAG_FILTERING_FACTOR 0.075
#define ACC_FILTERING_FACTOR 0.1
#define GYRO_FILTERING_FACTOR 0.137

void mag_low_pass_filter(Imu_Data* new_mag_values);
void acc_low_pass_filter(Imu_Data* new_acc_values);
void gyro_low_pass_filter(Imu_Data* new_gyro_values);

#endif
