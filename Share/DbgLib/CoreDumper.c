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
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "CoreDumper.h"
#include "ELFCore.h"
#include "LinuxSyscallSupport.h"
#include "LinuxThreads.h"
#include "ThreadLister.h"

static const char *const no_args_bzip2[]    = { "bzip2",    NULL };
static const char *const no_args_gzip[]     = { "gzip",     NULL };
static const char *const no_args_compress[] = { "compress", NULL };
const struct CoredumperCompressor COREDUMPER_COMPRESSED[] = {
  { "/bin/bzip2",        no_args_bzip2,    ".bz2" },
  { "/usr/bin/bzip2",    no_args_bzip2,    ".bz2" },
  { "bzip2",             no_args_bzip2,    ".bz2" },
  { "/bin/gzip",         no_args_gzip,     ".gz"  },
  { "/usr/bin/gzip",     no_args_gzip,     ".gz"  },
  { "gzip",              no_args_gzip,     ".gz"  },
  { "/bin/compress",     no_args_compress, ".Z"   },
  { "/usr/bin/compress", no_args_compress, ".Z"   },
  { "compress",          no_args_compress, ".Z"   },
  { "",                  0,                ""     },
  { 0,                   0,                0      } };
const struct CoredumperCompressor COREDUMPER_BZIP2_COMPRESSED[] = {
  { "/bin/bzip2",        no_args_bzip2,    ".bz2" },
  { "/usr/bin/bzip2",    no_args_bzip2,    ".bz2" },
  { "bzip2",             no_args_bzip2,    ".bz2" },
  { 0,                   0,                0      } };
const struct CoredumperCompressor COREDUMPER_GZIP_COMPRESSED[] = {
  { "/bin/gzip",         no_args_gzip,     ".gz"  },
  { "/usr/bin/gzip",     no_args_gzip,     ".gz"  },
  { "gzip",              no_args_gzip,     ".gz"  },
  { 0,                   0,                0      } };
const struct CoredumperCompressor COREDUMPER_COMPRESS_COMPRESSED[] = {
  { "/bin/compress",     no_args_compress, ".Z"   },
  { "/usr/bin/compress", no_args_compress, ".Z"   },
  { "compress",          no_args_compress, ".Z"   },
  { 0,                   0,                0      } };
const struct CoredumperCompressor COREDUMPER_TRY_BZIP2_COMPRESSED[] = {
  { "/bin/bzip2",        no_args_bzip2,    ".bz2" },
  { "/usr/bin/bzip2",    no_args_bzip2,    ".bz2" },
  { "bzip2",             no_args_bzip2,    ".bz2" },
  { "",                  0,                ""     },
  { 0,                   0,                0      } };
const struct CoredumperCompressor COREDUMPER_TRY_GZIP_COMPRESSED[] = {
  { "/bin/gzip",         no_args_gzip,     ".gz"  },
  { "/usr/bin/gzip",     no_args_gzip,     ".gz"  },
  { "gzip",              no_args_gzip,     ".gz"  },
  { "",                  0,                ""     },
  { 0,                   0,                0      } };
const struct CoredumperCompressor COREDUMPER_TRY_COMPRESS_COMPRESSED[] = {
  { "/bin/compress",     no_args_compress, ".Z"   },
  { "/usr/bin/compress", no_args_compress, ".Z"   },
  { "compress",          no_args_compress, ".Z"   },
  { "",                  0,                ""     },
  { 0,                   0,                0      } };
const struct CoredumperCompressor COREDUMPER_UNCOMPRESSED[] = {
  { "",                  0,                ""     },
  { 0,                   0,                0      } };


#ifndef DUMPER
/* If the target platform lacks the necessary support for generating core dumps
 * on the fly, or if nobody has ported the code, then return an error.
 */
typedef void *Frame;
#define FRAME(f) void *f = &&label; label:


int InternalGetCoreDump(void *frame, int num_threads, pid_t *thread_pids,
                        va_list ap) {
  errno = EINVAL;
  return -1;
}
#endif


/* Returns a file handle that can be read to obtain a snapshot of the
 * current state of this process. If a core file could not be
 * generated for any reason, -1 is returned.
 *
 * This function momentarily suspends all threads, while creating a
 * COW copy of the process's address space.
 *
 * This function is neither reentrant nor async signal safe. Callers
 * should wrap a mutex around the invocation of this function, if necessary.
 *
 * The current implementation tries very hard to do behave reasonably when
 * called from a signal handler, but no guarantees are made that this will
 * always work.
 */
int GetCoreDump(void) {
  FRAME(frame);
  return ListAllProcessThreads(&frame, InternalGetCoreDump,
                               (void *)0, (size_t)0,
                               (void *)0, (void *)0, (void *)0);
}

/* Attempts to compress the core file on the fly, if a suitable compressor
 * could be located. Sets "selected_compressor" to the compressor that
 * was picked.
 */
int GetCompressedCoreDump(const struct CoredumperCompressor compressors[],
                          struct CoredumperCompressor **selected_compressor) {
  FRAME(frame);
  return ListAllProcessThreads(&frame, InternalGetCoreDump,
                               (void *)0, (size_t)0,
                               getenv("PATH"),compressors,selected_compressor);
}

/* Re-runs fn until it doesn't cause EINTR.
 */
#define NO_INTR(fn)   do {} while ((fn) < 0 && errno == EINTR)


/* Internal helper method used by WriteCoreDump().
 */
static int WriteCoreDumpFunction(void *frame, const char *file_name,
                           size_t max_length,
                           const struct CoredumperCompressor compressors[],
                           struct CoredumperCompressor **selected_compressor) {
  return ListAllProcessThreads(frame, InternalGetCoreDump,
                               file_name, max_length, getenv("PATH"),
                               compressors, selected_compressor);
}

/* Writes the core file to disk. This is a convenience method wrapping
 * GetCoreDump(). If a core file could not be generated for any reason,
 * -1 is returned. On success, zero is returned.
 */
int WriteCoreDump(const char *file_name) {
  FRAME(frame);
  return WriteCoreDumpFunction(&frame, file_name, SIZE_MAX,
                               (void *)0, (void *)0);
}

/* Callers might need to restrict the maximum size of the core file. This
 * convenience method provides the necessary support to emulate "ulimit -c".
 */
int WriteCoreDumpLimited(const char *file_name, size_t max_length) {
  FRAME(frame);
  return WriteCoreDumpFunction(&frame, file_name, max_length,
                               (void *)0, (void *)0);
}

/* Attempts to compress the core file on the fly, if a suitable compressor
 * could be located. Sets "selected_compressor" to the compressor that
 * was picked. The filename automatically has a suitable suffix appended
 * to it. Normally this would be ".bz2" for bzip2 compression ".gz" for
 * gzip compression, or ".Z" for compress compression. This behavior can
 * be changed by defining custom CoredumperCompressor descriptions.
 */
int WriteCompressedCoreDump(const char *file_name, size_t max_length,
                            const struct CoredumperCompressor compressors[],
                            struct CoredumperCompressor **selected_compressor){
  FRAME(frame);
  return WriteCoreDumpFunction(&frame, file_name, max_length,
                               compressors, selected_compressor);
}
