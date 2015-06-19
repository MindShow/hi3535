#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif 

#define SUCCESS 0
#define FAILURE    -1

#define HISI_DEBUG_FLAG  1

#if HISI_DEBUG_FLAG

#define Printf(fmt...)\
    do{\
        printf("[File:%s - Func:%s() - Line:%d - Info:]=>", __FILE__,__FUNCTION__, __LINE__);\
        printf(fmt);\
    }while(0)

#define HISI_TRACE \
    do{\
        printf("[ %s, %s, %d ]=> ",__FILE__, __FUNCTION__,  __LINE__);\
        printf("\r\n");\
    }while(0)

#define HISI_DEBUG(msg...)\
    do{\
        printf("[ %s, %s, %d ]=> ",__FILE__, __FUNCTION__,  __LINE__);\
        printf(msg);\
        printf("\r\n");\
    }while(0)

#define HISI_ERROR(msg...)\
    do{\
        fprintf(stderr, "[ %s, %s, %d ]=> ",__FILE__,__FUNCTION__,  __LINE__);\
        printf(msg);\
        printf("\r\n")\
    }while(0)

#define HISI_ASSERT(expr...)\
    do{                                   \
        if (!(expr)) {                    \
            printf("\nASSERT failed at:\n"\
                   "  >File name: %s\n"   \
                   "  >Function : %s\n"   \
                   "  >Line No. : %d\n"   \
                   "  >Condition: %s\n",  \
                    __FILE__,__FUNCTION__, __LINE__, #expr);\
            _exit(-1);\
        } \
    }while(0)

#else

#define Printf(fmt...)      (void)(fmt)
#define HISI_TRACE
#define HISI_DEBUG(msg...)  (void)(msg)
#define HISI_ERROR(msg...)  (void)(msg)
#define HISI_ASSERT(exp...) (void)(exp)

#endif

#define cdr_int64_to(value)                             \
          value = ( (((value) & (0xffull << 56)) >> 56)           \
          | (((value) & (0xffull << 48)) >> 40)           \
          | (((value) & (0xffull << 40)) >> 24)           \
          | (((value) & (0xffull << 32)) >> 8)            \
          | (((value) & (0xffull << 24)) << 8)            \
          | (((value) & (0xffull << 16)) << 24)           \
          | (((value) & (0xffull << 8)) << 40)            \
          | (((value) & 0xffull) << 56) )


 #define cdr_int32_to(value)                     \
         value = ( (((value) & (0xfful << 24)) >> 24)     \
          | (((value) & (0xfful << 16)) >> 8)     \
          | (((value) & (0xfful << 8)) << 8)      \
          | (((value) & 0xfful) << 24) )

 #define cdr_int16_to(value)                     \
         value = ( (((value) & (0xffu << 8)) >> 8)        \
          | (((value) & 0xffu) << 8) )


#ifdef __cplusplus
}
#endif


#endif

