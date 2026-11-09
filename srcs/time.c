/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/30 12:29:04 by coren             #+#    #+#             */
/*   Updated: 2026/11/09 13:40:15 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"
#include <sys/time.h>

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