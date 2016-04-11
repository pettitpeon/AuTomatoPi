/*------------------------------------------------------------------------------
 *                             (c) 2015 by Ivan Peon
 *                             All rights reserved
 *------------------------------------------------------------------------------
 *   Module   : BaDbgMacros.h
 *   Date     : 14.03.2016
 *------------------------------------------------------------------------------
 *   Module description:
 */
/** @file
 *  Macros for debugging STL
 */
/*------------------------------------------------------------------------------
 */
#ifndef DADBGMACROS_H_
#define DADBGMACROS_H_

// String macros
#define STL_STRING(S)    (S._M_dataplus._M_p)

// Vector macros
#define STL_VEC_SZ(V)    (V._M_impl._M_finish - V._M_impl._M_start)
#define STL_VEC_PS(V, P) (*(mBuf._M_impl._M_start + P))

// Map macros
#define STL_MAP_SZ(M)    (M._M_t._M_impl._M_node_count)

#endif /* DADBGMACROS_H_ */
