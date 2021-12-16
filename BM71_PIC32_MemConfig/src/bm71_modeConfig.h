
#ifndef _EXAMPLE_FILE_NAME_H    /* Guard against multiple inclusion */
#define _EXAMPLE_FILE_NAME_H


/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
    BM_MODE_PROGRAM = 0,
    BM_MODE_APPLICATION
}BM_MODE;
    
    
    /* ************************************************************************** */
    /* Section: Constants                                                         */
    /* ************************************************************************** */
#define BM_RST_DIR                  TRISGbits.TRISG13
#define BM_P20_DIR                  TRISGbits.TRISG13
    
#define BM_RST_SET                  GPIO_RG13_Set()
#define BM_RST_CLEAR                GPIO_RG13_Clear()

#define BM_P20_SET                  GPIO_RG14_Set()
#define BM_P20_CLEAR                GPIO_RG14_Clear()

#define BM_MODULE_ProgramModeSet()    ({     uint32_t delay_cnt;                                         \
                                    BM_P20_CLEAR;                                                 \
                                    delay_cnt = 1800U;                                          \
                                    do                                                          \
                                    {                                                           \
                                        delay_cnt--;                                            \
                                    }while(delay_cnt);                                          \
                                    BM_RST_CLEAR;                                                 \
                                    delay_cnt = 1800U;                                          \
                                    do                                                          \
                                    {                                                           \
                                        delay_cnt--;                                            \
                                    }while(delay_cnt);                                          \
                                    BM_RST_SET;                                                 \
                                    delay_cnt = 1800000U;                                          \
                                    do                                                          \
                                    {                                                           \
                                        delay_cnt--;                                            \
                                    }while(delay_cnt); })                                         \
                                    /*BM_P20_SET;                                                 \
                                    delay_cnt = 1800U;                                          \
                                    do                                                          \
                                    {                                                           \
                                        delay_cnt--;                                            \
                                    }while(delay_cnt); })                                         \
*/


/*#define BM_MODULE_ProgramModeSet()    ({     uint32_t delay_cnt;                                         \
                                    BM_RST_CLEAR;                                                 \
                                    delay_cnt = 1800U;                                          \
                                    do                                                          \
                                    {                                                           \
                                        delay_cnt--;                                            \
                                    }while(delay_cnt);                                          \
                                    BM_P20_CLEAR;                                                 \
                                    delay_cnt = 1800U;                                          \
                                    do                                                          \
                                    {                                                           \
                                        delay_cnt--;                                            \
                                    }while(delay_cnt);                                          \
                                    BM_RST_SET;                                                 \
                                    delay_cnt = 10000u;                                       \
                                    do                                                          \
                                    {                                                           \
                                        delay_cnt--;                                            \
                                    }while(delay_cnt);})
*/

#define BM_MODULE_ApplicationModeSet()    ({     uint32_t delay_cnt;                                         \
                                    BM_RST_CLEAR;                                                 \
                                    delay_cnt = 1800U;                                          \
                                    do                                                          \
                                    {                                                           \
                                        delay_cnt--;                                            \
                                    }while(delay_cnt);                                          \
                                    BM_P20_SET;                                                 \
                                    delay_cnt = 1800U;                                          \
                                    do                                                          \
                                    {                                                           \
                                        delay_cnt--;                                            \
                                    }while(delay_cnt);                                          \
                                    BM_RST_SET;                                                 \
                                    delay_cnt = 1000000u;                                       \
                                    do                                                          \
                                    {                                                           \
                                        delay_cnt--;                                            \
                                    }while(delay_cnt);})


#define BM_MODE_Set(mode)             {                                           \
                                            if(mode == BM_MODE_PROGRAM)            \
                                                BM_MODULE_ProgramModeSet();        \
                                            if(mode == BM_MODE_APPLICATION)       \
                                                BM_MODULE_ApplicationModeSet();   \
                                        }                                           \

    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************


    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************


    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
