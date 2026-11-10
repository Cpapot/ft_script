/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/09 13:38:17 by coren             #+#    #+#             */
/*   Updated: 2026/11/09 14:07:03 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"
#include <sys/ioctl.h>

int	open_unique_file(char *name, int flags, int *fdDest);

int	child_process(t_script_data *data)
{
	close(data->masterFd);
	setsid();
	
	int slave_fd;
	if (open_unique_file(data->slavePath, O_RDWR, &slave_fd))
		return FAILED;
	
	ioctl(slave_fd, TIOCSCTTY, 0); //TODO check error on all
	dup2(slave_fd, STDIN_FILENO);
    dup2(slave_fd, STDOUT_FILENO);
    dup2(slave_fd, STDERR_FILENO);
	close(slave_fd);

	char *args[] = {"/bin/sh", NULL};
    execve("/bin/sh", args, NULL); //check error

	return SUCCES;
}