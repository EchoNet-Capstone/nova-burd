#ifndef STATE_MACROS_H
#define STATE_MACROS_H

#define SETTER_NEW(y) new_ ## y
#define SETTER_SIG(x, y) void set_ ## y(x SETTER_NEW(y))
#define GETTER_SIG(x, y) x get_ ## y(void)


#define GET_SET_FUNC_PROTO(x, y) SETTER_SIG(x, y);\
                             GETTER_SIG(x, y)

#define GET_SET_DEF(x, y, z)  x y = z; \
                            SETTER_SIG(x, y){\
                            y = SETTER_NEW(y);\
                            }\
                            GETTER_SIG(x, y){\
                            return y;\
                            }

#endif