/*==============================================================================

                        ds_json_ext_i.h

GENERAL DESCRIPTION
  Wrapper API for C to parse and generate JSON files

  Copyright (c) 2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/21/16    ml     Created file/Initial version.
==============================================================================*/
#ifndef DS_JSON_EXT_I_H
#define DS_JSON_EXT_I_H

#include "comdef.h"


/* List of error codes */
#define DS_JSON_SUCCESS 0

#define DS_JSON_ERROR_INVALID_INPUT     -1
#define DS_JSON_ERROR_SYNTAX_ERROR      -2
#define DS_JSON_ERROR_PARSE_FAILED      -3
#define DS_JSON_ERROR_SERIALIZE_FAILED  -4
#define DS_JSON_ERROR_SET_VALUE_FAILED  -5
#define DS_JSON_ERROR_VALUE_NOT_ARRAY   -6
#define DS_JSON_ERROR_PATH_NOT_FOUND    -7



// opaque type for c++ classes
typedef struct ds_json_handler ds_json_handler;
typedef struct ds_json_value   ds_json_value;



/* JSON types */
typedef enum
{
  DS_JSON_TYPE_UNSET = 0,
  DS_JSON_TYPE_NULL  = DS_JSON_TYPE_UNSET,
  DS_JSON_TYPE_NUMBER,
  DS_JSON_TYPE_STRING,
  DS_JSON_TYPE_BOOLEAN,
  DS_JSON_TYPE_ARRAY,
  DS_JSON_TYPE_OBJECT,
  DS_JSON_TYPE_MAX
} ds_json_value_type_e;



/* JSON String Type */
typedef struct
{
  const char* json_str;
  uint32      json_str_length;
} ds_json_string_s;



/* JSON values */
typedef union
{
  int32            number_value;
  ds_json_string_s string_value;
  boolean          bool_value;
  ds_json_value*   array_value;
  ds_json_value*   object_value;
} ds_json_value_u;



/* JSON value information */
typedef struct
{
  ds_json_value_type_e type;
  ds_json_value_u      value;
} ds_json_value_info_s;



/* Structure used to return serialized JSON in cache */
typedef struct
{
  const char* content;      // Pointer to generated content.
  uint32      content_size; // resulting content
} ds_json_serialize_content_s;





#ifdef __cplusplus
extern "C"
{
#endif


/*==============================================================================
//                              JSON Handler
==============================================================================*/
// Handler Constructor & Destructor

/*==============================================================================
FUNCTION ds_json_create_handler

DESCRIPTION
  Creates a JSON handler. This is needed to parse or serialize JSON content.

PARAMETERS
  None

RETURN VALUE
  On success, returns a pointer to an opaque type.
  On fail, NULL.

DEPENDENCIES
  None
==============================================================================*/
ds_json_handler* ds_json_create_handler();



/*==============================================================================
FUNCTION ds_json_destroy_handler

DESCRIPTION
  Deallocates memory for the json_handler.

PARAMETERS
  [In] json_handler - The handler to destroy

RETURN VALUE
  None

DEPENDENCIES
  None
==============================================================================*/
void ds_json_destroy_handler(ds_json_handler* json_handler);



/*==============================================================================
FUNCTION ds_json_get_root

DESCRIPTION
  Get root value object of the JSON handler

PARAMETERS
  [In] json_handler - JSON handler created by ds_json_create_handler()

RETURN VALUE
  Pointer to the JSON root
  NULL if the input JSON handler is NULL or invalid.

DEPENDENCIES
  None
==============================================================================*/
ds_json_value* ds_json_get_root(ds_json_handler* json_handler);



/*==============================================================================
FUNCTION json_parse_from_file

DESCRIPTION
  Parse JSON file content into the JSON handler

PARAMETERS
  [In] json_handler   - JSON handler created by ds_json_create_handler()
  [In] json_file_path - JSON file path to read from

RETURN VALUE
  DS_JSON_SUCCESS             - Success
  DS_JSON_ERROR_INVALID_INPUT - Invalid input parameters
  DS_JSON_ERROR_PARSE_FAILED  - Failed to parse file

DEPENDENCIES
  None
==============================================================================*/
int16 ds_json_parse_from_file(
                              ds_json_handler* json_handler,
                              const char*      json_file_path
                              );



/*==============================================================================
FUNCTION ds_json_parse_from_content

DESCRIPTION
  Parse JSON content into the JSON handler

PARAMETERS
  [In] json_handler - JSON handler created by ds_json_create_handler()
  [In] json_content - JSON content to parse
  [In] content_size - JSON content size

RETURN VALUE
  DS_JSON_SUCCESS             - Success
  DS_JSON_ERROR_INVALID_INPUT - Invalid input parameters
  DS_JSON_ERROR_PARSE_FAILED  - Failed to parse content

DEPENDENCIES
  None
==============================================================================*/
int16 ds_json_parse_from_content(
                                 ds_json_handler* json_handler,
                                 const char*      json_content,
                                 uint32           content_size
                                 );



// Generate JSON file/content
/*==============================================================================
FUNCTION ds_json_serialize_to_file

DESCRIPTION
  Serialize JSON handler contents to the specified file path

PARAMETERS
  [In] json_handler   - JSON handler created by ds_json_create_handler()
  [In] json_file_path - JSON file path to write to

RETURN VALUE
  DS_JSON_SUCCESS                - Success
  DS_JSON_ERROR_INVALID_INPUT    - Invalid input parameters
  DS_JSON_ERROR_SERIALIZE_FAILED - Failed to serialize to file

DEPENDENCIES
  None
==============================================================================*/
int16 ds_json_serialize_to_file(ds_json_handler* json_handler, const char* file_path);



/*==============================================================================
FUNCTION ds_json_serialize_to_content

DESCRIPTION
  Serialize JSON handler contents cache and return a pointer to it.
  Memory allocated for the generated JSON will be released when the handler is destroyed.

PARAMETERS
  [In] json_handler    - JSON handler created by ds_json_create_handler()
  [Out] result_content - JSON serialize result

RETURN VALUE
  DS_JSON_SUCCESS                - Success
  DS_JSON_ERROR_INVALID_INPUT    - Invalid input parameters
  DS_JSON_ERROR_SERIALIZE_FAILED - Failed to serialize to content

DEPENDENCIES
  None
==============================================================================*/
int16 ds_json_serialize_to_content(
                                   ds_json_handler*             json_handler,
                                   ds_json_serialize_content_s* result_content
                                   );






/*==============================================================================
                             JSON Accessor
==============================================================================*/
/*
// Access path format is same as json format. NULL/empty string will return/modify current value
// Sample JSON
{
  "name1" : {
    "name2" : [
      {
        "name3" : "value"
      }
    ]
  }
}

// Path to get "value"
name1.name2[0].name3
*/


/*==============================================================================
FUNCTION ds_json_get_value_obj_at_path

DESCRIPTION
  Get the value object at the specified path. The returned value object can be
    used for faster access to multiple internal nodes with common path.
    e.g. An internal array to iterate through

PARAMETERS
  [In] root_value - Root value to traverse
  [In] path       - JSON path to value; NULL or empty value will return the root

RETURN VALUE
  On success, returns a pointer to an opaque type.
  On fail, NULL.

DEPENDENCIES
  None
==============================================================================*/
ds_json_value* ds_json_get_value_obj_at_path(
                                             ds_json_value*  root_value,
                                             const char*     path
                                             );



/*==============================================================================
FUNCTION ds_json_get_value_obj_at_path

DESCRIPTION
  Get the value at the specified path.

PARAMETERS
  [In]  root_value - Root value to traverse
  [In]  path       - JSON path to value; NULL or empty value will return value
                       of the root
  [Out] value_info - Value at the specified path

RETURN VALUE
  DS_JSON_SUCCESS              - Success; value_info is set
  DS_JSON_ERROR_INVALID_INPUT  - Invalid input parameters
  DS_JSON_ERROR_PATH_NOT_FOUND - JSON path not found

DEPENDENCIES
  None
==============================================================================*/
int16 ds_json_get_value_info_at_path(
                                     ds_json_value*        root_value,
                                     const char*           path,
                                     ds_json_value_info_s* value_info
                                     );



/*==============================================================================
FUNCTION ds_json_set_value_at_path

DESCRIPTION
  Set the value at the specified path.

  Array/object will create an empty JSON array/object if value pointer is NULL

  If path does not exist or partially exists, it will create needed JSON objects
    accordingly. If part of a path is not a JSON object/array type, it will
    convert the data type into a JSON object/array.

  If a JSON value already exist in the given path, it will overwrite existing
    values with the new value.

  Note that it is possible to overwrite a JSON value even if the new value has a
    different type than the existing value.
    e.g. "object" : {...} -> "object" : "value"
         "object" : false -> "object" : [...]

PARAMETERS
  [In] root_value - Root value to traverse
  [In] path       - JSON path to value; NULL or empty value will return value
                       of the root object
  [In] value_info - Value at the specified path

RETURN VALUE
  DS_JSON_SUCCESS                - Success
  DS_JSON_ERROR_INVALID_INPUT    - Invalid input parameters
  DS_JSON_ERROR_SET_VALUE_FAILED - Failed to set value at path

DEPENDENCIES
  None
==============================================================================*/
int16 ds_json_set_value_at_path(
                                ds_json_value*              root_value,
                                const char*                 path,
                                const ds_json_value_info_s* value_info
                                );





/*==============================================================================
                              JSON Utils
==============================================================================*/

/*==============================================================================
FUNCTION ds_json_get_array_size

DESCRIPTION
  Get JSON array size. Only valid if root_value is DS_JSON_TYPE_Array.
    Otherwise it will return an error

PARAMETERS
  [In]  root_value - Value to get array size from
  [Out] list_size  - Name list output

RETURN VALUE
  DS_JSON_SUCCESS               - Success; list_size is set
  DS_JSON_ERROR_INVALID_INPUT   - Invalid input parameters
  DS_JSON_ERROR_VALUE_NOT_ARRAY - Input JSON value is not an array

DEPENDENCIES
  None
==============================================================================*/
int16 ds_json_get_array_size(ds_json_value* root_value, uint32* list_size);



/*==============================================================================
FUNCTION ds_json_get_value_type

DESCRIPTION
  Get JSON data type

PARAMETERS
  [In] root_value - Value to get JSON type from

RETURN VALUE
  JSON data type of the given value object

DEPENDENCIES
  None
==============================================================================*/
ds_json_value_type_e ds_json_get_value_type(ds_json_value* root_value);



/*==============================================================================
FUNCTION ds_json_validate_path

DESCRIPTION
  Validate if the path exists in the given root

PARAMETERS
  [In] root_value - Root to check path
  [In] path       - The path to validate

RETURN VALUE
  Returns true if path exists, otherwise returns false.

DEPENDENCIES
  None
==============================================================================*/
int16 ds_json_validate_path(ds_json_value* root_value, const char* path);



#ifdef __cplusplus
}
#endif

#endif /* DS_JSON_EXT_I_H */
