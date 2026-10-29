/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 14:01:19 by coren             #+#    #+#             */
/*   Updated: 2026/10/29 13:12:23 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "files.h"

static int	open_unique_file(char *name, int flags, int *fdDest)
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

int		set_slave_path(t_script_data *data)
{
	int		ptyId = 0;
	char	buffer[11];

	
	if (ioctl(data->masterFd, TIOCGPTN, &ptyId) == -1)
	{
		//TODO PRINT ERROR
		return FAILED;
	}
	ft_itoa_r(ptyId, buffer, 11);
	ft_strlcat(data->slavePath, "/dev/pts/", PATH_MAX);
	ft_strlcat(data->slavePath, buffer, PATH_MAX);

	return SUCCES;
}

int		init_pty_lock(t_script_data *data)
{
	if (ioctl(data->masterFd, TIOCSPTLCK, &data->unlockPty) == -1)
	{
		//TODO PRINT ERROR
		return FAILED;
	}
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
	if (data.masterFd)
		close(data.masterFd);
}

int		open_all_files(t_script_data *data)
{
	int flags = O_CREAT | O_WRONLY | (data->settings.isAppend ? O_APPEND : O_TRUNC) | (data->settings.isFlush ? O_SYNC : 0);
	if (data->settings.logIn && open_unique_file(data->settings.logIn, flags, &(data->logInFd)))
		return FAILED;
	if (data->settings.logTiming && open_unique_file(data->settings.logTiming, flags, &(data->logTimingFd)))
		return FAILED;
	if (data->settings.logOut && open_unique_file(data->settings.logOut, flags, &(data->logOutFd)))
		return FAILED;
	if (open_unique_file("/dev/ptmx", O_RDWR | O_NOCTTY, &(data->masterFd)))
		return FAILED;
	
	return SUCCES;
}