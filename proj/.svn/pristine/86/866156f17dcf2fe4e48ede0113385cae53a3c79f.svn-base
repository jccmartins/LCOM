#pragma once

#include <lcom/lcf.h>

#include "stdint.h"

/** @defgroup rtc rtc
 * @{
 * Constants and functions for the RTC.
 * 
 * \author João Carlos Carreira Martins
 * \author João Francisco de Pinho Brandão
 */

#define RTC_IRQ 8 /**< @brief RTC IRQ line */ 

#define RTC_ADDR_REG 0x70 /**< @brief RTC register address */
#define RTC_DATA_REG 0x71 /**< @brief RTC data address */

#define REG_A 0x0A /**< @brief Register A of the RTC */
#define REG_B 0x0B /**< @brief Register B of the RTC */
#define REG_C 0x0C /**< @brief Register C of the RTC */

#define SECONDS 0x00 /**< @brief Seconds Register of the RTC */
#define SECONDS_ALARM 0x01 /**< @brief Seconds Alarm Register of the RTC */
#define MINUTES 0x02 /**< @brief Minutes Register of the RTC */
#define MINUTES_ALARM 0x03 /**< @brief Minutes Alarm Register of the RTC */
#define HOURS 0x04 /**< @brief Hours Register of the RTC */
#define HOURS_ALARM 0x05 /**< @brief Hours Alarm Register of the RTC */
#define DAY_WEEK 0x06 /**< @brief Day of the week Register of the RTC */
#define DAY_MONTH 0x07 /**< @brief Day of the month Register of the RTC */
#define MONTH 0x08 /**< @brief Month Register of the RTC */
#define YEAR 0x09 /**< @brief Year Register of the RTC */

#define UIP BIT(7) /**< @brief Update in progress flag of Register A */

#define SET BIT(7) /**< @brief SET bit of Register B - if set inhibit updates of time/date registers */
#define PIE BIT(6) /**< @brief Periodic Interrupts */
#define AIE BIT(5) /**< @brief Alarm Interrupts */
#define UIE BIT(4) /**< @brief Update Interrupts */

#define PF BIT(6) /**< @brief Periodic Interrupt Pending */
#define AF BIT(5) /**< @brief Alarm Interrupt Pending */
#define UF BIT(4) /**< @brief Update Interrupt Pending */

/** @brief Stores hours, minutes and seconds of the RTC */
struct Time{
  int hours, minutes, seconds;
};

/** @brief Subscribe RTC interrupts */
int(rtc_subscribe_int)(uint8_t *bit_no);
/** @brief Set Alarm and Update Interrupts of the RTC */
void(set_rtc_interrupts)();
/** @brief Set time of the alarm seconds after time specified in hours, minutes and seconds */
void(set_alarm_seconds_after)(uint32_t hours, uint32_t minutes, uint32_t seconds, uint32_t seconds_after);
/** @brief Unsubscribe RTC interrupts */
int(rtc_unsubscribe_int)();
/** @brief Returns a decimal number converted from binary-coded decimal
 * @param value - binary-coded decimal to be converted
 */
uint32_t(bcd_to_decimal)(uint32_t value);
/** @brief Returns a binary-coded number converted from decimal
 * @param value - decimal number to be converted
 */
uint32_t decimal_to_bcd(uint32_t value);
/** @brief Returns hours from the RTC */
int(getHours)();
/** @brief Returns minutes from the RTC */
int(getMinutes)();
/** @brief Returns seconds from the RTC */
int(getSeconds)();
/** @brief RTC interrupt handler */
void(rtc_ih)();
/** @brief Handles update interrupts */
void(handle_update_int)();
/** @brief Handles alarm interrupts */
void(handle_alarm_int)();
