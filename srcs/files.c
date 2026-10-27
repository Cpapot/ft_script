/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   files.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/27 14:01:19 by coren             #+#    #+#             */
/*   Updated: 2026/10/27 15:11:34 by coren            ###   ########.fr       */
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

//close all fd
void	close_all_files(t_script_data data)
{
	if (data.targetFd)
		close(data.targetFd);
}

int		create_all_files(t_script_data *data)
{
	int flags = data->settings.isAppend ? O_CREAT | O_WRONLY | O_APPEND :  O_CREAT | O_WRONLY | O_TRUNC;
	if (create_unique_file(data->settings.target, flags, &(data->targetFd)))
		return FAILED;
	return SUCCES;
}