/* Copyright (c) 2005-2007, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ---
 * Author: Markus Gutschke
 *
 * Code to extract a core dump snapshot of the current process.
 */
#ifndef _COREDUMP_H
#define _COREDUMP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Description of external compressor programs to use for creating compressed
 * coredumps. There are a few predefined compressor descriptions; callers
 * can also define their own compressors.
 * All functions expect an array of compressors. Array entries will be tried
 * in sequence until an executable compressor has been found. An empty string
 * in place of the compressor name signals that no compression should be
 * performed.
 * The end of the array is signalled by an entry that is completely zero'd out.
 */
struct CoredumperCompressor {
  const char *compressor; /* File name of compressor; e.g. gzip              */
  const char *const *args;/* execv()-style command line arguments            */
  const char *suffix;     /* Suffix that should be appended; e.g. .gz        */
};

/* Try compressing with either bzip2, gzip, or compress. If all of those fail,
 * fall back on generating an uncompressed file.
 */
extern const struct CoredumperCompressor COREDUMPER_COMPRESSED[];

/* Try compressing with a specific compressor. Fail if no compressor could
 * be found.
 */
extern const struct CoredumperCompressor COREDUMPER_BZIP2_COMPRESSED[];
extern const struct CoredumperCompressor COREDUMPER_GZIP_COMPRESSED[];
extern const struct CoredumperCompressor COREDUMPER_COMPRESS_COMPRESSED[];

/* Try compressing with a specific compressor. Fall back on generating an
 * uncompressed file, if the specified compressor is unavailable.
 */
extern const struct CoredumperCompressor COREDUMPER_TRY_BZIP2_COMPRESSED[];
extern const struct CoredumperCompressor COREDUMPER_TRY_GZIP_COMPRESSED[];
extern const struct CoredumperCompressor COREDUMPER_TRY_COMPRESS_COMPRESSED[];

/* Always create an uncompressed core file.
 */
extern const struct CoredumperCompressor COREDUMPER_UNCOMPRESSED[];

/* Returns a file handle that can be read to obtain a snapshot of the
 * current state of this process. If a core file could not be
 * generated for any reason, -1 is returned and "errno" will be set
 * appropriately.
 *
 * This function momentarily suspends all threads, while creating a
 * COW (copy-on-write) copy of the process's address space.
 *
 * This function is neither reentrant nor async signal safe. Callers
 * should wrap a mutex around the invocation of this function, if necessary.
 *
 * The current implementation tries very hard to behave reasonably when
 * called from a signal handler, but no guarantees are made that this will
 * always work. Most importantly, it is the caller's responsibility to
 * make sure that there are never more than one instance of GetCoreDump()
 * or WriteCoreDump() executing concurrently.
 */
int GetCoreDump(void);

/* Attempts to compress the core file on the fly, if a suitable compressor
 * could be located. Sets "selected_compressor" to the compressor that
 * was picked.
 */
int GetCompressedCoreDump(const struct CoredumperCompressor compressors[],
                          struct CoredumperCompressor **selected_compressor);

/* Writes the core file to disk. This is a convenience method wrapping
 * GetCoreDump(). If a core file could not be generated for any reason,
 * -1 is returned and errno is set appropriately. On success, zero is
 * returned.
 */
int WriteCoreDump(const char *file_name);

/* Callers might need to restrict the maximum size of the core file. This
 * convenience method provides the necessary support to emulate "ulimit -c".
 */
int WriteCoreDumpLimited(const char *file_name, size_t max_length);

/* Attempts to compress the core file on the fly, if a suitable compressor
 * could be located. Sets "selected_compressor" to the compressor that
 * was picked. The filename automatically has a suitable suffix appended
 * to it. Normally this would be ".bz2" for bzip2 compression ".gz" for
 * gzip compression, or ".Z" for compress compression. This behavior can
 * be changed by defining custom CoredumperCompressor descriptions.
 */
int WriteCompressedCoreDump(const char *file_name, size_t max_length,
                            const struct CoredumperCompressor compressors[],
                            struct CoredumperCompressor **selected_compressor);

#ifdef __cplusplus
}
#endif
#endif /* _COREDUMP_H */
