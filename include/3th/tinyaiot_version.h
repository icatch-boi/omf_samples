/**
 * \file tinyaiot_version.h
 *
 * \brief Run-time version information
 */

#ifndef TINYAIOT_VERSION_H
#define TINYAIOT_VERSION_H

/**
 * The version number x.y.z is split into three parts.
 * Major, Minor, Patchlevel
 */

#define TINYAIOT_VERSION_MAJOR  1
#define TINYAIOT_VERSION_MINOR  2
#define TINYIAOT_VERSION_PATCH  6

/**
 * The single version number has the following structure:
 *    MMNNPP00
 *    Major version | Minor version | Patch version
 */
#define TINYAIOT_VERSION_NUMBER         0x01020600
#define TINYAIOT_VERSION_STRING         "1.2.6"
#define TINYAIOT_VERSION_STRING_FULL    "tinyai AIoT 1.2.6"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Get the version number.
 *
 * \return          The constructed version number in the format
 *                  MMNNPP00 (Major, Minor, Patch).
 */
unsigned int tinyAIoT_VersionNumberGet( void );

/**
 * Get the version string ("x.y.z").
 *
 * \param string    The string that will receive the value.
 *                  (Should be at least 9 bytes in size)
 */
void tinyAIoT_VersionStringGet( char *string );

/**
 * Get the full version string ("tinyai AIoT x.y.z").
 *
 * \param string    The string that will receive the value. The tinyai AIoT version
 *                  string will use 21 bytes AT MOST including a terminating
 *                  null byte.
 *                  (So the buffer should be at least 21 bytes to receive this
 *                  version string).
 */
void tinyAIoT_VersionStringFullGet( char *string );

#ifdef __cplusplus
}
#endif

#endif /* tinyaiot_version.h */
