#ifndef FF_COMPAT_H
#define FF_COMPAT_H

/* At FatFS R0.10 ChaN changed the API to disk_read, disk_write,
 * f_mount, and various other functions.  Unfortunately, there is no
 * obvious way to detect which version is being used at compile-time
 * (_FATFS in ffconf.h is not monotonic).  So, somebody needs to
 * define something so a brother has a clue. */

#ifndef FATFS_IS_PRE_R0_10
/** Defined to true value if the version of FatFS being used is older
 * than R0.10.  By default, assume we're using R0.10 or later. */
#define FATFS_IS_PRE_R0_10 0
#endif /* FATFS_IS_PRE_R0_10 */

#endif /* FF_COMPAT_H */
