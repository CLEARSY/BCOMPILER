#ifndef BXML_API_H
#define BXML_API_H


// Prototype "a la C"
#if defined(__cplusplus)
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif

EXTERN int  m_component_bxml(char* mach_as, char *db_file, const char *converter_name,
                              char* output_directory);
//EXTERN int m_component_bxml(T_component *comp, const char* converterName, char *output_directory);


#endif // BXML_API_H
