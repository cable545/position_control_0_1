#ifndef RECEIVER_H
#define RECEIVER_H

#define MAX_CHANNELS			6
#define DEGREES_PER_COUNT 0.08F

// bigger count values than COUNT_BORDER_SYNC indicates the sync phase or a invalid count(2,2 ms)
#define COUNT_BORDER_SYNC 3298

// a count value of CHANNEL_VALUE_COUNT_MIN means a signal length of 1,0 ms
#define CHANNEL_VALUE_COUNT_MIN 1499

// a count value of CHANNEL_VALUE_MIDDLE means a signal length of 1,5 ms
#define CHANNEL_VALUE_MIDDLE 		2248

// a count value of CHANNEL_VALUE_COUNT_MAX means a signal length of 2,0 ms
#define CHANNEL_VALUE_COUNT_MAX 2998

#define FILTER_SWITCH_INDEX	4
#define FILTER_SWITCH_MIN 1550
#define FILTER_SWITCH_MAX 2900

#define CONTROLLER_SWITCH_INDEX 5
#define CONTROLLER_SWITCH_MIN 1550
#define CONTROLLER_SWITCH_MAX 2900

#define ROLL_R_MIN  CHANNEL_VALUE_COUNT_MIN
#define ROLL_R_MAX  CHANNEL_VALUE_COUNT_MAX
#define ROLL_A_MIN  -35
#define ROLL_A_MAX  35

#define PITCH_R_MIN  CHANNEL_VALUE_COUNT_MIN
#define PITCH_R_MAX  CHANNEL_VALUE_COUNT_MAX
#define PITCH_A_MIN  -35
#define PITCH_A_MAX  35

#define YAW_R_MIN	CHANNEL_VALUE_COUNT_MIN
#define YAW_R_MAX CHANNEL_VALUE_COUNT_MAX

void init_receiver(int set_interrupt, int set_dma);
int get_counts_from_angle(int* angle);
int get_productive_counts(unsigned short* counts);
void check_sum_signal_values(void);
void evaluate_rc_switch_values(void);
void wait_for_sec_stick_comb(void);

#endif
