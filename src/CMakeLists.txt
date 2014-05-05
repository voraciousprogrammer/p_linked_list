ADD_LIBRARY(p_linked_list-static p_linked_list.h p_linked_list.c)
ADD_LIBRARY(p_linked_list SHARED p_linked_list.h p_linked_list.c)

SET_TARGET_PROPERTIES(p_linked_list PROPERTIES
                      VERSION ${p_linked_list_VERSION_STRING}
                      SOVERSION ${p_linked_list_VERSION_MAJOR}) 
