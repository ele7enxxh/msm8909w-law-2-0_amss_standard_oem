#ifndef KEYMASTER_TAGS_H
#define KEYMASTER_TAGS_H

/** General helper macro to create a bitmask from bits low to high. */
#define KEYMASTER_MASK_BITS(h,l)     ((0xffffffff >> (32 - ((h - l) + 1))) << l)

#define KEYMASTER_GET_TAG_TYPE(tag) (tag & KEYMASTER_MASK_BITS(31, 28) >> 28)

#endif //KEYMASTER_TAGS_H
