/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 14:01:19 by coren             #+#    #+#             */
/*   Updated: 2026/10/28 10:35:25 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "files.h"

static int	create_unique_file(char *name, int flags, int *fdDest)
{
	int fd = open(name, flags, 0644);
	if (fd < 0)
	{
		if (errno == EACCES)
			print_error(EACCES_ERROR, name, false, 0);
		else if (errno == EISDIR)
			print_error(EISDIR_ERROR, name, false, 0);
		else if (errno == ENOENT)
			print_error(ENOENT_ERROR, name, false, 0);
		else
			print_error(OPEN_ERROR, name, false, 0);
		return FAILED;
	}
	(*fdDest) = fd;
	return SUCCES;
}

void	close_all_files(t_script_data data)
{
	if (data.logOutFd)
		close(data.logOutFd);
	if (data.logInFd)
		close(data.logInFd);
	if (data.logTimingFd)
		close(data.logTimingFd);
}

int		create_all_files(t_script_data *data)
{
	int flags = O_CREAT | O_WRONLY | (data->settings.isAppend ? O_APPEND : O_TRUNC) | (data->settings.isFlush ? O_SYNC : 0);
	if (data->settings.logIn && create_unique_file(data->settings.logIn, flags, &(data->logInFd)))
		return FAILED;
	if (data->settings.logTiming && create_unique_file(data->settings.logTiming, flags, &(data->logTimingFd)))
		return FAILED;
	if (data->settings.logOut && create_unique_file(data->settings.logOut, flags, &(data->logOutFd)))
		return FAILED;
	return SUCCES;
}