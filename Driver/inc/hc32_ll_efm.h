/**
 *******************************************************************************
 * @file  hc32_ll_efm.h
 * @brief This file contains all the functions prototypes of the EFM driver
 *        library.
 @verbatim
   Change Logs:
   Date             Author          Notes
   2022-03-31       CDT             First version
   2022-10-31       CDT             Add Flash protect level define
   2023-01-15       CDT             Code refine
   2023-09-30       CDT             Add FLASH security addr define
   2023-12-15       CDT             Rename EFM_DataCacheResetCmd() as EFM_CacheRamReset() and modify comment
                                    Optimized macro group EFM_Remap_Size definitions
   2024-06-30       CDT             Move EFM_CACHE_ALL from c file to head file
                                    Add prefix EFM to SECTOR_SIZE and macro EFM_PROTECT_LEVEL_ALL
   2024-10-17       CDT             Add const before buffer pointer to cater top-level calls
 @endverbatim
 *******************************************************************************
 * Copyright (C) 2022-2025, Xiaohua Semiconductor Co., Ltd. All rights reserved.
 *
 * This software component is licensed by XHSC under BSD 3-Clause license
 * (the "License"); You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                    opensource.org/licenses/BSD-3-Clause
 *
 *******************************************************************************
 */
#ifndef __HC32_LL_EFM_H__
#define __HC32_LL_EFM_H__

/* C binding of definitions if building with C++ compiler */
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
 * Include files
 ******************************************************************************/
#include "hc32_ll_def.h"

#include "hc32f4xx.h"
#include "hc32f4xx_conf.h"
/**
 * @addtogroup LL_Driver
 * @{
 */

/**
 * @addtogroup LL_EFM
 * @{
 */

#if (LL_EFM_ENABLE == DDL_ON)

/*******************************************************************************
 * Global type definitions ('typedef')
 ******************************************************************************/
/**
 * @defgroup EFM_Global_Types EFM Global Types
 * @{
 */
/**
 * @brief EFM unique ID definition
 */
typedef struct {
    uint32_t            u32UniqueID0;      /*!< unique ID 0.       */
    uint32_t            u32UniqueID1;      /*!< unique ID 1.       */
    uint32_t            u32UniqueID2;      /*!< unique ID 2.       */
} stc_efm_unique_id_t;

typedef struct {
    uint32_t u32State;
    uint32_t u32Addr;
    uint32_t u32Size;
} stc_efm_remap_init_t;

/**
 * @}
 */

/*******************************************************************************
 * Global pre-processor symbols/macros ('#define')
 ******************************************************************************/
/**
 * @defgroup EFM_Global_Macros EFM Global Macros
 * @{
 */
/**
 * @defgroup EFM_Address EFM Address Area
 * @{
 */
#define EFM_START_ADDR                  (0x00000000UL)    /*!< Flash start address */

#define EFM_END_ADDR                    (0x0007FFFFUL)    /*!< Flash end address */
#define EFM_OTP_START_ADDR              (0x03000C00UL)    /*!< OTP start address */
#define EFM_OTP_END_ADDR                (0x03000FFBUL)    /*!< OTP end address */
#define EFM_OTP_LOCK_ADDR_START         (0x03000FC0UL)    /*!< OTP lock start address */
#define EFM_OTP_LOCK_ADDR_END           (0x03000FFCUL)    /*!< OTP lock end address */
#define EFM_SECURITY_START_ADDR         (0x0317FFE0UL)    /*!< Flash security start address */
#define EFM_SECURITY_END_ADDR           (0x0317FFFFUL)    /*!< Flash security end address */

/**
 * @}
 */

/**
 * @defgroup EFM_Chip_Sel EFM Chip Selection
 * @{
 */
#define EFM_CHIP_ALL                    (EFM_FSTP_FSTP)
/**
 * @}
 */

/**
 * @defgroup EFM_Bus_Status EFM Bus Status
 * @{
 */
#define EFM_BUS_HOLD                    (0x0UL)     /*!< Bus busy while flash program or erase */
#define EFM_BUS_RELEASE                 (0x1UL)     /*!< Bus release while flash program or erase */
/**
 * @}
 */

/**
 * @defgroup EFM_Wait_Cycle EFM Wait Cycle
 * @{
 */

#define EFM_WAIT_CYCLE0                 (0U << EFM_FRMC_FLWT_POS)      /*!< Don't insert read wait cycle */
#define EFM_WAIT_CYCLE1                 (1U << EFM_FRMC_FLWT_POS)      /*!< Insert 1 read wait cycle     */

#define EFM_WAIT_CYCLE2                 (2U << EFM_FRMC_FLWT_POS)      /*!< Insert 2 read wait cycles    */
#define EFM_WAIT_CYCLE3                 (3U << EFM_FRMC_FLWT_POS)      /*!< Insert 3 read wait cycles    */
#define EFM_WAIT_CYCLE4                 (4U << EFM_FRMC_FLWT_POS)      /*!< Insert 4 read wait cycles    */
#define EFM_WAIT_CYCLE5                 (5U << EFM_FRMC_FLWT_POS)      /*!< Insert 5 read wait cycles    */
#define EFM_WAIT_CYCLE6                 (6U << EFM_FRMC_FLWT_POS)      /*!< Insert 6 read wait cycles    */
#define EFM_WAIT_CYCLE7                 (7U << EFM_FRMC_FLWT_POS)      /*!< Insert 7 read wait cycles    */
#define EFM_WAIT_CYCLE8                 (8U << EFM_FRMC_FLWT_POS)      /*!< Insert 8 read wait cycles    */
#define EFM_WAIT_CYCLE9                 (9U << EFM_FRMC_FLWT_POS)      /*!< Insert 9 read wait cycles    */
#define EFM_WAIT_CYCLE10                (10U << EFM_FRMC_FLWT_POS)     /*!< Insert 10 read wait cycles   */
#define EFM_WAIT_CYCLE11                (11U << EFM_FRMC_FLWT_POS)     /*!< Insert 11 read wait cycles   */
#define EFM_WAIT_CYCLE12                (12U << EFM_FRMC_FLWT_POS)     /*!< Insert 12 read wait cycles   */
#define EFM_WAIT_CYCLE13                (13U << EFM_FRMC_FLWT_POS)     /*!< Insert 13 read wait cycles   */
#define EFM_WAIT_CYCLE14                (14U << EFM_FRMC_FLWT_POS)     /*!< Insert 14 read wait cycles   */
#define EFM_WAIT_CYCLE15                (15U << EFM_FRMC_FLWT_POS)     /*!< Insert 15 read wait cycles   */
/**
 * @}
 */

/**
 * @defgroup EFM_Swap_Address EFM Swap Address
 * @{
 */
#define EFM_SWAP_ADDR                   (0x0007FFDCUL)
#define EFM_SWAP_DATA                   (0xFFFF4321UL)
/**
 * @}
 */

/**
 * @defgroup EFM_OperateMode_Sel EFM Operate Mode Selection
 * @{
 */
#define EFM_MD_READONLY                 (0x0UL << EFM_FWMC_PEMOD_POS)   /*!< Read only mode               */
#define EFM_MD_PGM_SINGLE               (0x1UL << EFM_FWMC_PEMOD_POS)   /*!< Program single mode          */
#define EFM_MD_PGM_READBACK             (0x2UL << EFM_FWMC_PEMOD_POS)   /*!< Program and read back mode   */
#define EFM_MD_PGM_SEQ                  (0x3UL << EFM_FWMC_PEMOD_POS)   /*!< Program sequence mode        */
#define EFM_MD_ERASE_SECTOR             (0x4UL << EFM_FWMC_PEMOD_POS)   /*!< Sector erase mode            */

#define EFM_MD_ERASE_ALL_CHIP           (0x5UL << EFM_FWMC_PEMOD_POS)   /*!< Chip erase mode      */

/**
 * @}
 */

/**
 * @defgroup EFM_Flag_Sel  EFM Flag Selection
 * @{
 */
#define EFM_FLAG_PEWERR                 (EFM_FSR_PEWERR)        /*!< EFM Programming/erase error flag.              */
#define EFM_FLAG_PGMISMTCH              (EFM_FSR_PGMISMTCH)     /*!< EFM Programming missing match error flag       */
#define EFM_FLAG_OPTEND                 (EFM_FSR_OPTEND)        /*!< EFM End of operation flag.                     */
#define EFM_FLAG_COLERR                 (EFM_FSR_COLERR)        /*!< EFM Read collide error flag.                   */
#define EFM_FLAG_PEPRTERR               (EFM_FSR_PEPRTERR)      /*!< EFM write protect address error flag           */
#define EFM_FLAG_RDY                    (EFM_FSR_RDY)           /*!< EFM ready flag.                                */
#define EFM_FLAG_PGSZERR                (EFM_FSR_PGSZERR)       /*!< EFM Programming/erase protect area error flag. */

#define EFM_FLAG_ALL                (EFM_FLAG_PEWERR | EFM_FLAG_PGMISMTCH | EFM_FLAG_OPTEND | EFM_FLAG_PEPRTERR | \
                                     EFM_FLAG_COLERR | EFM_FLAG_PGSZERR   | EFM_FLAG_RDY)

/**
 * @}
 */

/**
 * @defgroup EFM_Interrupt_Sel EFM Interrupt Selection
 * @{
 */
#define EFM_INT_PEERR                   (EFM_FITE_PEERRITE)     /*!< Program/erase error Interrupt source    */
#define EFM_INT_OPTEND                  (EFM_FITE_OPTENDITE)    /*!< End of EFM operation Interrupt source   */
#define EFM_INT_COLERR                  (EFM_FITE_COLERRITE)    /*!< Read collide error Interrupt source     */

#define EFM_INT_ALL                     (EFM_FITE_PEERRITE | EFM_FITE_OPTENDITE | EFM_FITE_COLERRITE)
/**
 * @}
 */

/**
 * @defgroup EFM_Cache_Mask EFM Cache Bit Mask
 * @{
 */
#define EFM_CACHE_ALL                   (EFM_FRMC_CRST | EFM_FRMC_CACHE)

/**
 * @}
 */

/**
 * @defgroup EFM_Keys EFM Keys
 * @{
 */
#define EFM_REG_UNLOCK_KEY1             (0x0123UL)
#define EFM_REG_UNLOCK_KEY2             (0x3210UL)
#define EFM_REG_LOCK_KEY                (0x0000UL)
/**
 * @}
 */

/**
 * @defgroup EFM_Sector_Size EFM Sector Size
 * @{
 */
#define EFM_SECTOR_SIZE                 (0x2000UL)
/**
 * @}
 */

/**
 * @defgroup EFM_Sector_Address EFM Sector Address
 * @{
 */
#define EFM_SECTOR_ADDR(x)          (uint32_t)(EFM_SECTOR_SIZE * (x))
/**
 * @}
 */

/**
 * @defgroup EFM_OTP_Base_Address EFM Otp Base Address
 * @{
 */
#define EFM_OTP_BASE1_ADDR          (0x03000C00UL)
#define EFM_OTP_BASE1_SIZE          (0x40UL)
#define EFM_OTP_BASE1_OFFSET        (0UL)
#define EFM_OTP_LOCK_ADDR           (0x03000FC0UL)
/**
 * @}
 */

/**
 * @defgroup EFM_OTP_Address EFM Otp Address
 * @{
 */
#define EFM_OTP_BLOCK0              (EFM_OTP_BASE1_ADDR + ((0UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK1              (EFM_OTP_BASE1_ADDR + ((1UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK2              (EFM_OTP_BASE1_ADDR + ((2UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK3              (EFM_OTP_BASE1_ADDR + ((3UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK4              (EFM_OTP_BASE1_ADDR + ((4UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK5              (EFM_OTP_BASE1_ADDR + ((5UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK6              (EFM_OTP_BASE1_ADDR + ((6UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK7              (EFM_OTP_BASE1_ADDR + ((7UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK8              (EFM_OTP_BASE1_ADDR + ((8UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK9              (EFM_OTP_BASE1_ADDR + ((9UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK10             (EFM_OTP_BASE1_ADDR + ((10UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK11             (EFM_OTP_BASE1_ADDR + ((11UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK12             (EFM_OTP_BASE1_ADDR + ((12UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK13             (EFM_OTP_BASE1_ADDR + ((13UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))
#define EFM_OTP_BLOCK14             (EFM_OTP_BASE1_ADDR + ((14UL - EFM_OTP_BASE1_OFFSET) * EFM_OTP_BASE1_SIZE))

/**
 * @}
 */

/**
 * @defgroup EFM_OTP_Lock_Address EFM Otp Lock_address
 * @note    x at range of 0~14
 * @{
 */
#define EFM_OTP_BLOCK_LOCKADDR(x)    (EFM_OTP_LOCK_ADDR + 0x04UL * (x))   /*!< OTP block x  lock address */
/**
 * @}
 */

/**
 * @defgroup EFM_Remap_Reg_Write_Protection Write Protection Keys For EFM Remap Registers
 * @{
 */
#define EFM_REMAP_REG_LOCK_KEY      (0x0000UL)
#define EFM_REMAP_REG_UNLOCK_KEY1   (0x0123UL)
#define EFM_REMAP_REG_UNLOCK_KEY2   (0x3210UL)
/**
 * @}
 */

/**
 * @defgroup EFM_Remap_State EFM remap function state
 * @{
 */
#define EFM_REMAP_OFF               (0UL)
#define EFM_REMAP_ON                (EFM_MMF_REMCR_EN)
/**
 * @}
 */

/**
 * @defgroup EFM_Remap_Size EFM remap size definition
 * @note refer to chip user manual for details size spec.
 * @{
 */
#define EFM_REMAP_4K                (12UL)
#define EFM_REMAP_8K                (13UL)
#define EFM_REMAP_16K               (14UL)
#define EFM_REMAP_32K               (15UL)
#define EFM_REMAP_64K               (16UL)
#define EFM_REMAP_128K              (17UL)
#define EFM_REMAP_256K              (18UL)
#define EFM_REMAP_512K              (19UL)
#define EFM_REMAP_SIZE_MAX          EFM_REMAP_512K
/**
 * @}
 */

/**
 * @defgroup EFM_Remap_Index EFM remap index
 * @{
 */
#define EFM_REMAP_IDX0              (0U)
#define EFM_REMAP_IDX1              (1U)
/**
 * @}
 */

/**
 * @defgroup EFM_Remap_BaseAddr EFM remap base address
 * @{
 */
#define EFM_REMAP_BASE_ADDR0        (0x2000000UL)
#define EFM_REMAP_BASE_ADDR1        (0x2080000UL)
/**
 * @}
 */

/**
 * @defgroup EFM_Remap_Region EFM remap ROM/RAM region
 * @{
 */
#define EFM_REMAP_ROM_END_ADDR      EFM_END_ADDR

#define EFM_REMAP_RAM_START_ADDR    (0x1FFF8000UL)
#define EFM_REMAP_RAM_END_ADDR      (0x1FFFFFFFUL)
/**
 * @}
 */

/**
 * @defgroup EFM_Protect_Level EFM protect level
 * @{
 */
#define EFM_PROTECT_LEVEL1          (1UL << 0UL)
#define EFM_PROTECT_LEVEL2          (1UL << 1UL)
#define EFM_PROTECT_LEVEL_ALL       (EFM_PROTECT_LEVEL1 | EFM_PROTECT_LEVEL2)

/**
 * @}
 */

/**
 * @defgroup EFM_MCU_Status EFM protect level
 * @{
 */
#define EFM_MCU_PROTECT1_FREE       (0U)
#define EFM_MCU_PROTECT1_LOCK       (1U)
#define EFM_MCU_PROTECT1_UNLOCK     (2U)
#define EFM_MCU_PROTECT2_LOCK       (4U)
/**
 * @}
 */

/**
 * @}
 */
/*******************************************************************************
 * Global variable definitions ('extern')
 ******************************************************************************/

/*******************************************************************************
  Global function prototypes (definition in C source)
 ******************************************************************************/
/**
 * @addtogroup EFM_Global_Functions
 * @{
 */

/**
 * @brief  EFM Protect Unlock.
 * @param  None
 * @retval None
 */

__STATIC_INLINE void EFM_REG_Unlock(void)
{
    WRITE_REG32(CM_EFM->FAPRT, EFM_REG_UNLOCK_KEY1);
    WRITE_REG32(CM_EFM->FAPRT, EFM_REG_UNLOCK_KEY2);
}

/**
 * @brief  EFM Protect Lock.
 * @param  None
 * @retval None
 */
__STATIC_INLINE void EFM_REG_Lock(void)
{
    WRITE_REG32(CM_EFM->FAPRT, EFM_REG_LOCK_KEY);
}

/**
 * @brief  EFM remap Unlock.
 * @param  None
 * @retval None
 */
__STATIC_INLINE void EFM_REMAP_Unlock(void)
{
    WRITE_REG32(CM_EFM->MMF_REMPRT, EFM_REMAP_REG_UNLOCK_KEY1);
    WRITE_REG32(CM_EFM->MMF_REMPRT, EFM_REMAP_REG_UNLOCK_KEY2);
}

/**
 * @brief  EFM remap Lock.
 * @param  None
 * @retval None
 */
__STATIC_INLINE void EFM_REMAP_Lock(void)
{
    WRITE_REG32(CM_EFM->MMF_REMPRT, EFM_REMAP_REG_LOCK_KEY);
}

void EFM_Cmd(uint32_t u32Flash, en_functional_state_t enNewState);
void EFM_FWMC_Cmd(en_functional_state_t enNewState);
void EFM_SetBusStatus(uint32_t u32Status);
void EFM_IntCmd(uint32_t u32EfmInt, en_functional_state_t enNewState);
void EFM_ClearStatus(uint32_t u32Flag);
int32_t EFM_SetWaitCycle(uint32_t u32WaitCycle);
int32_t EFM_SetOperateMode(uint32_t u32Mode);
int32_t EFM_ReadByte(uint32_t u32Addr, uint8_t *pu8ReadBuf, uint32_t u32ByteLen);
int32_t EFM_Program(uint32_t u32Addr, const uint8_t *pu8Buf, uint32_t u32Len);
int32_t EFM_SequenceProgram(uint32_t u32Addr, const  uint8_t *pu8Buf, uint32_t u32Len);
int32_t EFM_ProgramWord(uint32_t u32Addr, uint32_t u32Data);
int32_t EFM_ProgramWordReadBack(uint32_t u32Addr, uint32_t u32Data);
int32_t EFM_ChipErase(uint8_t u8Chip);

int32_t EFM_SectorErase(uint32_t u32Addr);

en_flag_status_t EFM_GetAnyStatus(uint32_t u32Flag);
en_flag_status_t EFM_GetStatus(uint32_t u32Flag);
void EFM_GetUID(stc_efm_unique_id_t *pstcUID);

void EFM_CacheRamReset(en_functional_state_t enNewState);
void EFM_CacheCmd(en_functional_state_t enNewState);
void EFM_LowVoltageReadCmd(en_functional_state_t enNewState);
int32_t EFM_SwapCmd(en_functional_state_t enNewState);
en_flag_status_t EFM_GetSwapStatus(void);
int32_t EFM_OTP_Lock(uint32_t u32Addr);

int32_t EFM_REMAP_StructInit(stc_efm_remap_init_t *pstcEfmRemapInit);
int32_t EFM_REMAP_Init(uint8_t u8RemapIdx, stc_efm_remap_init_t *pstcEfmRemapInit);
void EFM_REMAP_DeInit(void);
void EFM_REMAP_Cmd(uint8_t u8RemapIdx, en_functional_state_t enNewState);
void EFM_REMAP_SetAddr(uint8_t u8RemapIdx, uint32_t u32Addr);
void EFM_REMAP_SetSize(uint8_t u8RemapIdx, uint32_t u32Size);

void EFM_LowVoltageCmd(en_functional_state_t enNewState);

void EFM_SetWindowProtectAddr(uint32_t u32StartAddr, uint32_t u32EndAddr);
void EFM_Protect_Enable(uint8_t u8Level);
int32_t EFM_WriteSecurityCode(const uint8_t *pu8Buf, uint32_t u32Len);

/**
 * @}
 */

#endif /* LL_EFM_ENABLE */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __HC32_LL_EFM_H__ */

/*******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/
