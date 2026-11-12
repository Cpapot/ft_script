/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 12:29:04 by coren             #+#    #+#             */
/*   Updated: 2026/11/12 13:31:06 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"
#include <sys/time.h>

static void append_num(char **ptr, int num, int digits) {
    char *p = *ptr + digits - 1;
    for (int i = 0; i < digits; i++) {
        *p-- = '0' + (num % 10);
        num /= 10;
    }
    *ptr += digits;
}

static int is_leap_year(int year) {
    return (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
}

int	init_time(t_script_data *data)
{
	if (gettimeofday((struct timeval *)(data->lastTimeStruct), NULL) == -1)
		return FAILED; //TODO PRINT ERROR
	return SUCCES;
}

double	get_elapsed_time(t_script_data *data)
{
	if (gettimeofday((struct timeval *)(data->currentTimeStruct), NULL) == -1)
		return -1; //TODO PRINT ERROR
	struct timeval *current = (struct timeval *)(data->currentTimeStruct);
	struct timeval *last = (struct timeval *)(data->lastTimeStruct);

	double elapsed = (current->tv_sec - last->tv_sec) + (current->tv_usec - last->tv_usec) / 1000000.0;	
	ft_memcpy(&(data->lastTimeStruct), &(data->currentTimeStruct), TIMEVAL_MAX);
	
	return elapsed;
}

void get_strtime(char *dest, t_script_data *data)
{
	int 	hourOffset = 1;
	int		minsOffset = 0;
	char	buffer[32];
	struct timeval *tv = (struct timeval *)(data->lastTimeStruct);
	if (tv == NULL)
		return;
	char *p = buffer;

    long long days = (tv->tv_sec + (hourOffset * 3600) + (minsOffset * 60)) / 86400;
    long long time_in_day = (tv->tv_sec + (hourOffset * 3600) + (minsOffset * 60)) % 86400;
    int hours = time_in_day / 3600;
    int minutes = (time_in_day % 3600) / 60;
    int seconds = time_in_day % 60;

    int year = 1970;
    while (1)
	{
        int days_this_year = is_leap_year(year) ? 366 : 365;
        if (days >= days_this_year)
		{
            days -= days_this_year; year++;
        }
		else
			break;
    }
	
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (is_leap_year(year))
        days_in_month[1] = 29;

    int month = 0;
    for (month = 0; month < 12; month++)
	{
        if (days >= days_in_month[month])
            days -= days_in_month[month];
		else
        	break;
    }
    
    // YYYY-MM-DD
    append_num(&p, year, 4);
    *p++ = '-';
    append_num(&p, month + 1, 2);
    *p++ = '-';
    append_num(&p, days + 1, 2);
    *p++ = ' ';

    // HH:MM:SS
    append_num(&p, hours, 2);
    *p++ = ':';
    append_num(&p, minutes, 2);
    *p++ = ':';
    append_num(&p, seconds, 2);

    // (+HH:MM)
    *p++ = (hourOffset >= 0) ? '+' : '-';
    append_num(&p, (hourOffset >= 0) ? hourOffset : -hourOffset, 2);
    *p++ = ':';
    append_num(&p, (minsOffset >= 0) ? minsOffset : -minsOffset, 2);
    *p++ = '\0';
	ft_strlcat(dest, buffer, 64);
}