#ifndef ANEMOMETER_H
#define ANEMOMETER_H

// Function prototypes for external use
void anemometer_init(void);
void anemometer_measure_and_update(void);
float getAnemometerSpeed_kmph(void);
float getAnemometerSpeed_mps(void);

#endif // ANEMOMETER_H