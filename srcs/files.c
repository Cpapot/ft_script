/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 14:01:19 by coren             #+#    #+#             */
/*   Updated: 2026/10/28 09:49:54 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "files.h"

static int	create_unique_file(char *name, int flags, int *fdDest)
{
	ft_printf("creating %s\n", name);
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

}

int		create_all_files(t_script_data *data)
{
	int flags = data->settings.isAppend ? O_CREAT | O_WRONLY | O_APPEND :  O_CREAT | O_WRONLY | O_TRUNC;
	if ((data->settings.logIn && create_unique_file(data->settings.logIn, flags, &(data->logInFd))) || !data->settings.logIn)
		return FAILED;
	if ((data->settings.logTiming && create_unique_file(data->settings.logTiming, flags, &(data->logTimingFd))) || !data->settings.logTiming)
		return FAILED;
	if ((data->settings.logOut && create_unique_file(data->settings.logOut, flags, &(data->logOutFd))) || !data->settings.logTiming)
		return FAILED;
	return SUCCES;
}