#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_FLASH_ADDRESS  0x0FE00000
#define CONFIG_FLASH_SIZE     2048 // TODO: why does it not work with 4096?
#define CONFIG_FLASH_ID       "ASD"
#define CONFIG_FLASH_VERSION  "1"

#define ID_SIZE 8
#define VALUE_SIZE 56

void configInit(void);
void clearConfig(void);
bool configExists(char *id);
int16_t getInt16(char *id);
uint32_t getUint32(char *id);
double getDouble(char *id);
char *getString(char *id);
void setInt16(char *id, int16_t val);
void setUint32(char *id, uint32_t val);
void setDouble(char *id, double val);
void setString(char *id, char *s);

void flushConfig(void);

#endif
