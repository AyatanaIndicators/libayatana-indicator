
#ifndef ___indicator_object_marshal_MARSHAL_H__
#define ___indicator_object_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:POINTER,UINT,UINT (./indicator-object-marshal.list:1) */
extern void _indicator_object_marshal_VOID__POINTER_UINT_UINT (GClosure     *closure,
                                                               GValue       *return_value,
                                                               guint         n_param_values,
                                                               const GValue *param_values,
                                                               gpointer      invocation_hint,
                                                               gpointer      marshal_data);

/* VOID:POINTER,UINT,ENUM (./indicator-object-marshal.list:2) */
extern void _indicator_object_marshal_VOID__POINTER_UINT_ENUM (GClosure     *closure,
                                                               GValue       *return_value,
                                                               guint         n_param_values,
                                                               const GValue *param_values,
                                                               gpointer      invocation_hint,
                                                               gpointer      marshal_data);

/* VOID:POINTER,UINT (./indicator-object-marshal.list:3) */
extern void _indicator_object_marshal_VOID__POINTER_UINT (GClosure     *closure,
                                                          GValue       *return_value,
                                                          guint         n_param_values,
                                                          const GValue *param_values,
                                                          gpointer      invocation_hint,
                                                          gpointer      marshal_data);

/* VOID:POINTER,BOOLEAN (./indicator-object-marshal.list:4) */
extern void _indicator_object_marshal_VOID__POINTER_BOOLEAN (GClosure     *closure,
                                                             GValue       *return_value,
                                                             guint         n_param_values,
                                                             const GValue *param_values,
                                                             gpointer      invocation_hint,
                                                             gpointer      marshal_data);

G_END_DECLS

#endif /* ___indicator_object_marshal_MARSHAL_H__ */

