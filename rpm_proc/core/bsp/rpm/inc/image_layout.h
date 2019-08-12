#ifndef IMAGE_LAYOUT_H
#define IMAGE_LAYOUT_H

// This header is the API for components who have sections of data
// in special locations to find out where that data is in this particular image.
// Sections go from section_base to the byte before section_end. It is the clients'
// code's responsibility to validate that the space allocated to them for their section
// is big enough.

typedef struct
{
	char* section_base;
	char* section_end;
} rpm_image_section_info;

typedef enum
{
    RPM_IMAGE_INFO_HEADER,
	RPM_HEAP,
	RPM_STACK,
	RPM_CPR_STATS,
	RPM_SLEEP_STATS,
	RPM_LOG,
	RPM_IMAGE_SECTIONS_COUNT
} rpm_image_section_name;

extern const rpm_image_section_info rpm_image_section_array[RPM_IMAGE_SECTIONS_COUNT];

void rpm_adjust_free_heap_space(int heap_adjust);

int get_free_heap_space(void);

void rpm_free_heap_space_init(void);

#endif //IMAGE_LAYOUT_H
