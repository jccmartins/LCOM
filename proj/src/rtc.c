#include "rtc.h"
#include "graphics.h"

static int rtc_hook_id = 3;

struct Time *rtc_time = NULL;

int(rtc_subscribe_int)(uint8_t *bit_no)
{

    *bit_no = rtc_hook_id;

    if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &rtc_hook_id) != 0)
    {
        return 1;
    }

    return 0;
}

int(rtc_unsubscribe_int)()
{
    if (sys_irqrmpolicy(&rtc_hook_id) != 0)
    {
        return 1;
    }

    return 0;
}

void(set_rtc_interrupts)()
{
    uint32_t regB;
    sys_outb(RTC_ADDR_REG, REG_B);
    sys_inb(RTC_DATA_REG, &regB);
    regB = regB | UIE | AIE; //update-ended and alarm interrupts
    sys_outb(RTC_ADDR_REG, REG_B);
    sys_outb(RTC_DATA_REG, regB);
}

void(set_alarm_seconds_after)(uint32_t hours, uint32_t minutes, uint32_t seconds, uint32_t seconds_after)
{

    uint32_t total_seconds = hours * 3600 + minutes * 60 + seconds + seconds_after;
    uint32_t seconds_in_a_day = 24 * 60 * 60;
    uint32_t seconds_in_a_hour = 60 * 60;
    uint32_t seconds_in_a_minute = 60;

    // remove 1 day if total time is over 1 day
    total_seconds = total_seconds % seconds_in_a_day;

    hours = total_seconds / seconds_in_a_hour;
    total_seconds = total_seconds % seconds_in_a_hour;

    minutes = total_seconds / seconds_in_a_minute;
    total_seconds = total_seconds % seconds_in_a_minute;

    seconds = total_seconds;

    sys_outb(RTC_ADDR_REG, HOURS_ALARM);
    hours = decimal_to_bcd(hours);
    sys_outb(RTC_DATA_REG, hours);

    sys_outb(RTC_ADDR_REG, MINUTES_ALARM);
    minutes = decimal_to_bcd(minutes);
    sys_outb(RTC_DATA_REG, minutes);

    sys_outb(RTC_ADDR_REG, SECONDS_ALARM);
    seconds = decimal_to_bcd(seconds);
    sys_outb(RTC_DATA_REG, seconds);
}

uint32_t(bcd_to_decimal)(uint32_t value)
{
    return ((value / 16 * 10) + (value % 16));
}

uint32_t decimal_to_bcd(uint32_t value)
{
    unsigned int ones = 0;
    unsigned int tens = 0;
    unsigned int temp = 0;

    ones = value % 10;
    temp = value / 10;
    tens = temp << 4;

    return (tens + ones);
}

int(getHours)()
{
    uint32_t hours;
    sys_outb(RTC_ADDR_REG, HOURS);
    sys_inb(RTC_DATA_REG, &hours);
    return bcd_to_decimal(hours);
}

int(getMinutes)()
{
    uint32_t minutes;
    sys_outb(RTC_ADDR_REG, MINUTES);
    sys_inb(RTC_DATA_REG, &minutes);
    return bcd_to_decimal(minutes);
}

int(getSeconds)()
{
    uint32_t seconds;
    sys_outb(RTC_ADDR_REG, SECONDS);
    sys_inb(RTC_DATA_REG, &seconds);
    return bcd_to_decimal(seconds);
}

void(rtc_ih)(bool *alarm_interrupt)
{

    uint32_t regC;
    sys_outb(RTC_ADDR_REG, REG_C);
    sys_inb(RTC_DATA_REG, &regC);
    if (regC & UF)
    {
        handle_update_int();
    }
    if (regC & AF)
    {
        printf("AF INTERRUPT\n");
        handle_alarm_int(alarm_interrupt);
    }
}

void(handle_update_int)()
{
    uint32_t regB;
    sys_outb(RTC_ADDR_REG, REG_B);
    sys_inb(RTC_DATA_REG, &regB);
    regB = regB | SET;
    sys_outb(RTC_ADDR_REG, REG_B);
    sys_outb(RTC_DATA_REG, regB);

    if (rtc_time == NULL)
    {
        rtc_time = (struct Time *)malloc(sizeof(struct Time));
    }

    rtc_time->hours = getHours();
    rtc_time->minutes = getMinutes();
    rtc_time->seconds = getSeconds();

    sys_outb(RTC_ADDR_REG, REG_B);
    sys_inb(RTC_DATA_REG, &regB);
    regB = regB & ~SET;
    sys_outb(RTC_ADDR_REG, REG_B);
    sys_outb(RTC_DATA_REG, regB);
}

void(handle_alarm_int)(bool *alarm_interrupt)
{
    *alarm_interrupt = true;
}
