/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   session.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: coren <coren@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 12:40:17 by coren             #+#    #+#             */
/*   Updated: 2026/11/12 14:39:13 by coren            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_script.h"
#include <stdio.h>

double	get_elapsed_time(t_script_data *data);

int	handle_pty_session(t_script_data *data)
{
	fd_set	read_fds;
	char	buffer[256];
	char	time_buf[64];
	int		bytes_read;

	while (42)
	{
		FD_ZERO(&read_fds);
		FD_SET(STDIN_FILENO, &read_fds);
		FD_SET(data->masterFd, &read_fds);

		if (select(data->masterFd + 1, &read_fds, NULL, NULL, NULL) < 0)
			break;

		// 1. Données venant du clavier (INPUT)
		if (FD_ISSET(STDIN_FILENO, &read_fds))
		{
			bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer));
			if (bytes_read <= 0)
				break;
				
			write(data->masterFd, buffer, bytes_read); // Envoi au shell
			
			// Log de l'entrée (-I)
			if (data->logInFd > 0)
				write(data->logInFd, buffer, bytes_read);
		}

		// 2. Données venant du shell (OUTPUT & TIMING)
		if (FD_ISSET(data->masterFd, &read_fds))
		{
			bytes_read = read(data->masterFd, buffer, sizeof(buffer));
			if (bytes_read <= 0)
				break; 
			
			write(STDOUT_FILENO, buffer, bytes_read); // Affichage à l'écran
			
			// Log de la sortie (-O) ou fichier par défaut (typescript)
			if (data->logOutFd > 0)
				write(data->logOutFd, buffer, bytes_read);
				
			// Log du chronométrage (-T)
			if (data->logTimingFd > 0)
			{
				double elapsed = get_elapsed_time(data);
				if (elapsed == -1)
					return FAILED;
					
				// Temporairement avec sprintf
				int len = sprintf(time_buf, "%f %d\n", elapsed, bytes_read);
				write(data->logTimingFd, time_buf, len);
			}
		}
	}
	return SUCCES;
}