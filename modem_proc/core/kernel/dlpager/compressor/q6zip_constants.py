#if 0
def _enum(**kw):
  globals().update(kw)
#endif
#define _enum(...) enum { __VA_ARGS__ };

_enum(DICT1_BITS=10)
_enum(DICT2_BITS=14)
_enum(LB_BITS=8)
_enum(N_CHUNKS=2)
_enum(N_COMPRESSION_BLOCKS=2)
_enum(Q6ZIP_RO_ALGORITHM_VERSION=0x0600)
_enum(Q6ZIP_RW_ALGORITHM_VERSION=0x0200)
_enum(CHUNK_END_MARKER=0xFF)

