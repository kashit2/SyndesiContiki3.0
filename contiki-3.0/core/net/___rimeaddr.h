
 
 #ifndef __RIMEADDR_H__
 #define __RIMEADDR_H__

#include "contiki-conf.h"

 #ifdef RIMEADDR_CONF_SIZE
 #define RIMEADDR_SIZE RIMEADDR_CONF_SIZE
 #else /* RIMEADDR_SIZE */
#define RIMEADDR_SIZE 2
 #endif /* RIMEADDR_SIZE */
typedef union {
  unsigned char u8[RIMEADDR_SIZE];
	} rimeaddr_t;
//00069 

 /**
00072  * \brief      Copy a Rime address
00073  * \param dest The destination
00074  * \param from The source
00075  *
00076  *             This function copies a Rime address from one location
00077  *             to another.
00078  *
00079  */
//00080 void rimeaddr_copy(rimeaddr_t *dest, const rimeaddr_t *from);
//00081 
/**
00083  * \brief      Compare two Rime addresses
00084  * \param addr1 The first address
00085  * \param addr2 The second address
00086  * \return     Non-zero if the addresses are the same, zero if they are different
00087  *
00088  *             This function compares two Rime addresses and returns
00089  *             the result of the comparison. The function acts like
00090  *             the '==' operator and returns non-zero if the addresses
00091  *             are the same, and zero if the addresses are different.
00092  *
00093  */ int rimeaddr_cmp(const rimeaddr_t *addr1, const rimeaddr_t *addr2);
//00095 
//00096 
/**
00098  * \brief      Set the address of the current node
00099  * \param addr The address
00100  *
00101  *             This function sets the Rime address of the node.
00102  *
00103  */
 void rimeaddr_set_node_addr(rimeaddr_t *addr);
 
 /**
00107  * \brief      The Rime address of the node
00108  *
00109  *             This variable contains the Rime address of the
00110  *             node. This variable should not be changed directly;
00111  *             rather, the rimeaddr_set_node_addr() function should be
00112  *             used.
00113  *
00114  */
 extern rimeaddr_t rimeaddr_node_addr;
 
/**
00118  * \brief      The null Rime address
00119  *
00120  *             This variable contains the null Rime address. The null
00121  *             address is used in route tables to indicate that the
00122  *             table entry is unused. Nodes with no configured address
00123  *             has the null address. Nodes with their node address set
00124  *             to the null address will have problems communicating
00125  *             with other nodes.
00126  *
00127  */
extern const rimeaddr_t rimeaddr_null;

 #endif /* __RIMEADDR_H__ */
/** @} */
/** @} */
